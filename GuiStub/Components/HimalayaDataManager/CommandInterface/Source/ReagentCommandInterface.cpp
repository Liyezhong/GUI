/****************************************************************************/
/*! \file ReagentCommandInterface.cpp
 *
 *  \brief Reagent Command Interface  Implementation
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-10-16
 *  $Author:    $ Bhanu Prasad
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
#include "HimalayaDataManager/CommandInterface/Include/ReagentCommandInterface.h"
#include <QDebug>

namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataManager = Datamanager object
 *  \iparam p_MasterThreadController = Master thread Cntrl
 *  \iparam p_DataContainer = DataContainer pointer
 */
/****************************************************************************/
CReagentCommandInterface::CReagentCommandInterface(CDataManager *p_DataManager, Threads::MasterThreadController *p_MasterThreadController, CDataContainer *p_DataContainer):
    CCommandInterfaceBase(p_DataManager, p_MasterThreadController, p_DataContainer)
{
    try {
        CONNECTSIGNALSLOT(p_DataManager, ReagentUpdate(CReagent &), this , UpdateReagent(CReagent &));
        CONNECTSIGNALSLOT(p_DataManager, ReagentContainerUpdate(CDataReagentList &), this , UpdateReagentContainer(CDataReagentList &));
    }
    catch (...) {
        qDebug()<<"Connecting signal slot failed";
    }

    RegisterCommands();
}

/****************************************************************************/
/**
 * \brief Register Commands related to Reagent Container
 */
/****************************************************************************/
void CReagentCommandInterface::RegisterCommands() {
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdReagentUpdate, DataManager::CReagentCommandInterface>
            (&CReagentCommandInterface::UpdateReagent, this);
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdReagentAdd, DataManager::CReagentCommandInterface>
            (&CReagentCommandInterface::AddReagent, this);
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdReagentRemove, DataManager::CReagentCommandInterface>
            (&CReagentCommandInterface::DeleteReagent, this);
}

/****************************************************************************/
/**
 * \brief Function which handles CmdReagentAdd
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CReagentCommandInterface::AddReagent(Global::tRefType Ref, const MsgClasses::CmdReagentAdd &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    QByteArray ReagentData(const_cast<QByteArray &>(Cmd.GetReagentData()));
    QDataStream ReagentDataStream(&ReagentData, QIODevice::ReadWrite);
    ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    CReagent Reagent;
    ReagentDataStream >> Reagent;
    bool Result = true;
    Result = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->AddReagent(&Reagent);
    if (!Result) {
        // If error occurs , get errors and send errors to GUI
        ListOfErrors_t &ErrorList = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->GetErrorList();
        QString ErrorString;
        DataManager::Helper::ErrorIDToString(ErrorList, ErrorString);
        qDebug()<<"\n\n\nAdding Reagent Failed";
        mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel, ErrorString, Global::GUIMSGTYPE_INFO);
    }
    else {
        //BroadCast Command
        static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->GetNextFreeReagentID(true);
        mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
        ReagentData.clear();
        ReagentDataStream.device()->reset();
        ReagentDataStream << Reagent;
        MsgClasses::CmdReagentAdd* p_Command = new MsgClasses::CmdReagentAdd(1000, ReagentDataStream);
        mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_Command));
        qDebug()<<"\n\n\n Adding New Reagent Success";
    }
    static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->Write();

}

/****************************************************************************/
/**
 * \brief Function which handles CmdReagentRemove
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CReagentCommandInterface::DeleteReagent(Global::tRefType Ref, const MsgClasses::CmdReagentRemove &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    bool Result = true;
    Result = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->DeleteReagent(Cmd.GetReagentID());
    if (!Result) {
        // If error occurs , get errors and send errors to GUI
        ListOfErrors_t &ErrorList = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->GetErrorList();
        QString ErrorString;
        DataManager::Helper::ErrorIDToString(ErrorList, ErrorString);
        mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel, ErrorString, Global::GUIMSGTYPE_INFO);
             qDebug()<<"\n\n Delete Reagent Failed";
    }
    else {
         mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
     //   QByteArray BA;
     //   QDataStream ReagentDataStream(&BA, QIODevice::ReadWrite);
     //   ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
     //   ReagentDataStream << *(mp_DataContainer->ReagentList);
    //    NetCommands::CmdConfigurationFile* p_Command = new NetCommands::CmdConfigurationFile(5000, NetCommands::REAGENT, ReagentDataStream);
        MsgClasses::CmdReagentRemove* p_Command = new MsgClasses::CmdReagentRemove(5000, Cmd.GetReagentID());
        mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_Command));
        qDebug()<<"\n\n Delete Reagent Success";
    }
    static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->Write();
}

/****************************************************************************/
/**
 * \brief Function which handles CmdReagentUpdate
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CReagentCommandInterface::UpdateReagent(Global::tRefType Ref, const MsgClasses::CmdReagentUpdate &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    QByteArray ReagentData(const_cast<QByteArray &>(Cmd.GetReagentData()));
    QDataStream ReagentDataStream(&ReagentData, QIODevice::ReadWrite);
    ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    CReagent Reagent;
    ReagentDataStream >> Reagent;
    bool Result = true;
    Result = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->UpdateReagent(&Reagent);
    if (!Result) {
        // If error occurs , get errors and send errors to GUI
        ListOfErrors_t &ErrorList = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->GetErrorList();
        QString ErrorString;
        DataManager::Helper::ErrorIDToString(ErrorList, ErrorString);
        mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel, ErrorString, Global::GUIMSGTYPE_INFO);
    }
    else {
        //BroadCast Command
        mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
        ReagentData.clear();
        ReagentDataStream.device()->reset();
        ReagentDataStream << Reagent;
        MsgClasses::CmdReagentUpdate* p_Command = new MsgClasses::CmdReagentUpdate(1000, ReagentDataStream);
        mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_Command));
        qDebug()<<"\n\n Update Reagent Success";
    }
    static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->Write();
}

/****************************************************************************/
/**
 * \brief Slot called when reagent in Updated by RMS component
 *  \iparam Reagent = Updated reagent
 */
/****************************************************************************/
void CReagentCommandInterface::UpdateReagent(CReagent &Reagent)
{
    bool Result = true;
    Result = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList->UpdateReagent(&Reagent);
    if (!Result) {
        qDebug()<<" \n\n Update Reaget Failed";
        //! Todo Inform Event Handler
    }
    else {
        //Inform components who use Reagent container
        QByteArray BA;
        QDataStream ReagentDataStream(&BA, QIODevice::ReadWrite);
        ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        try {
            ReagentDataStream << Reagent;
            MsgClasses::CmdReagentUpdate* p_Command = new MsgClasses::CmdReagentUpdate(1000, ReagentDataStream);
            mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_Command));
        }
        catch (...) {
            qDebug()<<" \n\n Update Reaget Failed";
            //! Todo Inform Event Handler
        }

    }
}

/****************************************************************************/
/**
 * \brief Slot called when reagent list in Updated by RMS component
 *  \iparam Reagent = Updated reagent list
 */
/****************************************************************************/
void CReagentCommandInterface::UpdateReagentContainer(CDataReagentList &ReagentList)
{
    try {
        *(static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList) = ReagentList;
        QByteArray BA;
        QDataStream ReagentDataStream(&BA, QIODevice::ReadWrite);
        ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        ReagentDataStream << *(static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ReagentList);
        NetCommands::CmdConfigurationFile *p_Command = new NetCommands::CmdConfigurationFile(5000, NetCommands::REAGENT, ReagentDataStream);
        mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_Command));
    }
    catch (...) {
        qDebug()<<"\n\n\n Reagent List Container Update Failed";
        //! Todo Inform Event Handler
    }
}

}// End of namespace DataManager







