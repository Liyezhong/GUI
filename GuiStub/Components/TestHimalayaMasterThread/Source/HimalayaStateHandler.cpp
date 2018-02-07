/****************************************************************************/
/*! \file HimalayaMasterThread/Source/HimalayaMasterThreadController.cpp
 *
 *  \brief Implementation file for class HimalayaStateHandler.
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

#include "../Include/HimalayaStateHandler.h"
#include "Global/Include/Commands/CmdEventUpdate.h"

#include <QAbstractTransition>

namespace Himalaya {


/****************************************************************************/
HimalayaStateHandler::HimalayaStateHandler(DataManager::CDataManager *dataManager)
    : mp_dataManager(dataManager)
{
    m_idleState = new QState();
    m_idleState->setObjectName("IdleState");
    m_busyState = new QState();
    m_busyState->setObjectName("BusyState");

    m_errorState = new QState();
    m_errorState->setObjectName("ErrorState");
    m_normalState = new QState();
    m_normalState->setObjectName("NormalState");

    m_operationMachine.addState(m_idleState);
    m_operationMachine.addState(m_busyState);

    m_availabilityMachine.addState(m_errorState);
    m_availabilityMachine.addState(m_normalState);

//    QAbstractTransition *transition = m_idleState->addTransition(m_errorState);

    m_busyState->addTransition(this, SIGNAL(enterIdleState()), m_idleState);
    m_idleState->addTransition(this, SIGNAL(enterBusyState()), m_busyState);

    m_errorState->addTransition(this, SIGNAL(enterNormalState()), m_normalState);
    m_normalState->addTransition(this, SIGNAL(enterErrorState()), m_errorState);

    m_operationMachine.setInitialState(m_idleState);
    m_availabilityMachine.setInitialState(m_normalState);

//    connect(m_idleState, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));
//    connect(m_busyState, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));

    connect(m_idleState, SIGNAL(entered()), this, SLOT(onOperationStateChanged()));
    connect(m_busyState, SIGNAL(entered()), this, SLOT(onOperationStateChanged()));
    connect(m_errorState, SIGNAL(entered()), this, SLOT(onAvailabilityStateChanged()));
    connect(m_normalState, SIGNAL(entered()), this, SLOT(onAvailabilityStateChanged()));

//    connect(m_errorState, SIGNAL(entered()), this, SLOT(onErrorState()));
//    connect(m_normalState, SIGNAL(entered()), this, SLOT(onNormalState()));

    m_operationMachine.start();
    m_availabilityMachine.start();
}

/****************************************************************************/
HimalayaStateHandler::~HimalayaStateHandler()
{
}

bool HimalayaStateHandler::isAllowed(Global::CommandShPtr_t command)
{
    bool retVal = true;

    foreach (QAbstractState* state, m_operationMachine.configuration())
    {
        if (!command.GetPointerToUserData()->isStateAllowed(state->objectName()))
            retVal = false;
    }

    foreach (QAbstractState* state, m_availabilityMachine.configuration())
    {
        if (!command.GetPointerToUserData()->isStateAllowed(state->objectName()))
            retVal = false;
    }

    return retVal;
}

QString HimalayaStateHandler::getCurrentOperationState()
{
    if (!m_operationMachine.configuration().empty())
    {
        QAbstractState *state = m_operationMachine.configuration().toList().at(0);
        return state->objectName();
    }
    return "";
}

QString HimalayaStateHandler::getCurrentAvailabilityState()
{
    if (!m_availabilityMachine.configuration().empty())
    {
        QAbstractState *state = m_availabilityMachine.configuration().toList().at(0);
        return state->objectName();
    }
    return "";
}

void HimalayaStateHandler::checkOperation()
{
    if (!mp_dataManager)
        return;

}

void HimalayaStateHandler::setAvailability(bool active, quint64 eventId)
{
    if (active)
    {
        m_errorList.append(eventId);
        if (m_errorList.count() == 1)
            emit enterErrorState();
    }
    else
    {
        m_errorList.removeOne(eventId);
        if (m_errorList.count() == 0)
            emit enterNormalState();
    }
}

void HimalayaStateHandler::onAvailabilityStateChanged()
{
    QString state = getCurrentAvailabilityState();
    emit stateChanged(state);
}

void HimalayaStateHandler::onOperationStateChanged()
{
    QString state = getCurrentOperationState();
    emit stateChanged(state);
}

//void HimalayaStateHandler::onNormalState()
//{
//    DeviceCommandProcessor::CmdExecutionStateChanged *p_CmdExecutionState = new DeviceCommandProcessor::CmdExecutionStateChanged(DeviceControl::DEVICE_INSTANCE_ID_GRAPPLER_1);
//    p_CmdExecutionState->m_Stop = false;
////    p_CmdExecutionState->m_eventId = Cmd.m_eventId;
//    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CmdExecutionState));

//    NetCommands::CmdExecutionStateChanged *p_CmdExecutionStateGUI = new NetCommands::CmdExecutionStateChanged(1000);
//    p_CmdExecutionStateGUI->m_Stop = false;
//    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CmdExecutionStateGUI));
//}

//void HimalayaStateHandler::onErrorState()
//{
//    DeviceCommandProcessor::CmdExecutionStateChanged *p_CmdExecutionState = new DeviceCommandProcessor::CmdExecutionStateChanged(DeviceControl::DEVICE_INSTANCE_ID_GRAPPLER_1);
//    p_CmdExecutionState->m_Stop = true;
////    p_CmdExecutionState->m_eventId = Cmd.m_eventId;
//    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CmdExecutionState));

//    NetCommands::CmdExecutionStateChanged *p_CmdExecutionStateGUI = new NetCommands::CmdExecutionStateChanged(1000);
//    p_CmdExecutionStateGUI->m_Stop = true;
//    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_CmdExecutionStateGUI));
//}

//HimalayaStartupState::HimalayaStartupState(HimalayaThreadController *threadController)
//    : QState()
//    , m_threadController(threadController)
//{

//}

//void HimalayaStartupState::onEntry(QEvent* event)
//{
//    // doing reference runs

//    qDebug() << "HimalayaStartupState::onEntry" << this->objectName();
////    emit SIGNAL(stateChanged());
//}

//void HimalayaStartupState::onExit(QEvent* event)
//{
//    qDebug() << "HimalayaState::onExit" << this->objectName();
////    emit SIGNAL(stateChanged());
//}

} // end namespace Himalaya
