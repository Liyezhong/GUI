/****************************************************************************/
/*! \file DashboardStationCommandInterface.h
 *
 *  \brief Dashboard station Command Interface definition
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-22
 *  $Author:    $ Swati Tiwari
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
#ifndef DashboardStationCommandInterface_H
#define DashboardStationCommandInterface_H

//#include "CommandInterfaceBase.h"

#include "TestStubDataManager/CommandInterface/Include/CommandInterfaceBase.h"
#include "../../HimalayaDataManager/Include/DataManager.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdDashboardStationUpdate.h"
#include "NetCommands/Include/CmdConfigurationFile.h"

namespace DataManager {
class CDashboardStationList;
class CDashboardStation;
/****************************************************************************/
/**
 * \brief  Reagent container command interface definition
 */
/****************************************************************************/
class CDashboardStationCommandInterface:public CCommandInterfaceBase
{
    Q_OBJECT
public:
    CDashboardStationCommandInterface(CDataManager *p_DataManager, Threads::MasterThreadController *p_MasterThreadController, CDataContainer *p_DataContainer);

private:
    void RegisterCommands();
    void UpdateDashboardStation(Global::tRefType, const MsgClasses::CmdDashboardStationUpdate &Cmd, Threads::CommandChannel &AckCommandChannel);

};

}// end of namespace DataManager

#endif // REAGENTCOMMANDINTERFACE_H
