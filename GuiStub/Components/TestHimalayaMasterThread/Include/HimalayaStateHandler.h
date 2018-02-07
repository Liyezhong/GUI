/****************************************************************************/
/*! \file HimalayaMasterThread/Include/HimalayaStateHandler.h
 *
 *  \brief Definition file for class HimalayaStateHandler.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-17
 *  $Author:    $ M.Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef HIMALAYA_HIMALAYASTATEHANDLER_H
#define HIMALAYA_HIMALAYASTATEHANDLER_H

#include <QStateMachine>
#include "Global/Include/Commands/Command.h"
#include "../../HimalayaDataManager/Include/DataManager.h"
#include "Global/Include/Commands/CmdEventUpdate.h"

namespace Himalaya {

//class HimalayaState;

/****************************************************************************/
/**
 * \brief State handler for Himalaya application.
 */
/****************************************************************************/
class HimalayaStateHandler : public QObject
{
    Q_OBJECT

public:
    HimalayaStateHandler(DataManager::CDataManager *dataManager);
    ~HimalayaStateHandler();

    /**
      @brief Checks whether a specific command is allowed in current state
      */
    bool isAllowed(Global::CommandShPtr_t command);

    /**
      @brief Updates the current state by checking availability of racks and errors
      */
    void checkOperation();


//    void checkError(Global::CommandShPtr_t command);
//    void checkError(Global::CmdEventUpdate *commandPtr);

    /**
      @brief Adds or removes the given eventId to or from list of errors. Triggers then a transition to required state (normal or error)
      */
    void setAvailability(bool active, quint64 eventId);
    QString getCurrentOperationState();
    QString getCurrentAvailabilityState();

private:
    QStateMachine m_operationMachine;
    QStateMachine m_availabilityMachine;

    QState *m_idleState;
    QState *m_busyState;

    QState *m_normalState;
    QState *m_errorState;

    //QState *m_startupState;
    DataManager::CDataManager *mp_dataManager;
    QList<quint64> m_errorList;

private slots:
    void onAvailabilityStateChanged();
    void onOperationStateChanged();

//    /**
//      @brief Is called when error state is entered. Will inform GUI and scheduler about stopping the execution
//      */
//    void onErrorState();

//    /**
//      @brief Is called when normal state is entered, the system is error free. Will inform GUI and scheduler about resuming the execution
//      */
//    void onNormalState();

signals:
    void enterErrorState();
    void enterNormalState();

    void enterIdleState();
    void enterBusyState();

    void stateChanged(QString);

}; // end class

//class HimalayaStartupState : public QState
//{
//    Q_OBJECT
//public:
//    HimalayaStartupState(HimalayaMasterThreadController *threadController);

//protected:
//    virtual void onEntry(QEvent* event);
//    virtual void onExit(QEvent* event);

//private:
//    HimalayaMasterThreadController *m_threadController;

////signals:
////    void stateChanged();
//};


} // end namespace Himalaya

#endif // HIMALAYA_HIMALAYASTATEHANDLER_H
