/****************************************************************************/
/*! \file ProgramCommandInterface.cpp
 *
 *  \brief Program Command Interface  Implementation
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
#include "../Include/ProgramCommandInterface.h"


namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataManager = Datamanager object
 *  \iparam p_MasterThreadController = Master thread Cntrl
 *  \iparam p_DataContainer = DataContainer pointer
 */
/****************************************************************************/
CProgramCommandInterface::CProgramCommandInterface(CDataManager *p_DataManager,
                                                   Threads::MasterThreadController *p_MasterThreadController,
                                                   CDataContainer *p_DataContainer):
    CCommandInterfaceBase(p_DataManager, p_MasterThreadController, p_DataContainer)
{
    RegisterCommands();
}

/****************************************************************************/
/**
 * \brief Register Commands related to Program Container
 */
/****************************************************************************/
void CProgramCommandInterface::RegisterCommands() {
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdProgramUpdate, DataManager::CProgramCommandInterface>
            (&CProgramCommandInterface::UpdateProgram, this);
    mp_MasterThreadController->RegisterCommandForProcessing<MsgClasses::CmdNewProgram, DataManager::CProgramCommandInterface>
            (&CProgramCommandInterface::AddProgram, this);
}

/****************************************************************************/
/**
 * \brief Function which handles CmdNewProgram
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CProgramCommandInterface::AddProgram(Global::tRefType Ref, const MsgClasses::CmdNewProgram &Cmd, Threads::CommandChannel &AckCommandChannel)
{


}

/****************************************************************************/
/**
 * \brief Function which handles CmdProgramUpdate
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void CProgramCommandInterface::UpdateProgram(Global::tRefType Ref, const MsgClasses::CmdProgramUpdate &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    //Program for which white color is assigned
    CProgram Program;
    //Program with modified color.This program uses the color previously assigned to above program
    CProgram ProgramWithNewColor;

    // we work on Temporary copy of program list , when Color was modified.
    DataManager::CDataProgramList TempProgramList;

    QByteArray ProgramData(const_cast<QByteArray &>(Cmd.GetProgramData()));
    QDataStream ProgramDataStream(&ProgramData, QIODevice::ReadWrite);
    ProgramDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    ProgramDataStream.device()->reset();
    ProgramDataStream >> Program;


    bool Result = true;
    bool ColorReplacedFlag = false;
    ColorReplacedFlag = (Cmd.GetProgramColorReplaced());
    if (ColorReplacedFlag == true) {

        TempProgramList = *(static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ProgramList);

        QByteArray ColorAssignedProgramData(const_cast<QByteArray &>(Cmd.GetColorAssignedProgramData()));
        QDataStream ColorAssignedProgramDataStream(&ColorAssignedProgramData,QIODevice::ReadWrite);
        ColorAssignedProgramDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        ColorAssignedProgramDataStream >> ProgramWithNewColor;

        // Update program
        Result = TempProgramList.UpdateProgram(&Program);
        if (!Result) {
            // If error occurs , get errors and send errors to GUI
            mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel);
            return;
        }

        Result = TempProgramList.UpdateProgram(&ProgramWithNewColor);

        if (Result){
            *(static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ProgramList) = TempProgramList;
            //Send Ack
            mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
            ProgramDataStream.device()->reset();
            ProgramDataStream << Program ;
            ColorAssignedProgramDataStream.device()->reset();
            ColorAssignedProgramDataStream << ProgramWithNewColor ;
            MsgClasses::CmdProgramUpdate* p_ProgramCommand = new MsgClasses::CmdProgramUpdate(1000, ProgramDataStream,
                                                                                              ColorAssignedProgramDataStream);
            mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_ProgramCommand));
            qDebug()<<"\n\n Update Program Success";
            emit StartableProgramEdited(Program.GetID());
        }
        else {
            // If error occurs , get errors and send errors to GUI
            mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel);
            qDebug()<<"\n\n Update Program Failed";
        }
    }
    else {
        Result = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ProgramList->UpdateProgram(&Program);
        if (!Result) {
            ListOfErrors_t &ErrorList = static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ProgramList->GetErrorList();
            QString ErrorString;
            DataManager::Helper::ErrorIDToString(ErrorList, ErrorString);
            // If error occurs , get errors and send errors to GUI
            mp_MasterThreadController->SendAcknowledgeNOK(Ref, AckCommandChannel, ErrorString, Global::GUIMSGTYPE_INFO);
            qDebug()<<"\n\n Update Program Failed";
        }
        else {
            //BroadCast Command
            mp_MasterThreadController->SendAcknowledgeOK(Ref, AckCommandChannel);
            ProgramData.clear();
            ProgramDataStream.device()->reset();
            ProgramDataStream << Program;

            MsgClasses::CmdProgramUpdate* p_Command = new MsgClasses::CmdProgramUpdate(1000, ProgramDataStream);
            mp_MasterThreadController->BroadcastCommand(Global::CommandShPtr_t(p_Command));
            emit StartableProgramEdited(Program.GetID());
            qDebug()<<"\n\n Update Program Success";
        }
    }
    static_cast<DataManager::CDataContainer*>(mp_DataContainer)->ProgramList->Write();
}

}// End of namespace DataManager




