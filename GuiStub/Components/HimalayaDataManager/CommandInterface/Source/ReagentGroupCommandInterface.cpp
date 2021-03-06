/****************************************************************************/
/*! \file ReagentGroupCommandInterface.cpp
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
#include "HimalayaDataManager/CommandInterface/Include/ReagentGroupCommandInterface.h"
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
CReagentGroupCommandInterface::CReagentGroupCommandInterface(CDataManager *p_DataManager, Threads::MasterThreadController *p_MasterThreadController, CDataContainer *p_DataContainer):
    CCommandInterfaceBase(p_DataManager, p_MasterThreadController, p_DataContainer)
{
//    try {
//        CONNECTSIGNALSLOT(p_DataManager, ReagentUpdate(CReagent &), this , UpdateReagent(CReagent &));
//        CONNECTSIGNALSLOT(p_DataManager, ReagentContainerUpdate(CDataReagentList &), this , UpdateReagentContainer(CDataReagentList &));
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
void CReagentGroupCommandInterface::RegisterCommands() {
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdReagentGroupUpdate, DataManager::CReagentGroupCommandInterface>
            (&CReagentGroupCommandInterface::UpdateReagentGroup, this);
}


/****************************************************************************/
/**
 * \brief Function which handles CmdReagentUpdate
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CReagentGroupCommandInterface::UpdateReagentGroup(Global::tRefType Ref, const MsgClasses::CmdReagentGroupUpdate &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    QByteArray ReagentGroupData(const_cast<QByteArray &>(Cmd.GetReagentGroupData()));
    QDataStream ReagentGroupDataStream(&ReagentGroupData, QIODevice::ReadWrite);
    ReagentGroupDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    CReagentGroup ReagentGroup;
    ReagentGroupDataStream >> ReagentGroup;
    bool Result = true;
    Result = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentGroupList->UpdateReagentGroup(&ReagentGroup);
    if (!Result) {
        // If error occurs , get errors and send errors to GUI
        mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel);
    }
    else {
        //BroadCast Command
        mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
        ReagentGroupData.clear();
        ReagentGroupDataStream.device()->reset();
        ReagentGroupDataStream << ReagentGroup;
        MsgClasses::CmdReagentGroupUpdate* p_Command = new MsgClasses::CmdReagentGroupUpdate(1000, ReagentGroupDataStream);
        mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_Command));
        qDebug()<<"\n\n Update Reagent Success";
    }
    static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentGroupList->Write();
}

}// End of namespace DataManager



