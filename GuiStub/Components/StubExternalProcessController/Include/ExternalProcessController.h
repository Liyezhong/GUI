/****************************************************************************/
/*! \file ExternalProcessController.h
 *
 *  \brief Definition file for class ExternalProcessController.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.12.2010
 *   $Author:  $ Y.Novak
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef EXTERNALPROCESSCONTROL_EXTERNALPROCESSCONTROLLER_H
#define EXTERNALPROCESSCONTROL_EXTERNALPROCESSCONTROLLER_H

#include <QTimer>
#include <StubThreads/Include/ThreadController.h>
#include "../Include/ExternalProcessDevice.h"
#include "../Include/ExternalProcess.h"
#include "../Include/ExternalProcessControlEventCodes.h"
#include <StateMachines/Include/StateMachineEngine.h>
#include <QHash>

// Inform Lint about cleanup functions:
//
//lint -sem(ExternalProcessControl::ExternalProcessController::DeleteActiveObjects, cleanup)
//lint -sem(ExternalProcessControl::ExternalProcessController::DeleteActiveStateObjects, cleanup)

namespace ExternalProcessControl {

class Initial;
class ExtProcessStartRetry;
class FatalError;
class Working;
class CommunicationRetry;
class Final;
class WaitState;

/// Existing External Process events
typedef enum {
    EP_START_OPERATION = 1,          ///<  1: Power up and go live.
    EP_CANNOT_START_DEVICE,          ///<  2: ExternalProcessDevice cannot start -> we're dead
    EP_CANNOT_START_EXTPROCESS,      ///<  3: ExternalProcess cannot start -> try to restart it X times
    EP_CANNOT_KILL_EXTPROCESS,       ///<  4: Killing ExternalProcess failed -> we're dead
    EP_NULL_CTRL_POINTER,            ///<  5: Pointer to working object is NULL -> we're dead
    EP_EXTPROCESS_CONNECTED,         ///<  6: yahoooo! ExternalProcess connected!
    EP_EXTPROCESS_DISCONNECTED,      ///<  7: oops, ExternalProcess disconnected ( Network down, but the process still runs )
    EP_EXTPROCESS_EXITED,            ///<  8: External Process exited ( usually follows after N7 :-] )
    EP_EXTPROCESS_RESTART_FAILED,    ///<  9: Tried to restart external process, but failed -> we're dead
    EP_ONSTOP_RECEIVED,              ///< 10: OnStop received -> stop.
    EP_EXTPROCESS_LOGIN_TIMEOUT,     ///< 11: External Process runs, but failed to login so far -> restart it
    EP_COMMUNICATION_RETRY,          ///< 12: Network communication problem
    EP_POWERDOWN,                    ///< 13: User pressed PowerDown button -> stop.
    EP_SIGNALCONNECT_FAILED,         ///< 14: QObject::connect failed -> we're dead
    EP_TOO_MANY_RESTARTS             ///< 15: External process keeps crashing and restarting -> we're dead
} ExternalProcessStateEvents_t;

typedef Global::SharedPointer<StubThreads::CreatorFunctor>  CreatorFunctorShPtr_t;  ///< Typedef or a shared pointer of CreatorFunctor.
typedef QHash<QString, CreatorFunctorShPtr_t>           CreatorFunctorHash_t;   ///< Typedef for the CreatorFunctorShPtr_t functor hash.

/****************************************************************************/
/**
 * \brief This class handles state machine and TCP communication with the
 *        device's external process. It manages two major members and
 *        their events:
 *         - ExternalProcessDevice (TCP server)
 *         - ExternalProcess (OS Process)
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class ExternalProcessController : public StubThreads::ThreadController
{
    Q_OBJECT

    /// ExternalProcess states are all friends:
    friend class Initial;
    friend class ExtProcessStartRetry;
    friend class FatalError;
    friend class Working;
    friend class CommunicationRetry;
    friend class Final;
    friend class WaitState;

    /// Test classes are all friends:
    friend class DerivedController;
    friend class TestExternalProcessCtlr;
    friend class TestExternalProcessCtlr2;

private:

    ExternalProcessController();                                                      ///< Not implemented.
    ExternalProcessController(const ExternalProcessController &);                     ///< Not implemented.
    const ExternalProcessController & operator = (const ExternalProcessController &); ///< Not implemented.
    bool LocalProcessErrorEvent(const DataLogging::DayEventEntry & TheDayEventEntry);

    bool InitializeProcessManager();
    bool InitializeExternalProcessDevice(const QString &remoteLoginEnabled, int remoteLoginTimeout);
    bool InitializeFullStateMachine();
    bool InitializeSimplifiedStateMachine();
    bool LoadSettings();
    void DeleteActiveObjects();
    void DeleteActiveStateObjects();
    void RegisterCreatorFunctor(const QString &ClassName, const CreatorFunctorShPtr_t &Functor);
    CreatorFunctorShPtr_t GetCreatorFunctor(const QString &CmdName) const;
    /****************************************************************************/
    /**
     * \brief Send message (command or acknowledge) to external process.
     *
     * Send a message (command or acknowledge) to the external process.
     *
     * \param[in]   Ref     Message reference.
     * \param[in]   Msg     Message to send.
     */
    /****************************************************************************/
    template<class MSG> void SendMsgToExternalProcess(Global::tRefType Ref, const MSG &Msg) {
        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, __PRETTY_FUNCTION__, Global::NO_NUMERIC_DATA, false);
        // serialize data
        QByteArray Data;
        // create data stream
        QDataStream DS(&Data, QIODevice::WriteOnly);
        // set version
        DS.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        // write class name
        DS << MSG::NAME;
        // write Ref
        DS << Ref;
        // serialize contents
        DS << Msg;
        // send data. if message fails, NetLayer will emit signal with the Ref supplied in this call.
        m_myDevice->SendOutgoingCommand(MSG::SERIALIZERSTRING, Data, Ref);
    }

protected:

    virtual void OnGoReceived();
    virtual void OnStopReceived();
    /****************************************************************************/
    /**
     * \brief Register a network message for processing.
     *
     * Register a network message (command or acknowledge) for processing.
     *
     * \param[in]   FunctionPointer     The function pointer to register.
     * \param[in]   pProcessorInstance  The instance which will process the message.
     */
    /****************************************************************************/
    template<class MsgClass, class ProcessorClass>
    void RegisterExternalMessage(void(ProcessorClass::*FunctionPointer)(Global::tRefType, MsgClass *), ProcessorClass *pProcessorInstance)
    {
        // create functor
        CreatorFunctorShPtr_t Functor(new StubThreads::TemplateCreatorFunctor<MsgClass, ProcessorClass> (pProcessorInstance, FunctionPointer));
        // and register
        RegisterCreatorFunctor(MsgClass::NAME, Functor);
    }

    void CreateAndDeserialize(const QByteArray &Data);

    /****************************************************************************/
    /**
     * \brief This method is called when External Process is ready to work.
     *
     *        This means we are in the "Working" state.
     */
    /****************************************************************************/
    virtual void OnReadyToWork() = 0;

    virtual void OnStopWorking() {};

    /****************************************************************************/
    /**
     * \brief Send command to external process.
     *
     * Send a command to the external process. Reference mapping does not
     * have to be done since the acknowledge we receive from the external
     * process corresponds with Ref.
     *
     * \param[in]   Ref                 Command reference.
     * \param[in]   Cmd                 Command to send.
     */
    /****************************************************************************/
    template<class CmdClass> void SendCmdToExternalProcess(Global::tRefType Ref, const CmdClass &Cmd) {
        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, __PRETTY_FUNCTION__
                  , Global::NO_NUMERIC_DATA, false);
        qDebug()<<"Sending command to external process:" << Cmd.NAME;
        SendMsgToExternalProcess<CmdClass>(Ref, Cmd);
    }

    /****************************************************************************/
    /**
     * \brief Send acknowledge to external process.
     *
     * Send an acknowledge to the external process. The received reference Ref
     * is the one from sending the command to master thread controller so it
     * must be mapped to the one from the received command from the external
     * process.
     *
     * \param[in]   Ref     Acknowledge reference.
     * \param[in]   Ack     Acknwoledge to send.
     */
    /****************************************************************************/
    template<class AckClass> void SendAckToExternalProcess(Global::tRefType Ref, const AckClass &Ack) {
        // check if we know Ref
        if(!m_RefMapper.contains(Ref)) {
            // error unknown reference
            LOGANDTHROWARG(EVENT_EXTERNALPROCESSCONTROL_ERROR_UNKNOWN_REFERENCE, QString::number(Ref));

        }
        // get old reference
        Global::tRefType OldRef = m_RefMapper.value(Ref);
        // remove from list of references
        m_RefMapper.remove(Ref);
        /* The below logic is added to prevent forwarding of Ack sent by main
          to External Process controller Thread (Not to External Process)
        */
        if (m_RefInternalCommands.contains(Ref)) {
            int Index = m_RefInternalCommands.indexOf(Ref);
            m_RefInternalCommands.removeAt(Index);
            return;
        }
        SendMsgToExternalProcess<AckClass>(OldRef, Ack);
    }

    /****************************************************************************/
    /**
     * \brief Forward command from external process.
     *
     * Forward command from external process. When sending the command, we get a
     * new reference which we must remember, so we can provide the correct Ref
     * when we receive the acknowledge from the master thread controller.
     *
     * \param[in]   Ref     Command reference.
     * \param[in]   pCmd    Command to send.
     */
    /****************************************************************************/
    template<class CmdClass> void ForwardCmdFromExternalProcess(Global::tRefType Ref, CmdClass *pCmd) {
        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, __PRETTY_FUNCTION__
                  , Global::NO_NUMERIC_DATA, false);
        SendCmd(Ref, pCmd);
    }

    /****************************************************************************/
    /**
     * \brief Send Command to Main Controller
     *
     * \param[in]   Ref         Command reference.
     * \param[in]   pCmd        Command to send.
     * \param[in]   ExternalCmd True - Command was sent from External process,
     *                          False - Locally created command
     */
    /****************************************************************************/
    void SendCmd(Global::tRefType Ref, Global::Command *pCmd, bool ExternalCmd = true) {
        CHECKPTR(pCmd);
         /// \todo JB: what about timeout and so on?
        Global::tRefType NewRef ;
        if (ExternalCmd) {
            NewRef = GetNewCommandRef();
        }
        else {
            NewRef = Ref;
        }
        if(m_RefMapper.contains(NewRef)) {
            // first insert then throw exeption
            /// \todo JB: insert and throw or just log error?
            m_RefMapper.insert(NewRef, Ref);
            LOGANDTHROWARGS(EVENT_EXTERNALPROCESSCONTROL_ERROR_REFERENCE_ALREADY_REGISTERED, Global::tTranslatableStringList() <<
                      QString::number(NewRef));
        }
        // insert reference
        m_RefMapper.insert(NewRef, Ref);
        // Create a list of commands which are sent by External process controller and
        // not External Process. This will be used to prevent forwarding of Acks ment for
        // External Process Controller to External Process.
        if (!ExternalCmd) {
            m_RefInternalCommands.append(Ref);
        }
        SendCommand(NewRef, Global::CommandShPtr_t(pCmd));
    }

    /****************************************************************************/
    /**
     * \brief Forward acknowledge from external process.
     *
     * Forward acknowledge from external process.
     *
     * \param[in]   Ref     Acknowledge reference.
     * \param[in]   pAck    Acknowledge to send.
     */
    /****************************************************************************/
    template<class AckClass> void ForwardAckFromExternalProcess(Global::tRefType Ref, AckClass *pAck) {
        LOG_EVENT(Global::EVTTYPE_INFO, Global::LOG_ENABLED, EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE, __PRETTY_FUNCTION__
                  , Global::NO_NUMERIC_DATA, false);
        CHECKPTR(pAck);
        SendAcknowledge(Ref, Global::AcknowledgeShPtr_t(pAck));
    }

    /****************************************************************************/
    /**
     * \brief Sets whether the process has to be restarted or not.
     *
     * \param[in]   StartFlag     Start flag value.
     */
    /****************************************************************************/
    inline void SetRestartProcess(bool StartFlag) {
        m_RestartProcess = StartFlag;
    }


public:

    ExternalProcessController(const QString &prname, Global::gSourceType TheHeartBeatSource);
    virtual ~ExternalProcessController();
    void RegisterExternalProcessDevice(ExternalProcessDevice *gd);
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();
    virtual bool ForwardMsgToRecepient(const QByteArray &bArr);
    QString GetProcessName() { return m_ProcessName; }

signals:
    void ProcessExited(const QString &, int);

public slots:

    void ExternalProcessConnected(const QString &str);
    void ExternalProcessDisconnected(const QString &str);
    void LoginTimeout();

    void ExternalProcessExited(const QString &, int);
    void ExternalProcessStarted(const QString &);
    void ExternalProcessError(int);

private:

    ExternalProcessDevice  *m_myDevice;     ///< Network Device for communication with ExternalProcess
    ExternalProcess        *m_myProcess;    ///< External process manager
    QString                 m_myCommand;    ///< Process start command (as string)
    QString                 m_ProcessName;  ///< Name of the managed process
    /*! Shall Master wait till remote process loggs in or shall Master start local process: Yes(for remote)/No(for local) */
    QString                 m_RemoteLoginEnabled;
    /*! Timeout for remote login option, in seconds. Zero means infinite waiting. */
    int                     m_RemoteLoginTimeout;

    /// State machine related stuff:
    StateMachines::StateMachineEngine           *m_myStateManager;              ///< The State machine engine
    Initial                                     *m_InitialState;                ///< First state to run
    ExtProcessStartRetry                        *m_ExtProcessStartRetryState;   ///< State where we Controller tries to restart Ext.Process
    FatalError                                  *m_FatalErrorState;             ///< State to goto in case of Fatal Error
    Working                                     *m_WorkingState;                ///< Normal operation state
    CommunicationRetry                          *m_CommunicationRetryState;     ///< State where we Controller tries to reconnect
    Final                                       *m_FinalState;                  ///< Power down state
    WaitState                                   *m_WaitState;                   ///< State to wait for incoming connections
    CreatorFunctorHash_t                        m_CreatorFunctors;              ///< Creator functors.
    QHash<Global::tRefType, Global::tRefType>   m_RefMapper;                    ///< Mapper for references.
    QList<Global::tRefType>                     m_RefInternalCommands;          ///< Reference of command sent from this thread controller to Main,not from external process
    bool                                        m_RestartProcess;               ///< Restart flag
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_EXTERNALPROCESSCONTROLLER_H
