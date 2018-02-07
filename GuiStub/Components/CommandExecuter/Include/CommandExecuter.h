/****************************************************************************/
/*! \file CommandExecuter/Include/CommandExecuter.h
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
#ifndef COMMANDEXECUTER_H
#define COMMANDEXECUTER_H

#include "Global/Include/Commands/Command.h"
#include <QObject>

namespace Himalaya {
    class HimalayaMasterThreadController;
}

namespace CommandExecuter {

/****************************************************************************/
/**
 * \brief Class for executing commands in the queue
 */
/****************************************************************************/
class CommandExecuter :public QObject
{
    Q_OBJECT
public:
    explicit CommandExecuter(Himalaya::HimalayaMasterThreadController *p_HimalayaMasterThreadCtrlr);
    ~CommandExecuter(){};
    void AddCommandToQueue(Global::Command *p_Command, qint32 CommandExecutionTime);

private:
    QList<Global::Command *> m_CommandList;     //!< List of commands
    //!< List of times, when the command is executed.This list shall be arranged in ascending order.
    QList<int> m_ListOfCommandExecutionTime;
    Himalaya::HimalayaMasterThreadController *mp_HimalayaMasterThreadCtrlr;

private slots:
    void ExecuteCommand();
};

}//End of namespace CommandExecuter
#endif // COMMANDEXECUTER_H
