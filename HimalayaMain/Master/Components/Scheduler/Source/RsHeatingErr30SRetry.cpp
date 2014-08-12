/****************************************************************************/
/*! \file RsHeatingErr30SRetry.cpp
 *
 *  \brief CRsHeatingErr30Retry class definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ June 12th, 2014
 *   $Author:  $ Songtao Yu
 *
 *  \b Company:
 *
 *       Leica Biosystems SH CN.
 *
 *  (C) Copyright 2014 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "Scheduler/Include/RsHeatingErr30SRetry.h"
#include "Scheduler/Include/SchedulerMainThreadController.h"
#include "Scheduler/Include/SchedulerCommandProcessor.h"
#include "Scheduler/Include/HeatingStrategy.h"

namespace Scheduler{

/****************************************************************************/
/*!
 *  \brief    Constructor
 *  \iparam   pStateMachine: Pointer to current state machine
 *  \iparam   pParentState: Pointer to parent state
 */
/****************************************************************************/
CRsHeatingErr30SRetry::CRsHeatingErr30SRetry(SchedulerMainThreadController* SchedController)
    :mp_SchedulerController(SchedController)
{
    mp_StateMachine = QSharedPointer<QStateMachine>(new QStateMachine());
    mp_Initialize = QSharedPointer<QState>(new QState(mp_StateMachine.data()));
    mp_ReleasePressure = QSharedPointer<QState>(new QState(mp_StateMachine.data()));
    mp_ShutdownFailedHeater = QSharedPointer<QState>(new QState(mp_StateMachine.data()));
    mp_WaitFor10Seconds = QSharedPointer<QState>(new QState(mp_StateMachine.data()));
    mp_RestartFailedHeater = QSharedPointer<QState>(new QState(mp_StateMachine.data()));
    mp_CheckTempModuleCurrent = QSharedPointer<QState>(new QState(mp_StateMachine.data()));


    mp_StateMachine->setInitialState(mp_Initialize.data());
    mp_Initialize->addTransition(this, SIGNAL(ReleasePressureSig()), mp_ReleasePressure.data());
    mp_ReleasePressure->addTransition(this, SIGNAL(ShutdownFailedHeaters()), mp_ShutdownFailedHeater.data());
    mp_ShutdownFailedHeater->addTransition(this, SIGNAL(WaitFor10Seconds()), mp_WaitFor10Seconds.data());
    mp_WaitFor10Seconds->addTransition(this, SIGNAL(RestartFailedHeater()), mp_RestartFailedHeater.data());
    mp_RestartFailedHeater->addTransition(this, SIGNAL(CheckTempModuleCurrernt()), mp_CheckTempModuleCurrent.data());
    mp_CheckTempModuleCurrent->addTransition(this, SIGNAL(Retry()), mp_ShutdownFailedHeater.data());
    mp_CheckTempModuleCurrent->addTransition(this, SIGNAL(TasksDone(bool)), mp_ShutdownFailedHeater.data());

    CONNECTSIGNALSLOT(mp_ReleasePressure.data(), entered(), mp_SchedulerController, ReleasePressure());

    // For error cases
    mp_WaitFor10Seconds->addTransition(this, SIGNAL(TasksDone(bool)), mp_ShutdownFailedHeater.data());
    mp_RestartFailedHeater->addTransition(this, SIGNAL(TasksDone(bool)), mp_ShutdownFailedHeater.data());
    m_ShutdownHeaterTime = 0;
    m_StartTime = 0;
    m_Counter = 0;

    mp_StateMachine->start();
}

/****************************************************************************/
/*!
 *  \brief    Destructor
 */
/****************************************************************************/
CRsHeatingErr30SRetry::~CRsHeatingErr30SRetry()
{
    mp_StateMachine->stop();
}

/****************************************************************************/
/*!
 *  \brief   Return the current state of the state machine
 *  \iparam statesList = A list contains the current state, usually get from
 *                       the State Machine current states belogs to
 *
 *  \return The current state of the state machine
 */
/****************************************************************************/
CRsHeatingErr30SRetry::StateList_t CRsHeatingErr30SRetry::GetCurrentState(QSet<QAbstractState*> statesList)
{
    StateList_t currentState = UNDEF;
    if(statesList.contains(mp_Initialize.data()))
    {
        currentState = HEATINGERROR_30SRetry_INIT;
    }
    else if(statesList.contains(mp_ReleasePressure.data()))
    {
        currentState = RELEASE_PRESSURE;
    }
    else if(statesList.contains(mp_ShutdownFailedHeater.data()))
    {
        currentState = SHUTDOWN_FAILD_HEATER;
    }
    else if(statesList.contains(mp_WaitFor10Seconds.data()))
    {
        currentState = WAIT_FOR_10SECONDS;
    }
    else if(statesList.contains(mp_RestartFailedHeater.data()))
    {
        currentState = RESTART_FAILED_HEATER;
    }
    else if (statesList.contains(mp_CheckTempModuleCurrent.data()))
    {
        currentState = CHECK_MODULE_CURRENT;
    }
    return currentState;
}

void CRsHeatingErr30SRetry::HandleWorkFlow(const QString& cmdName, ReturnCode_t retCode)
{
    StateList_t currentState = this->GetCurrentState(mp_StateMachine->configuration());
    qint64 now = 0;

    switch (currentState)
    {
    case HEATINGERROR_30SRetry_INIT:
        mp_SchedulerController->LogDebug("RS_HeatingErr_30SRetry, in state INITIALIZE");
        emit ReleasePressureSig();
        break;
    case RELEASE_PRESSURE:
        mp_SchedulerController->LogDebug("RS_HeatingErr_30SRetry, in state RELEASE_PRESSURE");
        if ("Scheduler::ALReleasePressure" == cmdName)
        {
            // We always return failure
            if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
            {
                emit TasksDone(false);
            }
            else
            {
                emit ShutdownFailedHeaters();
            }
        }
        break;
    case SHUTDOWN_FAILD_HEATER:
        mp_SchedulerController->LogDebug("RS_HeatingErr_30SRetry, in state SHUTDOWN_FAILD_HEATER");
        if (true == mp_SchedulerController->ShutdownFailedHeaters())
        {
            m_ShutdownHeaterTime = QDateTime::currentMSecsSinceEpoch();
            emit WaitFor10Seconds();
        }
        else
        {
            mp_SchedulerController->LogDebug("RS_HeatingErr_30SRetry, SHUTDOWN_FAILD_HEATER failed");
            emit TasksDone(false);
        }
        break;
    case WAIT_FOR_10SECONDS:
        now = QDateTime::currentMSecsSinceEpoch();
        if ((now - m_ShutdownHeaterTime) >= 10*1000)
        {
            emit RestartFailedHeater();
        }
        break;
    case RESTART_FAILED_HEATER:
        if (true == mp_SchedulerController->RestartFailedHeaters())
        {
            m_StartTime = QDateTime::currentMSecsSinceEpoch();
            emit CheckTempModuleCurrernt();
        }
        else
        {
            emit TasksDone(false);
        }
        break;
    case CHECK_MODULE_CURRENT:
        now = QDateTime::currentMSecsSinceEpoch();
        if ((now - m_StartTime) > 3*1000)
        {
            emit TasksDone(true);
        }
        else
        {
            if (false ==  mp_SchedulerController->CheckSlaveTempModulesCurrentRange(3)
                    || false == mp_SchedulerController->CheckSensorTempOverange())
            {
                m_Counter++;
                if (3 == m_Counter)
                {
                    emit TasksDone(false);
                }
                else
                {
                    emit Retry();
                }
            }
        }
        break;
    default:
        break;
    }
}



}
