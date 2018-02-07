
/****************************************************************************/
/*! \file TestHimalayaMasterThread/Include/TestHimalayaMasterThreadController.h
 *
 *  \brief Definition file for class TestHimalayaMasterThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-02-21
 *  $Author:    $ BhanuPrasad Hulsoor
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

#ifndef HIMALAYA_TESTHIMALAYAMASTERTHREADCONTROLLER_H
#define HIMALAYA_TESTHIMALAYAMASTERTHREADCONTROLLER_H

#include <Threads/Include/MasterThreadController.h>
#include <PasswordManager/Include/PasswordManager.h>
#include <TestStubDataManager/Containers/UserSettings/Include/UserSettings.h>
#include <Global/Include/Commands/CmdDateAndTime.h>
#include <Global/Include/Commands/AckOKNOK.h>
#include <Global/Include/Commands/CmdByteArray.h>
#include "HimalayaStateHandler.h"
#include "../../HimalayaDataManager/Include/DataManager.h"
#include <HimalayaDataContainer/Containers/StainingReagents/Commands/Include/CmdReagentUpdate.h>
#include <TestStubDataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h>
#include <TestStubDataManager/Containers/ExportConfiguration/Commands/Include/CmdDataExport.h>
#include <TestStubDataManager/Containers/ExportConfiguration/Commands/Include/CmdDataImport.h>
#include <HimalayaDataContainer/Containers/StainingReagents/Commands/Include/CmdReagentAdd.h>
#include <HimalayaDataContainer/Containers/StainingReagents/Commands/Include/CmdReagentRemove.h>
#include <TestStubDataManager/Containers/UserSettings/Include/UserSettingsInterface.h>
#include <NetCommands/Include/CmdExecutionStateChanged.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include "CommandExecuter/Include/CommandExecuter.h"
#include <TestStubDataManager/Containers/DeviceConfiguration/Include/DeviceConfiguration.h>
#include <Qt/qtextbrowser.h>
//#include <DeviceCommandProcessor/Include/Commands/CmdLoaderUpdate.h>
#include <NetCommands/Include/CmdChangeUserLevel.h>
#include <NetCommands/Include/CmdGuiInit.h>
#include "ProgramStartableFlagManager.h"
#include <NetCommands/Include/CmdDataUpdate.h>

namespace NetCommands {
    class CmdAcknEventReport;
}
namespace Himalaya {

typedef enum  {
    UNDEFINED_HIMALAYA_THREAD,
    HIMALAYA_GUI_THREAD,
    SCHEDULER_THREAD,
    DEVICE_CMD_PROCESSOR_THREAD,
    AXEDA_CONTROLLER_THREAD,
    SEPIA_CONTROLLER_THREAD,
    EXPORT_CONTROLLER_THREAD,
    IMPORT_EXPORT_THREAD,
    BLG_CONTROLLER_THREAD
} HimalayaThreads_t;

/****************************************************************************/
/**
 * \brief Controller for master thread for Test SW for Himalaya GUI application.
 *
 * This is the controller for master thread. It has to create Himalaya GUI
 * controller and thread, initialize them, start and stop them and finally
 * destroy them.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class HimalayaMasterThreadController : public Threads::MasterThreadController {
    Q_OBJECT
private:
    /****************************************************************************/
    /**
     * \brief \todo Replace with correct brain functionality
     */
    /****************************************************************************/
    enum ProtoBrainState {
        PBS_Started,                    ///< ProtoBrain was started.
        PBS_WaitingForDCInit,           ///< Waiting for device control init.
        PBS_WaitingForGrappler1RefRun,  ///< Waiting for grappler 1 reference run.
        PBS_WaitingForGrappler2RefRun,  ///< Waiting for grappler 2 reference run.
        PBS_WaitingForLoaderRefRun,     ///< Waiting for loader reference run.
        PBS_WaitingForUnloaderRefRun,   ///< Waiting for unloader reference run.
        PBS_NormalOperation             ///< Waiting for grappler 2 reference run.
    };                          
    Threads::CommandChannel         m_CommandChannelGui;                    ///< Command channel for Gui thread controller.
    Threads::CommandChannel         m_CommandChannelScheduler;              ///< Command channel for Scheduler thread controller.
    Threads::CommandChannel         m_CommandChanngelDevice;                ///< Command channel for the device

    /****************************************************************************/
    HimalayaMasterThreadController(const HimalayaMasterThreadController &);                     ///< Not implemented.
    const HimalayaMasterThreadController & operator = (const HimalayaMasterThreadController &); ///< Not implemented.

    Global::tRefType m_Ref;             ///< To check ACK ref
    DataManager::CDataProgramList m_ProgramList;  //!< Global Program list
    DataManager::CProgram   m_Program;  ///< Global Program object
    DataManager::CDataManager  *mp_DataManager; //!< Global Station list
    QByteArray *mp_ByteArray;                   //!<  Global Byte array pointer
    CommandExecuter::CommandExecuter *mp_CommandExecuter;   ///< Container of commands with time instance to execute the particular command
    NetCommands::CmdExecutionStateChanged *mp_CmdExecutionStateChanged; ///< CmdExecutionStateChanged objects
    bool m_ChangeLanguage;                    ///< bool value to change the language
    HimalayaStateHandler *mp_stateHandler;    ///< Himalaya State Handler
    QList<int> RefList;                       ///< Commands reference list
    ProgramStartableManager          m_ProgramStartableManager;              ///< Object Managing Program Startablity


    /****************************************************************************/
    /**
     * \brief Register commands from device command processor
     */
    /****************************************************************************/
    virtual void RegisterDeviceCommands();
    /****************************************************************************/
    /**
     * \brief Register commands.
     */
    /****************************************************************************/
    virtual void RegisterSchedulerCommands();

    /****************************************************************************/
    /**
     * \brief Register commands.
     */
    /****************************************************************************/
    virtual void RegisterCommands();
    /****************************************************************************/
    /**
     * \brief Start statemachine.
     *
     * Start own statemachine and start to work.
     *
     */
    /****************************************************************************/
    virtual void StartStatemachine();
    /****************************************************************************/
    /**
     * \brief Send DataContainers to scheduler
     */
    /****************************************************************************/
    virtual void SendContainersToScheduler();

    /****************************************************************************/
    /**
     * \brief Set new date and time offset.
     *
     *\iparam   Ref                 Command reference.
     *\iparam   Cmd                 Command.
     *\iparam   AckCommandChannel   Command channel for acknowledges.
     */
    /****************************************************************************/
    void SetDateTime(Global::tRefType Ref, const Global::CmdDateAndTime &Cmd, Threads::CommandChannel &AckCommandChannel);


   /****************************************************************************/
    /**
     * \brief Acknowledge of type AckOKNOK received.
     *
     *\iparam   Ref     Command reference.
     *\iparam   Ack     Acknowledge.
     */
    /****************************************************************************/
    void OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack);

    /****************************************************************************/
    /**
     * \brief Handler for acknowledged.
     *
     *\iparam  Ref -- unique command reference.
     *\iparam  Ack -- received acknowledge instance.
     */
    /****************************************************************************/
    void OnEventReportAck(Global::tRefType Ref, const NetCommands::CmdAcknEventReport &Ack);

    /****************************************************************************/
    /**
     * \brief Handler for CmdDataExport
     *
     * \iparam Ref - Refernce of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void ExportDataFileHandler(Global::tRefType Ref, const MsgClasses::CmdDataExport &Cmd,Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Handler for CmdDataImport.
     *
     * \iparam Ref - Refernce of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void ImportDataFileHandler(Global::tRefType Ref, const MsgClasses::CmdDataImport &Cmd,Threads::CommandChannel &AckCommandChannel);
    /****************************************************************************/
    /*!
     *  \brief   This handler is called on reception of "CmdGuiInit", The remaining
                 threads are created on reception of this command.
     *  \iparam Ref = Command reference
     *  \iparam Cmd = Command
     *  \AckCommandChannel = Command channel to send acknowledge
     */
    /****************************************************************************/
    void OnCmdGuiInitHandler(Global::tRefType Ref, const NetCommands::CmdGuiInit &Cmd, Threads::CommandChannel &AckCommandChannel);

    /************************************************************************************/
    /*!
     *  \brief Change User level Handler Handler
     *
    */
    /************************************************************************************/
    void ChangeUserLevelHandler(Global::tRefType Ref, const NetCommands::CmdChangeUserLevel &Cmd, Threads::CommandChannel &AckCommandChannel);


protected:
    /****************************************************************************/
    /**
     * \brief Create all controllers and threads.
     *
     * First of all base class' method is called then own controllers are created.
     */
    /****************************************************************************/
    virtual void CreateControllersAndThreads();
    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     */
    /****************************************************************************/
    virtual void OnPowerFail();
    /****************************************************************************/
    /**
     * \brief Initiate the shutdown process.
     *
     * Initiate the shutdown process. Make some project specific tasks the call
     * \ref Shutdown to shut down software.
     */
    /****************************************************************************/
    virtual void InitiateShutdown();
    /****************************************************************************/
    /**
     * \brief This method is called when the base received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread.\n
     * We create all controllers used (the according method in derived classes
     * \ref CreateControllersAndThreads is also called), then initialize the controllers
     * and finally attach them to the corresponding threads and start them.\n
     * If something goes wrong, the master thread (and application) will be stopped.
     */
    /****************************************************************************/
    virtual void OnGoReceived();

public:
    /****************************************************************************/
    /**
     * \brief Default Constructor.
     */
    /****************************************************************************/
    HimalayaMasterThreadController();
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~HimalayaMasterThreadController();

    /****************************************************************************/
    /**
     * \brief Wraps the Himalaya StateHandler's CheckOperation function
     */
    /****************************************************************************/
    void CheckOperation() { mp_stateHandler->checkOperation();}
    /****************************************************************************/
    /**
     * \brief Return the command channel object requested
     * \return CommandChannel object
     */
    /****************************************************************************/
    inline Threads::CommandChannel & GetCommandChannel() {
        return m_CommandChannelGui;
    }


    /****************************************************************************/
    /**
     * \brief Sends the XML files to HimalayaGUI
     *\iparam   TextBrowser = Text Browser Reference.
     * \return true if the xml files sending succeeded, false otherwise
     */
    /****************************************************************************/
    bool SendXML(QTextBrowser &TextBrowser);
    /****************************************************************************/
     /**
      * \brief Send the language file to GUI
      *
      * \param FileName - Name of the file
      *
      * \return On successful (True) or not (false)
      */
     /****************************************************************************/
     bool SendLanguageFileToGUI(QString FileName);
    /****************************************************************************/
    /**
     * \brief Update the GUI language files (qm files from translation directory)
     */
    /****************************************************************************/
    bool UpdateSupportedGUILanguages();
    /****************************************************************************/
    void SendMessage(Global::Command *p_Command);

    /****************************************************************************/
    /**
     * \brief Get the type of the import mentioned in config file.
     *
     * \iparam FilePath - Path of the importConfig file
     * \return Type of the import
     */
    /****************************************************************************/
    QString GetImportType(QString FilePath);

    /****************************************************************************/
    /*!
     *  \brief Sends the language file to the GUI
     *\iparam   TextBrowser = Text Browser Reference
     * \return true if the Change in language succeeded, false otherwise
     */
    /****************************************************************************/
    bool SendChangeLanguage(QTextBrowser &TextBrowser);

    /****************************************************************************/
    /*!
     *  \brief Set the new date and time and send it to the GUI
     * \return true if the Date and time is set, false otherwise
     */
    /****************************************************************************/
    bool SetDateAndTime(QDateTime &DateTime);

    /****************************************************************************/
    /*!
     *  \brief SendEventMessage : Sending different event messages to the GUI
     *\iparam   Type = Event Message type.
     *\iparam   ButtonType = Event message Button type
     *\iparam   StatusBarIcon = Display in statusbar yes/no
     *\iparam   EventString = Event message string
     * \return true if the sequence excecution succeeded, false otherwise
    */
    /****************************************************************************/
    bool SendEventMessage(int Type, Global::GuiButtonType ButtonType, bool StatusBarIcon,QString EventString);

    /****************************************************************************/
    /**
     * \brief Send the config files for import.
     *
     * \return true if success
     */
    /****************************************************************************/
    bool ConfigFilesImport();
    /****************************************************************************/
    /**
     * \brief Send the Leica programs/Reagent for import.
     *
     * \return true if success
     */
    /****************************************************************************/
    bool LeicaProgImport();

    /****************************************************************************/
    /*!
     *  \brief SendRemoveMessageBox : Removing event messages from the Message queue
     * \iparam   EventID = Event Id of the message to be removed
     * \return true if the sequence excecution succeeded, false otherwise
    *
    */
    /****************************************************************************/
    bool SendRemoveMessageBox(quint64 EventID);

    /****************************************************************************/
    /**
     * \brief Create and initialize used objects.
     *
     * We try to read the hardware configuration file.
     */
    /****************************************************************************/
    void CreateAndInitializeObjects();

private slots:
    void ClearHeap();
    /****************************************************************************/
    /*!
     *  \brief Sends Process State command with state set to running
     */
    /****************************************************************************/
    void SendProcessStart();
    /****************************************************************************/
    /*!
     *  \brief Sends process state command with state set to stopped
     */
    /****************************************************************************/
    void SendProcessStop();

signals:
    /****************************************************************************/
    /*!
     *  \brief SendAckToTextBrowser: This signal is emitted, when ack needs to be
     *   written to the text browser
     */
    /****************************************************************************/
    void SendAckToTextBrowser(QString );

}; // end class HimalayaMasterThreadController

} // end namespace Himalaya

#endif // HIMALAYA_TESTHIMALAYAMASTERTHREADCONTROLLER_H
