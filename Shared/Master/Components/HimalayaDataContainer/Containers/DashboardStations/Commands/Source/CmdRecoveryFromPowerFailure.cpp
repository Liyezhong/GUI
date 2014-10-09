/****************************************************************************/
/*! \file CmdRecoveryFromPowerFailure.cpp
 *
 *  \brief CmdRecoveryFromPowerFailure command implementation.
 *
 *   $Version: $ 0.1
*   $Date:    $ 30.09.2014
 *   $Author:  $ Logan Yang
 *
 *  \b Company:
 *
  *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdRecoveryFromPowerFailure.h"
namespace MsgClasses {

QString CmdRecoveryFromPowerFailure::NAME = "MsgClasses::CmdRecoveryFromPowerFailure";


CmdRecoveryFromPowerFailure::CmdRecoveryFromPowerFailure(int timeout, const QString& ProgramID,
                                                         int StepIndex, int Scenario, int RemainingTime,
                                                         QString RgbID, const QList<QString>& StationList) :
    Command(timeout),
    m_ProgramID(ProgramID), m_StepIndex(StepIndex),m_Scenario(Scenario),
    m_RemainingTime(RemainingTime),m_LastReagentGroupID(RgbID),m_StationList(StationList)
{
}

CmdRecoveryFromPowerFailure::CmdRecoveryFromPowerFailure()
    : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRecoveryFromPowerFailure::~CmdRecoveryFromPowerFailure()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRecoveryFromPowerFailure::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses