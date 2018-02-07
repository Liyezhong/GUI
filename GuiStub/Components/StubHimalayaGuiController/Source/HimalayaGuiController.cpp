/****************************************************************************/
/*! \file HimalayaGuiController.cpp
 *
 *  \brief Implementation file for class HimalayaGuiController.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.12.2010
 *   $Author:  $ Y.Novak
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

#include "../Include/HimalayaGuiController.h"
#include <../Include/DataLoggingSources.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>

#include <Global/Include/Commands/AckOKNOK.h>
#include <Global/Include/Commands/CmdDateAndTime.h>
#include <NetCommands/Include/CmdConfigurationFile.h>
#include <NetCommands/Include/CmdGenerateBathLayout.h>
#include <NetCommands/Include/CmdDataUpdate.h>
#include <HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentUpdate.h>
#include <HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentAdd.h>
#include <HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentRemove.h>
#include <HimalayaDataContainer/Containers/Stations/Commands/Include/CmdStationChangeReagent.h>
#include <HimalayaDataContainer/Containers/Stations/Commands/Include/CmdStationResetData.h>
#include <HimalayaDataContainer/Containers/Stations/Commands/Include/CmdStationSetAsEmpty.h>
#include <HimalayaDataContainer/Containers/Stations/Commands/Include/CmdStationSetAsFull.h>

#include <DataManager/Containers/Racks/Commands/Include/CmdRackUpdate.h>
#include <DataManager/Containers/UserSettings/Commands/Include/CmdAlarmToneTest.h>
#include <DataManager/Containers/Racks/Commands/Include/CmdRackRemove.h>
#include <DataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h>
#include <NetCommands/Include/CmdProcessState.h>
#include <DataManager/Containers/UserSettings/Commands/Include/CmdWaterStationUpdate.h>
#include <HimalayaDataContainer/Containers/Programs/Commands/Include/CmdProgramUpdate.h>
#include <HimalayaDataContainer/Containers/Programs/Commands/Include/CmdNewProgram.h>
#include <HimalayaDataContainer/Containers/Programs/Commands/Include/CmdDeleteItem.h>

#include <NetCommands/Include/CmdUpdateBlgStatus.h>

#include <NetCommands/Include/CmdEventStrings.h>
#include <NetCommands/Include/CmdExecutionStateChanged.h>
#include <NetCommands/Include/CmdLanguageFile.h>
#include <NetCommands/Include/CmdScheduledStations.h>
#include <NetCommands/Include/CmdExternalProcessState.h>
#include <HimalayaDataContainer/Containers/Programs/Commands/Include/CmdCancelProgram.h>

#include <NetCommands/Include/CmdEventReport.h>
#include <NetCommands/Include/CmdChangeAdminPassword.h>
#include <NetCommands/Include/CmdChangeAdminPasswordReply.h>
#include <NetCommands/Include/CmdChangeUserLevel.h>
#include <NetCommands/Include/CmdChangeUserLevelReply.h>
#include <NetCommands/Include/CmdStationsDefect.h>
#include <NetCommands/Include/CmdDayRunLogReply.h>
#include <NetCommands/Include/CmdDayRunLogReplyFile.h>
#include <NetCommands/Include/CmdDayRunLogRequest.h>
#include <NetCommands/Include/CmdDayRunLogRequestFile.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <NetCommands/Include/CmdGuiInit.h>
#include <NetCommands/Include/CmdRequestSepiaRack.h>
#include <NetCommands/Include/CmdRequestSepiaParameterSet.h>


namespace HimalayaGui {

/****************************************************************************/
/**
 * \brief Constructor.
 *
 * \param[in]   TheHeartBeatSource    Logging source to be used.
 */
/****************************************************************************/
HimalayaGuiController::HimalayaGuiController(Global::gSourceType TheHeartBeatSource) :
    ExternalProcessController::ExternalProcessController(GUI_PROCESS_NAME, TheHeartBeatSource),
    m_myGuiDevice(NULL)
{
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
HimalayaGuiController::~HimalayaGuiController() {
    try {
        if (m_myGuiDevice != NULL) {
            delete m_myGuiDevice;
        }
        m_myGuiDevice = NULL;
    } catch(...) {
        // to please PCLint...
    }
}

/****************************************************************************/
/**
 * \brief Create and configure needed objects.
 *
 * If something goes wrong, the already created objects must be deleted.
 */
/****************************************************************************/
void HimalayaGuiController::CreateAndInitializeObjects() {

    qDebug() << "Himalaya-GUI: initializing objects...";

    QString path = Global::SystemPaths::Instance().GetSettingsPath() + NetworkBase::CMH_PATH_TO_SETTINGS;
    m_myGuiDevice = new ExternalProcessControl::ExternalProcessDevice(NetworkBase::NSE_TYPE_NORMAL_GUI, GUI_PROCESS_NAME, path, this, this);

    // register GuiDevice with Base Class
    ExternalProcessController::RegisterExternalProcessDevice(m_myGuiDevice);

    // register acknowledge processing and timeout functors
    RegisterThreadAcksAndTimeouts();

    // register OnDataChanged processing functions
    /// \todo implement

    // continue with initialization
    ExternalProcessController::CreateAndInitializeObjects();
}

/****************************************************************************/
/**
 * \brief This function registers all command acknowledges
 *        used by this thread controller
 */
/****************************************************************************/
void HimalayaGuiController::RegisterThreadAcksAndTimeouts()
{
    // register commands for sending to external process
    RegisterCommandForProcessing<NetCommands::CmdConfigurationFile, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdConfigurationFile>, this);
    RegisterCommandForProcessing<MsgClasses::CmdRackUpdate, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdRackUpdate>, this);
    RegisterCommandForProcessing<MsgClasses::CmdRackRemoved, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdRackRemoved>, this);
    RegisterAcknowledgeForProcessing<Global::AckOKNOK, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendAckToExternalProcess<Global::AckOKNOK>, this);
    RegisterCommandForProcessing<MsgClasses::CmdStationChangeReagent, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdStationChangeReagent>, this);
    RegisterCommandForProcessing<MsgClasses::CmdStationResetData, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdStationResetData>, this);
    RegisterCommandForProcessing<MsgClasses::CmdStationSetAsEmpty, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdStationSetAsEmpty>, this);
    RegisterCommandForProcessing<MsgClasses::CmdStationSetAsFull, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdStationSetAsFull>, this);

    RegisterCommandForProcessing<NetCommands::CmdProcessState, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdProcessState>, this);
    RegisterCommandForProcessing<MsgClasses::CmdWaterStationUpdate, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdWaterStationUpdate>, this);
    RegisterCommandForProcessing<NetCommands::CmdEventStrings, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdEventStrings>, this);
    RegisterCommandForProcessing<NetCommands::CmdExecutionStateChanged, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdExecutionStateChanged>, this);
    RegisterCommandForProcessing<NetCommands::CmdLanguageFile, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdLanguageFile>, this);
    RegisterCommandForProcessing<MsgClasses::CmdChangeUserSettings, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdChangeUserSettings>, this);
    RegisterCommandForProcessing<NetCommands::CmdScheduledStations, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdScheduledStations>, this);

    // user level functions
    RegisterCommandForProcessing<NetCommands::CmdChangeUserLevelReply, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdChangeUserLevelReply>, this);
    RegisterCommandForProcessing<NetCommands::CmdChangeAdminPasswordReply, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdChangeAdminPasswordReply>, this);
        // Reagents
    RegisterCommandForProcessing<MsgClasses::CmdReagentAdd, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdReagentAdd>, this);
    RegisterCommandForProcessing<MsgClasses::CmdReagentUpdate, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdReagentUpdate>, this);
    RegisterCommandForProcessing<MsgClasses::CmdReagentRemove, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdReagentRemove>, this);

        //  Program
    RegisterCommandForProcessing<MsgClasses::CmdProgramUpdate, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdProgramUpdate>, this);
    RegisterCommandForProcessing<MsgClasses::CmdNewProgram, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdNewProgram>, this);
    RegisterCommandForProcessing<MsgClasses::CmdDeleteItem, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<MsgClasses::CmdDeleteItem>, this);

    RegisterCommandForProcessing<NetCommands::CmdDataUpdate, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdDataUpdate>, this);

    // Event view commands for Daily run log file which need to be sent to GUI
    RegisterCommandForProcessing<NetCommands::CmdDayRunLogReply, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdDayRunLogReply>, this);
    RegisterCommandForProcessing<NetCommands::CmdDayRunLogReplyFile, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdDayRunLogReplyFile>, this);
    RegisterCommandForProcessing<NetCommands::CmdGuiInit, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdGuiInit>, this);

    // Register commands for receiving from external process
    RegisterExternalMessage<Global::CmdDateAndTime, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<Global::CmdDateAndTime>, this);
    RegisterExternalMessage<MsgClasses::CmdChangeUserSettings, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdChangeUserSettings>, this);
//    RegisterExternalMessage<NetCommands::CmdAcknEventReport, HimalayaGui::HimalayaGuiController>
//            (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdAcknEventReport>, this);
        //  Reagents
    //RegisterExternalMessage<MsgClasses::CmdCancelProgram, HimalayaGui::HimalayaGuiController>
           // (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdCancelProgram>, this);

    RegisterExternalMessage<MsgClasses::CmdReagentAdd, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdReagentAdd>, this);
    RegisterExternalMessage<MsgClasses::CmdReagentRemove, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdReagentRemove>, this);
    RegisterExternalMessage<MsgClasses::CmdReagentUpdate, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdReagentUpdate>, this);
        //  Program
    RegisterExternalMessage<MsgClasses::CmdProgramUpdate, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdProgramUpdate>, this);
    RegisterExternalMessage<MsgClasses::CmdNewProgram, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdNewProgram>, this);
    RegisterExternalMessage<MsgClasses::CmdDeleteItem, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdDeleteItem>, this);
        // commands for Import and Export
    RegisterExternalMessage<MsgClasses::CmdDataExport, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdDataExport>, this);
    RegisterExternalMessage<MsgClasses::CmdDataImport, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdDataImport>, this);
    // User level functions for the different users
    RegisterExternalMessage<NetCommands::CmdChangeAdminPassword, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdChangeAdminPassword>, this);
    RegisterExternalMessage<NetCommands::CmdChangeUserLevel, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdChangeUserLevel>, this);


    // Event view commands for Daily run log file which receives from GUI
    RegisterExternalMessage<NetCommands::CmdDayRunLogRequest, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdDayRunLogRequest>, this);
    RegisterExternalMessage<NetCommands::CmdDayRunLogRequestFile, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdDayRunLogRequestFile>, this);

    // register acknowledges for receiving from external process
    RegisterExternalMessage<Global::AckOKNOK, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardAckFromExternalProcess<Global::AckOKNOK>, this);
    RegisterExternalMessage<NetCommands::CmdAcknEventReport, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardAckFromExternalProcess<NetCommands::CmdAcknEventReport>, this);
    RegisterExternalMessage<NetCommands::CmdGenerateBathLayout, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdGenerateBathLayout>, this);

    


    RegisterCommandForProcessing<NetCommands::CmdEventReport, HimalayaGui::HimalayaGuiController>
            (&HimalayaGuiController::SendCmdToExternalProcess<NetCommands::CmdEventReport>,this);



    //RMS-GUI Commands

    RegisterExternalMessage<NetCommands::CmdConfigurationFile, HimalayaGui::HimalayaGuiController>
                (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdConfigurationFile>, this);


    RegisterExternalMessage<NetCommands::CmdGuiInit, HimalayaGui::HimalayaGuiController>
                (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdGuiInit>, this);
    RegisterExternalMessage<NetCommands::CmdRequestSepiaRack, HimalayaGui::HimalayaGuiController>
                (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdRequestSepiaRack>, this);
    RegisterExternalMessage<NetCommands::CmdRequestSepiaParameterSet, HimalayaGui::HimalayaGuiController>
                (&HimalayaGuiController::ForwardCmdFromExternalProcess<NetCommands::CmdRequestSepiaParameterSet>, this);
    RegisterExternalMessage<MsgClasses::CmdAlarmToneTest, HimalayaGui::HimalayaGuiController>
                (&HimalayaGuiController::ForwardCmdFromExternalProcess<MsgClasses::CmdAlarmToneTest>, this);

}

/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Go signal.
 *
 * This means that everything is fine and normal operation started.
 * We are running in our own thread now.
 */
/****************************************************************************/
void HimalayaGuiController::OnGoReceived() {
    /// \todo implement

    /// \todo sort out the path to binaries & the rest of constants...
    qDebug() << (QString)("Himalaya-GUI: OnGo in GUI controller");
    ExternalProcessController::OnGoReceived();
}

/****************************************************************************/
/**
 * \brief This method is called when GUI is connected and ready to work.
 *
 *        This means we are in the "Working" state.
 */
/****************************************************************************/
void HimalayaGuiController::OnReadyToWork() {
    qDebug() << "HimalayaGuiController: ready to work called.";

    if (m_myGuiDevice == NULL) {
        /// \todo log error
        return;
    }

    // start sending DateAndTime (supply Zero to use the default period)
    if (!m_myGuiDevice->StartDateTimeSync(0)) {
        /// \todo: handle error.
    }
    emit SendConfigurationFiles();

    NetCommands::CmdExternalProcessState* commandPtr(new NetCommands::CmdExternalProcessState(5000, NetCommands::HIMALAYA_GUI_PROCESS, true));
    Q_ASSERT(commandPtr);

    Global::tRefType Ref = GetNewCommandRef();

    SendCommand(Ref, Global::CommandShPtr_t(commandPtr));

}

void HimalayaGuiController::OnStopWorking()
{
    NetCommands::CmdExternalProcessState* commandPtr(new NetCommands::CmdExternalProcessState(5000, NetCommands::HIMALAYA_GUI_PROCESS, false));
    Q_ASSERT(commandPtr);

    Global::tRefType Ref = GetNewCommandRef();

    SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
}


/****************************************************************************/
/**
 * \brief This method is called when the base class received the \ref Stop signal.
 *
 * This means that normal operation will stop after processing this signal.
 * We are still running in our own thread.\n
 */
/****************************************************************************/
void HimalayaGuiController::OnStopReceived() {
    /// \todo implement
    qDebug() << (QString)("Himalaya-GUI: OnStop received.");
    ExternalProcessController::OnStopReceived();

}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 *
 * Power will fail shortly.
 */
/****************************************************************************/
void HimalayaGuiController::OnPowerFail() {
    /// \todo implement
}

/****************************************************************************/
/**
 * \brief Cleanup and destroy the created objects.
 */
/****************************************************************************/
void HimalayaGuiController::CleanupAndDestroyObjects() {
    // destroy all objects
    /// \todo implement
    qDebug() << (QString)("Himalaya-GUI: CleanupAndDestroyObjects called.");
    ExternalProcessController::CleanupAndDestroyObjects();
}

/****************************************************************************/
/**
 * \brief Handler for command timeouts.
 *
 * \param[in]       Ref         The command reference.
 * \param[in]       CmdName     Name of command.
 */
/****************************************************************************/
void HimalayaGuiController::OnCmdTimeout(Global::tRefType Ref, const QString &CmdName)
{
    Q_UNUSED(Ref)
    Q_UNUSED(CmdName)
    /// \todo: implement
}

} // end namespace HimalayaGui

