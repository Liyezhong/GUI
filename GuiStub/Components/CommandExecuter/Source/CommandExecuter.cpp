/****************************************************************************/
/*! \file CommandExecuter/Source/CommandExecuter.cpp
 *
 *  \brief Implementation file for class BaseLogger.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-03-20
 *  $Author:    $ N.Kamath
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

#include <TestHimalayaMasterThread/Include/TestHimalayaMasterThreadController.h>
#include "CommandExecuter/Include/CommandExecuter.h"
#include <QTimer>
#include <QDebug>
#include <QtAlgorithms>

namespace CommandExecuter {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
*/
/****************************************************************************/
CommandExecuter::CommandExecuter(Himalaya::HimalayaMasterThreadController *p_HimalayaMasterThreadCtrlr)
{
    m_CommandList.clear();
    m_ListOfCommandExecutionTime.clear();
    mp_HimalayaMasterThreadCtrlr = p_HimalayaMasterThreadCtrlr;
}

/****************************************************************************/
/*!
 *  \brief Adds Commands to the execution queue.Commands  will be sorted in
 *         ascending order, based on the time they need to be sent.
 *  \iparam p_Command               = Command to insert in queue
 *  \iparam CommandExecutionTime    = Time at which the command should be sent
 *                                    to GUI
*/
/****************************************************************************/
void CommandExecuter::AddCommandToQueue(Global::Command *p_Command, qint32 CommandExecutionTime)
{
    if ((!m_CommandList.isEmpty()) && (!m_ListOfCommandExecutionTime.isEmpty())) {
        m_ListOfCommandExecutionTime.append(CommandExecutionTime);
        //Sort execution time queue in ascending order
        qStableSort(m_ListOfCommandExecutionTime.begin(), m_ListOfCommandExecutionTime.end());

        if (CommandExecutionTime == m_ListOfCommandExecutionTime.at(0)) {
            m_CommandList.prepend(p_Command);
        }
        else {
            //Sorts the Command list according to execution time
            for (qint32 I = 0 ; I < m_ListOfCommandExecutionTime.count(); I++) {
                if (CommandExecutionTime != m_ListOfCommandExecutionTime.at(I)) {
                    continue;
                }
                else {
                    m_CommandList.insert(I, p_Command);
                    break;
                }
            }
        }

    }
    else {
        m_CommandList.append(p_Command);
        m_ListOfCommandExecutionTime.append(CommandExecutionTime);
    }

    QTimer::singleShot(1000 * CommandExecutionTime, this, SLOT(ExecuteCommand()));
}

/****************************************************************************/
/*!
 *  \brief Executes the commands present in the command queue
 */
 /****************************************************************************/
void CommandExecuter::ExecuteCommand()
{
    // printing queue values only for debuging , remove after testing.
    for (qint32 I =0 ; I < m_ListOfCommandExecutionTime.count(); I++) {
        qDebug()<<"time queue"<<m_ListOfCommandExecutionTime.at(I);
    }
    if (!m_CommandList.isEmpty()) {
        mp_HimalayaMasterThreadCtrlr->SendMessage(m_CommandList.takeFirst());
        m_ListOfCommandExecutionTime.takeFirst();
    }
}

}//End of namespace CommandExecuter
