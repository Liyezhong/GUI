/****************************************************************************/
/*! \file DashboardStationCommandInterface.cpp
 *
 *  \brief Reagent Command Interface  Implementation
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
#include "HimalayaDataManager/CommandInterface/Include/DashboardStationCommandInterface.h"
#include <QDebug>

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataManager = Datamanager object
 *  \iparam p_HimalayaMasterThreadController = Himalaya Master thread Cntrl
 *  \iparam p_DataContainer = DataContainer pointer
 */
/****************************************************************************/
CDashboardStationCommandInterface::CDashboardStationCommandInterface(CDataManager *p_DataManager, Threads::MasterThreadController *p_MasterThreadController, CDataContainer *p_DataContainer):
    CCommandInterfaceBase(p_DataManager, p_MasterThreadController, p_DataContainer)
{
//    try {
//        CONNECTSIGNALSLOT(p_DataManager, DashboardStationUpdate(CDashboardStation &), this , UpdateDashboardStation(CDashboardStation &));
//    }
//    catch (...) {
//        qDebug()<<"Connecting signal slot failed";
//    }

    RegisterCommands();
}

/****************************************************************************/
/**
 * \brief Register Commands related to Reagent Container
 */
/****************************************************************************/
void CDashboardStationCommandInterface::RegisterCommands() {
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdDashboardStationUpdate, DataManager::CDashboardStationCommandInterface>
            (&CDashboardStationCommandInterface::UpdateDashboardStation, this);
}

/****************************************************************************/
/**
 * \brief Function which handles CmdReagentUpdate
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CDashboardStationCommandInterface::UpdateDashboardStation(Global::tRefType Ref, const MsgClasses::CmdDashboardStationUpdate &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    QByteArray DashboardStationData(const_cast<QByteArray &>(Cmd.GetDashboardStationUpdateData()));
    QDataStream DashboardStationDataStream(&DashboardStationData, QIODevice::ReadWrite);
    DashboardStationDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    CDashboardStation DashboardStation;
    DashboardStationDataStream >> DashboardStation;
    bool Result = true;
    Result = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->DashboardStationList->UpdateDashboardStation(&DashboardStation);
    if (!Result) {
        // If error occurs , get errors and send errors to GUI
        mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel);
    }
    else {
        //BroadCast Command
        mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
        DashboardStationData.clear();
        DashboardStationDataStream.device()->reset();
        DashboardStationDataStream << DashboardStation;
        MsgClasses::CmdDashboardStationUpdate* p_Command = new MsgClasses::CmdDashboardStationUpdate(1000, DashboardStationDataStream);
        mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_Command));
        qDebug()<<"\n\n Update Reagent Success";
    }
    static_cast<DataManager::CDataContainer*>(mp_DataContainer)->DashboardStationList->Write();
}

}// End of namespace DataManager



