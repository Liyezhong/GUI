/****************************************************************************/
/*! \file DataConnector.cpp
 *
 *  \brief DataConnector implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-02-24, 29-Nov-2012
 *   $Author:  $ M.Scherer, Ningu
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
#include "Core/Include/DataContainer.h"
#include "Core/Include/DataConnector.h"
#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataExport.h"
#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataImport.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdAlarmToneTest.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAcknowledge.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdEnterCleaningProgram.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAborted.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramSelected.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdKeepCassetteCount.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdTakeOutSpecimenFinished.h"
#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdQuitAppShutdown.h"
#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdResetOperationHours.h"
#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdParaffinMeltPointChanged.h"
#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdBottleCheck.h"
#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdBottleCheckReply.h"

#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "Global/Include/Commands/AckOKNOK.h"
#include "Global/Include/Commands/CmdDateAndTime.h"
#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include <QApplication>
#include "Global/Include/UITranslator.h"
#include "MainMenu//Include/MsgBoxManager.h"
#include <Global/Include/SystemPaths.h>
#include <QProcess>
#include <QDesktopWidget>
#include <Core/Include/ReagentStatusModel.h>
#include "Core/Include/GlobalHelper.h"
#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"

namespace Core {
const int COMMAND_TIME_OUT = 30000;       ///<  Definition/Declaration of variable COMMAND_TIME_OUT
const int RC_REQUEST_COMMAND_TIME_OUT = 30000;        //!< Remote Care Command Timeout

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CDataConnector::CDataConnector(MainMenu::CMainWindow *p_Parent) : DataManager::CDataContainer(),
    mp_MainWindow(p_Parent), mp_WaitDialog(NULL),mp_UserSettingWaitDialog(NULL),
    mp_LanguageFile(NULL), mp_OldFile(NULL),
    m_LanguageChangeCount(0), m_ConsumableType(KIT), m_BottleCount(0), m_GuiInit(true),
    m_strCommunicationErrorTitle(tr("Communication Error")),
    m_strChangeNotSave(tr("The changes can not be saved.")),
    m_strStartup(tr("Startup")),
    m_strInitDevCom(tr("Initializing device communication ...")),
    m_strDisconnect(tr("Disconnect")),
    m_strReinitDeviceCommunication(tr("Reinitializing device communication ...")),
    m_strError(tr("Error")),
    m_strInformation(tr("Information")),
    m_strWarning(tr("Warning")),
    m_strOK(tr("OK")),
    m_strDeviceCommunication(tr("Operation ongoing")),
    m_strSavingSettings(tr("Saving Settings ...")),
    m_strUserExport(tr("User Export")),
    m_strExportUserData(tr("Exporting user data ...")),
    m_strServiceExport(tr("Service Export")),
    m_strExportServiceData(tr("Exporting service data ...")),
    m_strImport("Import"),
    m_strImportData("Importing data ..."),
    m_strLogFile(tr("Log Files")),
    m_strGettingDailyLog(""),
    mp_WaitDlgExecChanged(NULL)

{
    if (m_NetworkObject.NetworkInit()) {
        THROW(Global::EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION);
    }

    m_NetworkObject.RegisterNetMessage<NetCommands::CmdConfigurationFile>(&CDataConnector::ConfFileHandler, this);
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdProcessState>(&CDataConnector::ProcessStateHandler, this);

    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdNewProgram>(&CDataConnector::AddNewProgramHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdProgramUpdate>(&CDataConnector::UpdateProgramHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdProgramDeleteItem>(&CDataConnector::ProgramRemoveHandler, this);

    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdReagentAdd>(&CDataConnector::AddNewReagentHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdReagentUpdate>(&CDataConnector::UpdateReagentHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdReagentRemove>(&CDataConnector::ReagentRemoveHandler, this);

    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdReagentGroupUpdate>(&CDataConnector::UpdateReagentGroupHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdUpdateProgramEndTime>(&CDataConnector::UpdateEndTimeHandler, this);

    //Reagent Station Commands
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdStationChangeReagent>(&CDataConnector::UpdateStationChangeReagentHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdStationResetData>(&CDataConnector::UpdateStationResetDataHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdStationSetAsEmpty>(&CDataConnector::UpdateStationSetAsEmptyHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdStationSetAsFull>(&CDataConnector::UpdateStationSetAsFullHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdUpdateStationReagentStatus>(&CDataConnector::UpdateStationReagentStatus, this);

    // Dashboard Command Handlers
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdCurrentProgramStepInfor>(&CDataConnector::CurrentProgramStepInfoHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdProgramAcknowledge>(&CDataConnector::ProgramAcknowledgeHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdEnterCleaningProgram>(&CDataConnector::EnterCleaningProgramHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdProgramAborted>(&CDataConnector::ProgramAbortedHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdStationSuckDrain>(&CDataConnector::StationParaffinBathStatusHandler, this);

    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdProgramSelectedReply>(&CDataConnector::ProgramSelectedReplyHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdLockStatus>(&CDataConnector::RetortLockStatusHandler, this);

    m_NetworkObject.RegisterNetMessage<NetCommands::CmdEventStrings>(&CDataConnector::EventStringHandler, this);
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdExecutionStateChanged>(&CDataConnector::ExecutionStateHandler, this);
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdLanguageFile>(&CDataConnector::LanguageFileHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdChangeUserSettings>(&CDataConnector::SettingsUpdateHandler, this);
    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdRecoveryFromPowerFailure>(&CDataConnector::RecoveryFromPowerFailureHandler, this);

    // RemoteCare commands
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdRemoteCareState>(&CDataConnector::RemoteCareStateHandler, this);
    m_NetworkObject.RegisterNetMessage<RemoteCare::CmdRCSoftwareUpdate>(&CDataConnector::OnRCSoftwareUpdateHandler, this);
    m_NetworkObject.RegisterNetMessage<RemoteCare::CmdRCRequestRemoteSession>(&CDataConnector::OnRCRequestRemoteSessionHandler, this);

    //EventHandlercomamnds
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdEventReport>(&CDataConnector::EventReportHandler, this);

    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdDataImportFiles>(&CDataConnector::OnImportFileSelection, this);
    //user menu functions
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdChangeUserLevelReply>(&CDataConnector::UserAuthenticationHandler, this);
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdChangeAdminPasswordReply>(&CDataConnector::AdminPasswordChangeHandler, this);
    //Event view commands
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdDayRunLogReplyFile>(&CDataConnector::DayRunLogReplyFileHandler, this);
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdDayRunLogReply>(&CDataConnector::DayRunLogReplyHandler, this);
    m_NetworkObject.RegisterNetMessage<NetCommands::CmdGuiInit>(&CDataConnector::OnCmdGuiInit, this);

    m_NetworkObject.RegisterNetMessage<MsgClasses::CmdBottleCheckReply>(&CDataConnector::BottleCheckReplyHandler, this);
    // Signal/slot for DateAndTime reporting
    CONNECTSIGNALSLOT(&m_NetworkObject, SigDateAndTime(QDateTime), this, SetDateTime(QDateTime));
    // Signal/slot for timeout reporting
    CONNECTSIGNALSLOT(&m_NetworkObject, SigCmdTimeout(Global::tRefType, const QString &),
                      this, OnCmdTimeout(Global::tRefType, const QString &));
    // Signal indicating a valid connection to Master
    CONNECTSIGNALSLOT(&m_NetworkObject, SigMasterConnected(const QString &), this, OnMasterConnected(const QString &));
    // Signal indicating a disconnection from Master
    CONNECTSIGNALSLOT(&m_NetworkObject, SigMasterDisconnected(const QString &), this, OnMasterDisconnected(const QString &));

    CONNECTSIGNALSLOT(mp_MainWindow, onChangeEvent(), this, RetranslateUI());

    CONNECTSIGNALSLOT(mp_MainWindow, CurrentTabChanged(int), this, OnCurrentTabChanged(int));

    mp_MessageDlg = new MainMenu::CMessageDlg(mp_MainWindow);
    mp_MessageDlg->SetTitle(m_strCommunicationErrorTitle);
    mp_MessageDlg->SetText(m_strChangeNotSave);
    mp_MessageDlg->SetIcon(QMessageBox::Critical);
    mp_MessageDlg->SetButtonText(1, m_strOK);
    mp_MessageDlg->setModal(true);
    mp_MessageDlg->HideButtons();

    mp_WaitDialog = new MainMenu::CWaitDialog(mp_MainWindow);
    mp_WaitDialog->HideAbort();
    CONNECTSIGNALSLOT(mp_WaitDialog, Timeout(), mp_MessageDlg, Show());

    // MsgBox Manager
    mp_MesgBoxManager  = new MainMenu::CMsgBoxManager(mp_MainWindow,SettingsInterface);
    (void)connect(mp_MesgBoxManager, SIGNAL(EventReportAck(NetCommands::ClickedButton_t,Global::tRefType, quint64)),
                      this, SLOT(OnEventReportAck(NetCommands::ClickedButton_t,Global::tRefType, quint64)));

    m_pServiceProcess = new QProcess();

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDataConnector::~CDataConnector()
{
    try {
        delete mp_MesgBoxManager;

        delete mp_WaitDialog;
        delete mp_MessageDlg;
        delete mp_LanguageFile;
        delete m_pServiceProcess;
        delete mp_WaitDlgExecChanged;
    }
    catch (...) {
        //To please lint warnings
    }
}

/****************************************************************************/
/*!
 *  \brief Sets time and and date of the program
 *
 *  \iparam DateTime = New date and time
 */
/****************************************************************************/
void CDataConnector::SetDateTime(QDateTime DateTime)
{
    DEBUGWHEREAMI;
    // set new time
    Global::AdjustedTime::Instance().AdjustToDateTime(DateTime);
}

/****************************************************************************/
/*!
 *  \brief Called if a command sent to master times out.
 *
 * Called if a command sent to master times out. \todo check if it can be removed
 *
 * \param[in]   Ref             Reference of command which timed out.
 * \param[in]   CommandName     Name of command which timed out.
 */
/****************************************************************************/
void CDataConnector::OnCmdTimeout(Global::tRefType Ref, const QString &CommandName)
{
    DEBUGWHEREAMI;
    qDebug() << "Ref = " << Ref << " Name = " << CommandName;
}

/****************************************************************************/
/*!
 *  \brief  This slot is called as soon as connection with Master is established
 *
 *  \param[in] name = client name
 */
/****************************************************************************/
void CDataConnector::OnMasterConnected(const QString &name)
{
    Q_UNUSED(name)
    // hide the "waiting for connection" dialog:
    mp_WaitDialog->accept();
    /// \todo: do whatever else needs to be done on this event
}

/****************************************************************************/
/*!
 *  \brief  This slot is called as soon as connection to Master is lost
 *
 *  \param[in] name = client name
 */
/****************************************************************************/
void CDataConnector::OnMasterDisconnected(const QString &name)
{
    Q_UNUSED(name)
    // show the "waiting for reconnect" dialog:
    mp_WaitDialog->SetDialogTitle(m_strDisconnect);
    mp_WaitDialog->SetText(m_strReinitDeviceCommunication);
    mp_WaitDialog->show();
    /// \todo: do whatever needs to be done on this event
}

/****************************************************************************/
/**
 * \brief Acknowledge for a date and time transaction received.
 *
 * Acknowledge received.
 *
 * \param[in]   Ref     Acknowledge reference.
 * \param[in]   Ack     The acknowledge.
 */
/****************************************************************************/
void CDataConnector::OnAckDateAndTime(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref);
    mp_WaitDialog->accept();
    if (Ack.GetStatus() == true) {
        emit DateTimeAcked();
        emit ReagentUpdated();//update for reagent expired
    }
    else {
        if (Ack.GetText().length() > 0) {
            switch(Ack.GetType()) {
                case Global::GUIMSGTYPE_ERROR:
                    mp_MessageDlg->SetIcon(QMessageBox::Critical);
                    mp_MessageDlg->SetTitle(m_strError);
                    break;
                case Global::GUIMSGTYPE_INFO:
                    mp_MessageDlg->SetIcon(QMessageBox::Information);
                    mp_MessageDlg->SetTitle(m_strInformation);
                    break;
                case Global::GUIMSGTYPE_WARNING:
                    mp_MessageDlg->SetIcon(QMessageBox::Warning);
                    mp_MessageDlg->SetTitle(m_strWarning);
                    break;
                default:
                    break;
            }
            mp_MessageDlg->SetText(Ack.GetText());
        }
        else {
            mp_MessageDlg->SetTitle(m_strCommunicationErrorTitle);
            mp_MessageDlg->SetText(m_strCommunicationError);
            mp_MessageDlg->SetIcon(QMessageBox::Critical);
        }
        mp_MessageDlg->SetButtonText(1, m_strOK);
        mp_MessageDlg->show();
    }
}

/****************************************************************************/
/**
 * \brief Acknowledge received for transmissions consisting of two commands
 *
 * Acknowledge received.
 *
 * \param[in]   Ref     Acknowledge reference.
 * \param[in]   Ack     The acknowledge.
 */
/****************************************************************************/
void CDataConnector::OnAckTwoPhase(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref);
    qDebug()<<"\n\n Ack.GetStatus() = "<< Ack.GetStatus();
    // just accept the wait dialog
    mp_WaitDialog->accept();
    if (Ack.GetStatus() == false && Ack.GetText().length() > 0) {
        ShowMessageDialog(Ack.GetType(), Ack.GetText());
    }    
}

/****************************************************************************/
/**
 * \brief Acknowledge received for transmissions consisting of two commands
 *
 * Acknowledge received.
 *
 * \param[in]   Ref     Acknowledge reference.
 * \param[in]   Ack     The acknowledge.
 */
/****************************************************************************/
void CDataConnector::OnAck(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref);
    if (Ack.GetStatus() == false) {
        mp_WaitDialog->accept();
    }   
}
/****************************************************************************/
/*!
 *  \brief Sends data and time to the master
 *
 *  \iparam DateTime = Date and time to be sent
 */
/****************************************************************************/
void CDataConnector::SendDateTime(QDateTime DateTime)
{
    Global::CmdDateAndTime Command(COMMAND_TIME_OUT, DateTime);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckDateAndTime, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->show();
}

bool CDataConnector::IsOvenDoorLocked()
{
    if(mp_MesgBoxManager)
    {
        return mp_MesgBoxManager->IsOvenDoorLocked();
    }
}

/****************************************************************************/
/*!
 *  \brief Sends updated Dashboard Station Change Reagent data to the master
 *
 *  \param StationId
 *  \param ReagentId
 */
/****************************************************************************/
void CDataConnector::SendStationChangeReagent(const QString& StationId, const QString& ReagentId)
{
    MsgClasses::CmdStationChangeReagent Command(COMMAND_TIME_OUT, StationId, ReagentId);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

void CDataConnector::SendStationResetData(const QString& StationId)
{
    MsgClasses::CmdStationResetData Command(COMMAND_TIME_OUT, StationId);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

void CDataConnector::SendStationSetAsEmpty(const QString StationId)
{
    MsgClasses::CmdStationSetAsEmpty Command(COMMAND_TIME_OUT, StationId);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

void CDataConnector::SendStationSetAsFull(const QString& StationId)
{
    MsgClasses::CmdStationSetAsFull Command(COMMAND_TIME_OUT, StationId);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

void CDataConnector::SendRMSChanged(Global::RMSOptions_t rmsOption)
{
    Q_UNUSED(rmsOption)
}

/****************************************************************************/
/*!
 *  \brief Sends updated Reagent data to the master
 *
 *  \param ReagentGroup
 */
/****************************************************************************/
void CDataConnector::SendReagentGroupUpdate(DataManager::CReagentGroup &ReagentGroup)
{
    MsgClasses::CmdReagentGroupUpdate Command(COMMAND_TIME_OUT, ReagentGroup.GetGroupID(), ReagentGroup.GetGroupColor());
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

/****************************************************************************/
/*!
 *  \brief Sends updated Reagent data to the master
 *
 *  \iparam Reagent = Reagent data object
 */
/****************************************************************************/
void CDataConnector::SendReagentUpdate(DataManager::CReagent &Reagent)
{
    QByteArray ByteArray;
    QDataStream ReagentDataStream(&ByteArray,QIODevice::ReadWrite);
    ReagentDataStream << Reagent;
    MsgClasses::CmdReagentUpdate Command(COMMAND_TIME_OUT, ReagentDataStream);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

/****************************************************************************/
/*!
 *  \brief Sends new Reagent data to the master
 *
 *  \iparam Reagent = Reagent data object
 */
/****************************************************************************/
void CDataConnector::SendReagentAdd(DataManager::CReagent &Reagent)
{
    QByteArray ByteArray;
    QDataStream ReagentDataStream(&ByteArray,QIODevice::ReadWrite);
    ReagentDataStream << Reagent;
    MsgClasses::CmdReagentAdd Command(COMMAND_TIME_OUT, ReagentDataStream);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnReagentAck, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
 }

/****************************************************************************/
/*!
 *  \brief Sends new Reagent data to the master
 *
 *  \iparam ReagentID = Reagent data object
 */
/****************************************************************************/
void CDataConnector::SendReagentRemove(QString ReagentID)
{
    MsgClasses::CmdReagentRemove Command(COMMAND_TIME_OUT, ReagentID);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

/****************************************************************************/
/*!
 *  \brief Sends updated Program data to the master
 *
 *  \iparam Program = Program data object
 */
/****************************************************************************/
void CDataConnector::SendProgramUpdate(DataManager::CProgram &Program)
{
      QByteArray ByteArray;
      QDataStream ProgramDataStream(&ByteArray,QIODevice::ReadWrite);
      ProgramDataStream << Program;
      MsgClasses::CmdProgramUpdate Command(COMMAND_TIME_OUT, ProgramDataStream, ProgramDataStream);
      Command.SetProgramColorReplaced(false);
      (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnProgramAck, this);
      mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
      mp_WaitDialog->SetText(m_strSavingSettings);
      mp_WaitDialog->SetTimeout(10000);
      mp_WaitDialog->show();
}

/****************************************************************************/
/*!
 *  \brief Sends new program data to the master
 *
 *  \iparam Program = Program data object
 */
/****************************************************************************/
void CDataConnector::SendProgramAdd(DataManager::CProgram &Program)
{
    QByteArray ByteArray;
    QDataStream ProgramDataStream(&ByteArray,QIODevice::ReadWrite);
    ProgramDataStream << Program;
    (void)ProgramDataStream.device()->reset();
    MsgClasses::CmdNewProgram Command(COMMAND_TIME_OUT, ProgramDataStream);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnProgramAck, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

/****************************************************************************/
/*!
 *  \brief Sends new program data to the master
 *
 *  \iparam ProgramID = Program data object
 */
/****************************************************************************/
void CDataConnector::SendProgramRemove(QString &ProgramID)
{
    MsgClasses::CmdProgramDeleteItem Command(COMMAND_TIME_OUT, ProgramID);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnProgramAck, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

void CDataConnector::SendAppQuitSystemPrepareShutdown(DataManager::QuitAppShutdownActionType_t quitAppShutdownActionType)
{
    MsgClasses::CmdQuitAppShutdown Command(COMMAND_TIME_OUT, quitAppShutdownActionType);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

void CDataConnector::SendResetOperationDays(DataManager::ResetOperationHoursType_t resetOperationHoursType)
{
    MsgClasses::CmdResetOperationHours Command(COMMAND_TIME_OUT, resetOperationHoursType);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(10000);
    mp_WaitDialog->show();
}

/****************************************************************************/
/*!
 *  \brief Handles incoming configuration Program commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::AddNewProgramHandler(Global::tRefType Ref, const MsgClasses::CmdNewProgram &Command)
{
    bool Result = false;
    DataManager::CProgram Program;
    QDataStream ProgramDataStream(&const_cast<QByteArray &>(Command.GetProgramData()), QIODevice::ReadWrite);
    ProgramDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    ProgramDataStream >> Program;

    Result = ProgramList->AddProgram(&Program);
    if (Result) {
        emit ProgramAdded();
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
}

/****************************************************************************/
/*!
 *  \brief Handles incoming configuration Program commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::UpdateProgramHandler(Global::tRefType Ref, const MsgClasses::CmdProgramUpdate &Command)
{
    bool Result = false;
    DataManager::CProgram Program;
    QDataStream ProgramDataStream(&const_cast<QByteArray &>(Command.GetProgramData()), QIODevice::ReadWrite);
    ProgramDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    ProgramDataStream >> Program;
    Result = ProgramList->UpdateProgram(&Program);
    if (Result) {
        emit ProgramUpdated();
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
    return;
}

/****************************************************************************/
/*!
 *  \brief Handles incomming ProgramRemove cmd
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::ProgramRemoveHandler(Global::tRefType Ref,
                                       const MsgClasses::CmdProgramDeleteItem &Command)
{
    bool Result = true;
    QString ProgramId = Command.GetItemId();
    Result = ProgramList-> DeleteProgram(ProgramId);
    if (Result) {
        emit ProgramDeleted();
    }
    else {
        Result = false;
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
    return;
}

/****************************************************************************/
/*!
 *  \brief Handles incoming configuration Reagents commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::AddNewReagentHandler(Global::tRefType Ref, const MsgClasses::CmdReagentAdd &Command)
{
    bool Result = true;
    DataManager::CReagent Reagent;
    QDataStream ReagentDataStream(&const_cast<QByteArray &>(Command.GetReagentData()), QIODevice::ReadWrite);
    ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    ReagentDataStream >> Reagent;
    Result = ReagentList->AddReagent(&Reagent);
    if (Result) {
        emit ReagentAdded();
        (void)ReagentList->GetNextFreeReagentID(true);
        mp_WaitDialog->accept();
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
    return;
}
/****************************************************************************/
/*!
 *  \brief Handles incoming configuration Reagent commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::UpdateReagentHandler(Global::tRefType Ref, const MsgClasses::CmdReagentUpdate &Command)
{
    bool Result = true;
    DataManager::CReagent Reagent;
    QDataStream ReagentDataStream(&const_cast<QByteArray &>(Command.GetReagentData()), QIODevice::ReadWrite);
    ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    ReagentDataStream >> Reagent;
    Result = ReagentList->UpdateReagent(&Reagent);
    if (Result) {
        mp_WaitDialog->accept();
        emit ReagentUpdated();
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
    return;
}

/****************************************************************************/
/*!
 *  \brief Handles incoming configuration Reagent commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::UpdateReagentGroupHandler(Global::tRefType Ref, const MsgClasses::CmdReagentGroupUpdate &Command)
{
    bool Result = true;
    DataManager::CReagentGroup* pReagentGroup = ReagentGroupList->GetReagentGroup(Command.GroupId());
    if (pReagentGroup)
        pReagentGroup->SetGroupColor(Command.GroupColor());
    else
        Result = false;
    if(Result){
        mp_WaitDialog->accept();
        emit ReagentGroupUpdated();
        emit ReagentUpdated();
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
    return;
}

/****************************************************************************/
/*!
 *  \brief Handles incoming update end time commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::UpdateEndTimeHandler(Global::tRefType Ref, const MsgClasses::CmdUpdateProgramEndTime &Command)
{
    emit UpdateProgramEndTime(Command.GetEndTimeDiff());
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
}

/****************************************************************************/
/*!
 *  \brief Handles incoming configuration Dashboard Station commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::UpdateStationChangeReagentHandler(Global::tRefType Ref,
                                                       const MsgClasses::CmdStationChangeReagent &Command)
{
    bool Result = true;
    DataManager::CDashboardStation* pDashboardStation = DashboardStationList->GetDashboardStation(Command.StationID());
    if (pDashboardStation)
    {
        pDashboardStation->SetDashboardReagentID(Command.ReagentID());
        pDashboardStation->SetDashboardReagentActualCassettes(0);
        pDashboardStation->SetDashboardReagentActualCycles(0);
        pDashboardStation->SetDashboardReagentStatus("Empty");
    }
    else
        Result = false;
    if(Result){
        mp_WaitDialog->accept();
        emit DashboardStationChangeReagent(Command.StationID());
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
}


void CDataConnector::UpdateStationResetDataHandler(Global::tRefType Ref, const MsgClasses::CmdStationResetData &Command)
{
    bool Result = true;
    DataManager::CDashboardStation* pDashboardStation = DashboardStationList->GetDashboardStation(Command.StationID());
     if (pDashboardStation) {
        pDashboardStation->SetDashboardReagentExchangeDate(QDateTime::currentDateTime());

        DataManager::CReagent* pReagent = ReagentList->GetReagent(pDashboardStation->GetDashboardReagentID());
        DataManager::CReagentGroup* pReagentGroup = ReagentGroupList->GetReagentGroup(pReagent->GetGroupID());
        Global::RMSOptions_t rmsOption;
        if (pReagentGroup->IsCleaningReagentGroup())
        {
            rmsOption = CReagentStatusModel::RMSCLEANINGOPTIONS;
        }
        else
        {
            rmsOption = CReagentStatusModel::RMSPROCESSINGOPTION;
        }
        switch (rmsOption) {
            default:
                 QString("");
                break;
            case Global::RMS_CASSETTES:
                pDashboardStation->SetDashboardReagentActualCassettes(0);
                break;
            case Global::RMS_CYCLES:
                pDashboardStation->SetDashboardReagentActualCycles(0);
                break;
         }
     }
     else
        Result = false;
     if(Result){
        mp_WaitDialog->accept();
        emit DashboardStationChangeReagent(Command.StationID());
     }
     m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
}

void CDataConnector::UpdateStationSetAsEmptyHandler(Global::tRefType Ref, const MsgClasses::CmdStationSetAsEmpty &Command)
{
    bool Result = true;
    DataManager::CDashboardStation* pDashboardStation = DashboardStationList->GetDashboardStation(Command.StationID());
    if (pDashboardStation)
    {
        pDashboardStation->SetDashboardReagentStatus("Empty");
    }
    else
        Result = false;
    if(Result){
        mp_WaitDialog->accept();
        emit DashboardStationChangeReagent(Command.StationID());
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
}

void CDataConnector::UpdateStationSetAsFullHandler(Global::tRefType Ref, const MsgClasses::CmdStationSetAsFull &Command)
{
    bool Result = true;
    DataManager::CDashboardStation* pDashboardStation = DashboardStationList->GetDashboardStation(Command.StationID());
    if (pDashboardStation){
        pDashboardStation->SetDashboardReagentStatus("Full");
        pDashboardStation->SetDashboardReagentExchangeDate(QDateTime::currentDateTime());

        DataManager::CReagent* pReagent = ReagentList->GetReagent(pDashboardStation->GetDashboardReagentID());
        DataManager::CReagentGroup* pReagentGroup = ReagentGroupList->GetReagentGroup(pReagent->GetGroupID());
        Global::RMSOptions_t rmsOption;
        if (pReagentGroup->IsCleaningReagentGroup())
        {
            rmsOption = CReagentStatusModel::RMSCLEANINGOPTIONS;
        }
        else
        {
            rmsOption = CReagentStatusModel::RMSPROCESSINGOPTION;
        }

        switch (rmsOption) {
            default:
                 QString("");
                break;
            case Global::RMS_CASSETTES:
                pDashboardStation->SetDashboardReagentActualCassettes(0);
                break;
            case Global::RMS_CYCLES:
                 pDashboardStation->SetDashboardReagentActualCycles(0);
                break;
         }
    }
    else
        Result = false;
    if(Result){
        mp_WaitDialog->accept();
        emit DashboardStationChangeReagent(Command.StationID());
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
}

void CDataConnector::UpdateStationReagentStatus(Global::tRefType Ref, const MsgClasses::CmdUpdateStationReagentStatus &Command)
{
    (void)m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    const QStringList& Ids = Command.StationIDs();
    for (int i = 0; i < Ids.count(); i++)
    {
        DataManager::CDashboardStation* pDashboardStation = DashboardStationList->GetDashboardStation(Ids[i]);
        if (pDashboardStation)
        {
            if(Command.CassetteCount() > 0)
            {

                pDashboardStation->SetDashboardReagentActualCassettes(
                            pDashboardStation->GetDashboardReagentActualCassettes() + Command.CassetteCount());
                emit DashboardStationChangeReagent(Ids[i]);

            }
            else
            {
                pDashboardStation->SetDashboardReagentActualCycles(
                            pDashboardStation->GetDashboardReagentActualCycles() + 1);
                emit DashboardStationChangeReagent(Ids[i]);
            }
        }
        else
        {
            qDebug() << "UpdateStationReagentStatus Failed.";
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Handles incoming ReagentRemove cmd
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::ReagentRemoveHandler(Global::tRefType Ref,
                                       const MsgClasses::CmdReagentRemove &Command)
{
    bool Result = false;
    bool ResultUpdateStation = false;
    ResultUpdateStation = DashboardStationList->UpdateStationsByReagentDelete(Command.GetReagentID());
    Result = ReagentList->DeleteReagent(Command.GetReagentID());
    if (Result && ResultUpdateStation) {
        mp_WaitDialog->accept();
        emit ReagentUpdated();
    }
    else {
        Result = false;
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
    return;
}

void CDataConnector::ServiceStart()
{
    bool IsNeedUpdate = false;
    QString strDate = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
    DataManager::CHimalayaUserSettings* HimalayaUserSettings = SettingsInterface->GetUserSettings();
    if ("" ==  HimalayaUserSettings->GetOperationLastResetDate())
    {
        IsNeedUpdate = true;
        HimalayaUserSettings->SetOperationLastResetDate(strDate);
    }

    if ("" == HimalayaUserSettings->GetActiveCarbonLastResetDate())
    {
        IsNeedUpdate = true;
        HimalayaUserSettings->SetActiveCarbonLastResetDate(strDate);
    }

    if (IsNeedUpdate)
    {
        QByteArray ByteArray;
        QDataStream SettingsDataStream(&ByteArray, QIODevice::ReadWrite);
        SettingsDataStream << *HimalayaUserSettings;
        (void)SettingsDataStream.device()->reset();
        MsgClasses::CmdChangeUserSettings Command(COMMAND_TIME_OUT, SettingsDataStream);
        (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnUserSettingsAck, this);
    }
}

/****************************************************************************/
/*!
 *  \brief Handles incoming configuration file commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = The command
 */
/****************************************************************************/
void CDataConnector::ConfFileHandler(Global::tRefType Ref, const NetCommands::CmdConfigurationFile &Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));

    QDataStream DataStream(const_cast<QByteArray *>(&Command.GetFileContent()), QIODevice::ReadWrite);
    (void)DataStream.device()->reset();
    switch (Command.GetFileType()) {

        case NetCommands::PROGRAM:
            DataStream >> *ProgramList;
            ProgramList->SetDataVerificationMode(false);
            emit ProgramsInitialized();
            break;

        case NetCommands::REAGENT:
            DataStream >> *ReagentList;
            ReagentList->SetDataVerificationMode(false);
            emit ReagentUpdated();
            break;
        case NetCommands::REAGENTGROUP:            
            DataStream >> *ReagentGroupList;
            emit ReagentGroupUpdated();
            break;
        case NetCommands::STATION:
            DataStream >> *DashboardStationList;
            emit DashboardStationsUpdated();
            break;

        case NetCommands::REAGENTGROUPCOLOR:
              DataStream >> *ReagentGroupColorList;
              emit ReagentGroupColorUpdated();
              break;

        case NetCommands::USER_SETTING:
            DataStream >> *SettingsInterface;
            SettingsInterface->SetDataVerificationMode(false);
            ServiceStart();
            emit UserSettingsUpdated();
            break;
        case NetCommands::DEVICE_CONFIGURATION:
            DataStream >> *DeviceConfigurationInterface;
            DeviceConfigurationInterface->SetDataVerificationMode(false);
            qDebug()<<"Receive NetCommands::DEVICECONFIGURATION \n";
            emit DeviceConfigurationUpdated();
            break;
        default:
            break;
    }

    mp_WaitDialog->accept();


    /*! Device Configuration is the last among all the XML's sent from Main. We
     *  inform main that GUI is intialized.
     */
    if (m_GuiInit && m_GuiAllConfigurationReceived && Command.GetFileType() == NetCommands::DEVICE_CONFIGURATION) {
        qDebug()<<"Sending GUI INIT COMMAND \n";
        //end of initialization
        m_GuiInit = false;
        NetCommands::CmdGuiInit Cmd(COMMAND_TIME_OUT, true);
        (void)m_NetworkObject.SendCmdToMaster(Cmd, &CDataConnector::OnAckTwoPhase, this);
        emit EndGUIInitialization();
    }
    return;
}

/****************************************************************************/
/*!
 *  \brief Handles incoming Process state commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = Process State command
 */
/****************************************************************************/
void CDataConnector::ProcessStateHandler(Global::tRefType Ref, const NetCommands::CmdProcessState &Command)
{
    bool Result = false;
    NetCommands::ProcessStateType processStateType = Command.GetProcessState();
    if (NetCommands::ProcessStateType::InitState == processStateType ||
            NetCommands::ProcessStateType::IdleState == processStateType ||
            NetCommands::ProcessStateType::BusyState == processStateType ||
            NetCommands::ProcessStateType::NormalState == processStateType ) {
            Core::CGlobalHelper::SetSystemErrorStatus(false);

        if (NetCommands::ProcessStateType::InitState == processStateType ||
                NetCommands::ProcessStateType::IdleState == processStateType)
        Result = mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::ProcessRunning);//not show running icon
    }
    else
    {
        if (NetCommands::ProcessStateType::ErrorState == processStateType){
           Core::CGlobalHelper::SetSystemErrorStatus(true);
        }
        Result = mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::ProcessRunning);//show running icon
    }
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(Result));
    mp_WaitDialog->accept();
    return;
}

/****************************************************************************/
/*!
 *  \brief Sends the Updated/Modified settings to the master
 *
 *  \iparam settings = Settings reference
 */
/****************************************************************************/
void CDataConnector::SendUpdatedSettings(DataManager::CUserSettings &settings)
{
    if (mp_UserSettingWaitDialog)
    {
        delete mp_UserSettingWaitDialog;
        mp_UserSettingWaitDialog = NULL;
    }
    mp_UserSettingWaitDialog = new MainMenu::CWaitDialog(mp_MainWindow);
    mp_UserSettingWaitDialog->HideAbort();
    CONNECTSIGNALSLOT(mp_UserSettingWaitDialog, Timeout(), mp_MessageDlg, Show());

    mp_UserSettingWaitDialog->SetDialogTitle(QApplication::translate("Core::CDataConnector", "Device Communication",
                                                          Q_NULLPTR, -1));
    mp_UserSettingWaitDialog->SetText(QApplication::translate("Core::CDataConnector", "Saving settings ...",
                                                   Q_NULLPTR, -1));
    mp_UserSettingWaitDialog->SetTimeout(100000);
    mp_UserSettingWaitDialog->show();

    QByteArray ByteArray;
    QDataStream SettingsDataStream(&ByteArray, QIODevice::ReadWrite);
    SettingsDataStream << settings;
    (void)SettingsDataStream.device()->reset();
    MsgClasses::CmdChangeUserSettings Command(COMMAND_TIME_OUT, SettingsDataStream);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnUserSettingsAck, this);
}
/****************************************************************************/
/*!
 *  \brief Sends the TestToneAlarm settings to the master
 *
 *  \iparam Volume = Volume increments
 *  \iparam Sound = Sound type
 *  \iparam Type =
 */
/****************************************************************************/
void CDataConnector::SendCmdPlayTestToneAlarm(quint8 Volume, quint8 Sound, Global::AlarmType Type)
{
    qDebug()<< Volume << Sound << Type;

    MsgClasses::CmdAlarmToneTest Command(COMMAND_TIME_OUT, Sound, Volume, Type);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->show();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CDataConnector::RetranslateUI()
{
   m_strCommunicationError = QApplication::translate("Core::CDataConnector", "Communication error. Please try again or contact your local service.", Q_NULLPTR, -1);
   m_strCommunicationErrorTitle = QApplication::translate("Core::CDataConnector", "Communication Error", Q_NULLPTR, -1);
   if (mp_MessageDlg)
   {
        mp_MessageDlg->SetTitle(m_strCommunicationErrorTitle);
   }
   m_strChangeNotSave = QApplication::translate("Core::CDataConnector", "The changes can not be saved.", Q_NULLPTR, -1);
   if (mp_MessageDlg)
   {
        mp_MessageDlg->SetText(m_strChangeNotSave);
   }
   m_strStartup = QApplication::translate("Core::CDataConnector", "Startup", Q_NULLPTR, -1);
   m_strInitDevCom = QApplication::translate("Core::CDataConnector", "Initializing device communication ...", Q_NULLPTR, -1);
   m_strDisconnect = QApplication::translate("Core::CDataConnector", "Disconnect", Q_NULLPTR, -1);
   m_strReinitDeviceCommunication = QApplication::translate("Core::CDataConnector", "Reinitializing device communication ...", Q_NULLPTR, -1);
   m_strError = QApplication::translate("Core::CDataConnector", "Error", Q_NULLPTR, -1);
   m_strInformation = QApplication::translate("Core::CDataConnector", "Information", Q_NULLPTR, -1);

   m_strWarning = QApplication::translate("Core::CDataConnector", "Warning", Q_NULLPTR, -1);
   m_strOK = QApplication::translate("Core::CDataConnector", "OK", Q_NULLPTR, -1);
   m_strDeviceCommunication = QApplication::translate("Core::CDataConnector", "Operation ongoing", Q_NULLPTR, -1);
   m_strSavingSettings = QApplication::translate("Core::CDataConnector", "Saving Settings ...", Q_NULLPTR, -1);
   m_strUserExport = QApplication::translate("Core::CDataConnector", "User Export", Q_NULLPTR, -1);
   m_strExportUserData = QApplication::translate("Core::CDataConnector", "Exporting user data ...", Q_NULLPTR, -1);

   m_strServiceExport = QApplication::translate("Core::CDataConnector", "Service Export", Q_NULLPTR, -1);
   m_strExportServiceData = QApplication::translate("Core::CDataConnector", "Exporting service data ...", Q_NULLPTR, -1);

   m_strImport = QApplication::translate("Core::CDataConnector", "Import", Q_NULLPTR, -1);
   m_strImportData = QApplication::translate("Core::CDataConnector", "Importing data ...", Q_NULLPTR, -1);

   m_strLogFile = QApplication::translate("Core::CDataConnector", "Log Files", Q_NULLPTR, -1);
   m_strGettingDailyLog = QApplication::translate("Core::CDataConnector", "Getting the daily run log file ...", Q_NULLPTR, -1);
}


/****************************************************************************/
/*!
 *  \brief This slot is called when  ---- is emitted.
 *
 *  \param Name =  QString type parameter
 *  \param ParamList =  QStringList type parameter
 */
/****************************************************************************/
void CDataConnector::SendDataImportExport(const QString Name, const QStringList &ParamList)
{
    QByteArray ByteArray;
    QString DialogTitle;
    QString DialogText;
    if (Name.contains("Export")) {
        // update the parameter list in the byte array
        foreach (QString ListValue, ParamList) {
            ByteArray.append(ListValue);
        }
        if (ParamList.contains("User")) {
            DialogTitle = m_strUserExport;
            DialogText = m_strExportUserData;
        }
        else {
            DialogTitle = m_strServiceExport;
            DialogText = m_strExportServiceData;
        }
        MsgClasses::CmdDataExport Command(Global::Command::MAXTIMEOUT, ByteArray);
        (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    }
    else {
        MsgClasses::CmdDataImport Command(Global::Command::MAXTIMEOUT, ByteArray);
        (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
        DialogTitle = m_strImport;
        DialogText = m_strImportData;
    }


    mp_WaitDialog->SetDialogTitle(DialogTitle);
    mp_WaitDialog->SetText(DialogText);

    mp_WaitDialog->show();

}
/****************************************************************************/
/*!
 *  \brief Send Software Update command to Main.
 *
 *  \iparam USBUpdate = True if SW update has to done using USB and
 *                      False if SW update has to done using remote care.
 */
/****************************************************************************/
void CDataConnector::SendSWUpdate(bool USBUpdate)
{
    NetCommands::CmdSWUpdate Command(Global::Command::MAXTIMEOUT, USBUpdate);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}


/****************************************************************************/
/*!
 *  \brief Handler for selection to import files
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = CmdDataImportFiles class object
 */
/****************************************************************************/
void CDataConnector::OnImportFileSelection(Global::tRefType Ref, const MsgClasses::CmdDataImportFiles &Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit DisplaySelectionDialog(Command.GetCommandData());
}

/****************************************************************************/
/*!
 *  \brief Send command to Main to import the required files
 *  \iparam FileList = List of files
 */
/****************************************************************************/
void CDataConnector::SendRequestedFilesToImport(QStringList FileList)
{
    // For the file selection cancel no need of displaying the wait dialog.
    if (FileList.count() > 0) {
        mp_WaitDialog->SetDialogTitle(m_strImport);
        mp_WaitDialog->SetText(m_strImportData);
        mp_WaitDialog->show();
    }
    MsgClasses::CmdDataImportFiles Command(120000, FileList);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

void CDataConnector::SendParaffinTemperatureChanged(int lastMeltPoint, int currentMeltPoint, bool reheat)
{
    MsgClasses::CmdParaffinMeltPointChanged Command(COMMAND_TIME_OUT, lastMeltPoint, currentMeltPoint, reheat);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

void  CDataConnector::SendBottleCheck()
{
    MsgClasses::CmdBottleCheck Command(COMMAND_TIME_OUT);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

/****************************************************************************/
/*!
 *  \brief Handles incoming Event String command
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = Event String command
 */
/****************************************************************************/
void CDataConnector::EventStringHandler(Global::tRefType Ref, const NetCommands::CmdEventStrings &Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    QByteArray EventsStringsData = Command.GetEventsStringsData();
    QDataStream DataStream(&EventsStringsData, QIODevice::ReadWrite);
    Global::tTranslations Translations;

    QDataStream::Status oldStatus = DataStream.status();
    DataStream.resetStatus();
    Translations.clear();

    quint32 n;
    DataStream >> n;

    for (quint32 i = 0; i < n; ++i)
    {
        if (DataStream.status() != QDataStream::Ok)
        {
            break;
        }
        QLocale::Language Key;
        Global::tLanguageData T;
        quint32 num;
        DataStream >> num >> T;
        Key = static_cast<QLocale::Language>(num);
        Translations.insertMulti(Key, T);
    }

    if (DataStream.status() != QDataStream::Ok)
        Translations.clear();
    if (oldStatus != QDataStream::Ok)
        DataStream.setStatus(oldStatus);

    QHashIterator<QLocale::Language, Global::tLanguageData> j(Translations);
    while (j.hasNext())
    {
        (void)j.next();
        //qDebug() << j.key() << ": " << j.value() << endl;
        Global::UITranslator::TranslatorInstance().SetLanguageData(j.key(), j.value(), false, false);

    }
    // set default language
    Global::UITranslator::TranslatorInstance().SetDefaultLanguage(SettingsInterface->GetUserSettings()->GetLanguage());
    // set fallback language
    Global::UITranslator::TranslatorInstance().SetFallbackLanguage(QLocale::English);
}

/****************************************************************************/
/*!
 *  \brief Handles CmdExecution State , informs Status screen about
 *         changes in execution state
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = Event String command
 */
/****************************************************************************/
void CDataConnector::ExecutionStateHandler(Global::tRefType Ref, const NetCommands::CmdExecutionStateChanged &Command)
{
    if (Command.m_Stop) {
        // Stop timers in rack button and dialogs associated with it.
        qDebug()<<" \n\n PROCESSING STOPPED, STOPPING TIMERS" <<Global::AdjustedTime::Instance().GetCurrentDateTime();
        emit StopTimers();
    }
    else {
        //Start timers, before starting calculate the updated endtimes.
        qDebug()<<"Starting Timers"<< Global::AdjustedTime::Instance().GetCurrentDateTime();
        emit ReCalculateEndTimes();
    }

    if (!Command.m_Stop && Command.m_WaitDialogFlag) {
        if (mp_WaitDlgExecChanged) {
            delete mp_WaitDlgExecChanged;
            mp_WaitDlgExecChanged = NULL;
        }

        mp_WaitDlgExecChanged = new MainMenu::CWaitDialog(mp_MainWindow);
        mp_WaitDlgExecChanged->HideAbort();

        if (Command.m_WaitDialogText == Global::SOFTWARE_UPDATE_TEXT) {
            mp_WaitDlgExecChanged->SetDialogTitle(QApplication::translate("Core::CDataConnector", "Software Update",
                                                                  Q_NULLPTR, -1));
            mp_WaitDlgExecChanged->SetText(QApplication::translate("Core::CDataConnector", "Updating the software ...",
                                                           Q_NULLPTR, -1));
            mp_WaitDlgExecChanged->show();
        }

    }
    else if (!Command.m_Stop && !(Command.m_WaitDialogFlag)) {
        if (mp_WaitDlgExecChanged == NULL) {
            //If wait dialog object is NULL return immediately
            return;
        }
        mp_WaitDlgExecChanged->accept();
    }

    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
}

/****************************************************************************/
/*!
 *  \brief Handles CmdLanguageFile
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = Event String command
 */
/****************************************************************************/
void CDataConnector::LanguageFileHandler(Global::tRefType Ref, const NetCommands::CmdLanguageFile &Command)
{
    QString fileName(QString("languagefile%1.qm").arg(m_LanguageChangeCount));
    if (QFile::exists(fileName))
    {
        (void)QFile::remove(fileName);
    }

    mp_LanguageFile = new QFile(fileName);
    if (!mp_LanguageFile->isOpen()) {
        (void)mp_LanguageFile->open(QIODevice::ReadWrite);
    }
    if (mp_LanguageFile->write(Command.GetLanguageData()) != Command.GetLanguageData().size()) {
        m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(false));
        (void)mp_LanguageFile->remove();
        delete mp_LanguageFile;
        mp_LanguageFile = NULL;
        return;
    }
    (void)mp_LanguageFile->flush();

    QLocale::Language lastLang = Global::UITranslator::TranslatorInstance().GetDefaultLanguage();
    QApplication* pApp =  dynamic_cast<QApplication*>(QCoreApplication::instance());

    if (!m_Translator.load(mp_LanguageFile->fileName())) {
        if ((lastLang == QLocale::English) && (QLocale::English == Command.GetCurrentLanuage()))
        {
            QEvent* e = new QEvent(QEvent::LanguageChange);
            QCoreApplication::postEvent(pApp, e);
        }
        else {
            qDebug()<<"\n\nTranslations failed";
            m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(false));
            (void)mp_LanguageFile->remove();
            delete mp_LanguageFile;
            mp_LanguageFile = NULL;
            return;
        }
    }
    qApp->installTranslator(&m_Translator);
    m_LanguageChangeCount++;

    if (mp_OldFile) {
        (void)mp_OldFile->remove();
        delete mp_OldFile;
    }
    mp_OldFile = mp_LanguageFile;

    QString strFontFamily;
    if (QLocale::Korean == Command.GetCurrentLanuage())
    {
        strFontFamily = "NanumGothic";
    }
    else if ((lastLang == QLocale::Korean) && (QLocale::Korean != Command.GetCurrentLanuage()))
    {
        strFontFamily = "FreeSans";
    }

    if  (!strFontFamily.isEmpty())
    {
        QFont font;
        font.setPointSize(11);
        font.setFamily(strFontFamily);
        pApp->setFont(font);
    }

    emit LanguageChanged(Global::LanguageToString(Command.GetCurrentLanuage()));
    Global::UITranslator::TranslatorInstance().SetDefaultLanguage(Command.GetCurrentLanuage());
    emit DashboardStationsUpdated();
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
}

/****************************************************************************/
/*!
 *  \brief Handles CmdChangeUserSettings
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = Change User Settings command
 */
/****************************************************************************/
void CDataConnector::SettingsUpdateHandler(Global::tRefType Ref, const MsgClasses::CmdChangeUserSettings &Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    QByteArray SettingsData(const_cast<QByteArray &>(Command.GetUserSettings()));
    QDataStream SettingsDataStream(&SettingsData, QIODevice::ReadWrite);
    SettingsDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataManager::CUserSettings settings;
    SettingsDataStream >> settings;
    bool Result = false;
    if (SettingsInterface) {
        Result = SettingsInterface->UpdateUserSettings(&settings);
        if (Result) {
            emit UserSettingsUpdated();
        }
    }
}

/****************************************************************************/
/****************************************************************************/
/*!
 *  \brief CmdEventReport Handler
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = EventReport Command
 */
/****************************************************************************/
void CDataConnector::EventReportHandler(Global::tRefType Ref, const NetCommands::CmdEventReport &Command)
{
//    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    QByteArray EventReportData(const_cast<QByteArray &>(Command.GetEventReportData()));
    QDataStream EventDataStream(&EventReportData, QIODevice::ReadWrite);
    EventDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    mp_MesgBoxManager->Manage(EventDataStream, Ref);
}

/****************************************************************************/
/*!
 *  \brief  Send Ack To CmdEventReport
 *  \iparam ClickedButton = Button clicked
 *  \iparam CmdRef = Command reference
 *  \iparam EventKey  = Event key
 */
/****************************************************************************/
void CDataConnector::OnEventReportAck(NetCommands::ClickedButton_t ClickedButton, Global::tRefType CmdRef, quint64 EventKey)
{
    m_NetworkObject.SendAckToMaster(CmdRef, NetCommands::CmdAcknEventReport(EventKey, ClickedButton));
}

/****************************************************************************/
/*!
 *  \brief Send the user level information to the Main
 *
 *  \iparam DataStream = Data stream refernce
 */
/****************************************************************************/
void CDataConnector::SendUserLevel(QDataStream &DataStream)
{
    mp_WaitDialog->SetDialogTitle(m_strDeviceCommunication);
    mp_WaitDialog->SetText(m_strSavingSettings);
    mp_WaitDialog->SetTimeout(30000);
    mp_WaitDialog->show();

    qint32 UserLevel;
    QString Password;
    (void)DataStream.device()->reset();
    DataStream >> UserLevel >> Password;
    (void)DataStream.device()->reset();
    qDebug()<<" Password = "<< DataStream.device()->readAll();
    NetCommands::CmdChangeUserLevel Command(COMMAND_TIME_OUT, (Global::GuiUserLevel)UserLevel, Password);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

/****************************************************************************/
/*!
 *  \brief Send the change password information to the Main
 *
 *  \iparam DataStream = Data stream refernce
 */
/****************************************************************************/
void CDataConnector::SendChangepassword(QDataStream &DataStream)
{
    QString Type;
    QString Password;
    (void)DataStream.device()->reset();
    DataStream >> Type >> Password;
    NetCommands::CmdChangeAdminPassword Command(COMMAND_TIME_OUT, Type, Password);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

/****************************************************************************/
/*!
 *  \brief User authentication handler
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = User authentication Command
 */
/****************************************************************************/
void CDataConnector::UserAuthenticationHandler(Global::tRefType Ref, const NetCommands::CmdChangeUserLevelReply &Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit UserAuthentication((qint32)Command.GetUserAuthenticatedLevel());
}

/****************************************************************************/
/*!
 *  \brief admin password change handler
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = CmdChangeAdminPassword class object
 */
/****************************************************************************/
void CDataConnector::AdminPasswordChangeHandler(Global::tRefType Ref, const NetCommands::CmdChangeAdminPasswordReply &Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit ChangeAdminPassword(Command.GetCommandType());
}

/****************************************************************************/
/*!
 *  \brief Day run log reply handler for the daily run log file names
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = CmdDayRunLogReply class object
 */
/****************************************************************************/
void CDataConnector::DayRunLogReplyHandler(Global::tRefType Ref, const NetCommands::CmdDayRunLogReply &Command)
{
    Q_UNUSED(Ref);
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit UpdateDayRunLogFileNames(Command.GetFileNames());
}
/****************************************************************************/
/*!
 *  \brief Day run log file handler for the daily run log file
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = CmdDayRunLogReplyFile class object
 */
/****************************************************************************/
void CDataConnector::DayRunLogReplyFileHandler(Global::tRefType Ref, const NetCommands::CmdDayRunLogReplyFile &Command)
{
    Q_UNUSED(Ref);
    mp_WaitDialog->accept();
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    QDataStream FileContentDataStream(const_cast<QByteArray *>(&Command.GetFileData()), QIODevice::ReadWrite);
    FileContentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    emit UpdateDayRunLogFileContent(FileContentDataStream);
}
/****************************************************************************/
/*!
 *  \brief Send the selected day run log file to Main
 *
 *  \iparam FileName = Name of the file
 */
/****************************************************************************/
void CDataConnector::SendSelectedDayRunLogFile(const QString &FileName)
{
    Global::GuiUserLevel userRole = (Global::GuiUserLevel)MainMenu::CMainWindow::GetCurrentUserRole();
    NetCommands::CmdDayRunLogRequestFile Command(COMMAND_TIME_OUT, userRole, FileName);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
    mp_WaitDialog->SetDialogTitle(m_strLogFile);
    mp_WaitDialog->SetText(m_strGettingDailyLog);
    mp_WaitDialog->show();
}

void CDataConnector::SendProgramAction(const QString& ProgramID,
                                       DataManager::ProgramActionType_t ActionType,
                                       int delayTime, int runDuration, const QString& reagentExpiredFlag)
{
    MsgClasses::CmdProgramAction Command(COMMAND_TIME_OUT, ProgramID, ActionType, delayTime, runDuration, reagentExpiredFlag);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

void CDataConnector::SendProgramSelected(const QString& ProgramID, int ParaffinStepIndex)
{
    MsgClasses::CmdProgramSelected Command(COMMAND_TIME_OUT, ProgramID, ParaffinStepIndex);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

void CDataConnector::SendKeepCassetteCount(int CassetteCount)
{
    MsgClasses::CmdKeepCassetteCount Command(COMMAND_TIME_OUT, CassetteCount);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

void CDataConnector::SendTakeOutSpecimenFinishedCMD()
{
    MsgClasses::CmdTakeOutSpecimenFinished Command(COMMAND_TIME_OUT);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

/****************************************************************************/
/*!
 *  \brief RequestDayRunLogFileNames
 */
/****************************************************************************/
void CDataConnector::RequestDayRunLogFileNames()
{
    NetCommands::CmdDayRunLogRequest Command(COMMAND_TIME_OUT,(int) MainMenu::CMainWindow::GetCurrentUserRole());
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}
/****************************************************************************/
/*!
 *  \brief OnCmdGuiInit
 */
/****************************************************************************/
void CDataConnector::OnCmdGuiInit(Global::tRefType Ref, const NetCommands::CmdGuiInit &Command)
{
    Q_UNUSED(Command)
    //Set GUI Init Flag to true. This will be set to false when last xml file is received from Master
    qDebug()<<"NetCommands::CmdGuiInit Received \n";
    m_GuiAllConfigurationReceived = true;
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
}
/****************************************************************************/
/*!
 *  \brief OnUserSettingsAck
 */
/****************************************************************************/
void CDataConnector::OnUserSettingsAck(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref);
    qDebug()<<"\n\n Ack.GetStatus() = "<< Ack.GetStatus();
    // just accept the wait dialog
    if (mp_UserSettingWaitDialog)
    {
        mp_UserSettingWaitDialog->accept();
        delete mp_UserSettingWaitDialog;
        mp_UserSettingWaitDialog = NULL;
    }
    if (Ack.GetStatus() == false) {
        if (Ack.GetText().length() > 0) {
            ShowMessageDialog(Ack.GetType(), Ack.GetText());
            (void)mp_MessageDlg->exec();
        }
        emit RevertChangedUserSettings();
    }
}

void CDataConnector::OnProgramAck(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref);
    qDebug()<<"\n\n Ack.GetStatus() = "<< Ack.GetStatus();
    // just accept the wait dialog
    mp_WaitDialog->accept();
    if (Ack.GetStatus() == false) {
        ShowMessageDialog(Ack.GetType(), Ack.GetText());
        if (mp_MessageDlg->exec() == (int)QDialog::Accepted ) {
            emit RevertChangedProgram();
        }
    }
}

void CDataConnector::OnReagentAck(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{
    Q_UNUSED(Ref);
    qDebug()<<"\n\n Ack.GetStatus() = "<< Ack.GetStatus();
    // just accept the wait dialog
    mp_WaitDialog->accept();
    if (Ack.GetStatus() == false) {
        ShowMessageDialog(Ack.GetType(), Ack.GetText());
        if (mp_MessageDlg->exec() == (int)QDialog::Accepted ) {
            emit RevertChangedReagent();
        }
    }
}
/****************************************************************************/
/*!
 *  \brief OnCurrentTabChanged
 *
 *  \param CurrentTabIndex = int type parameter
 */
/****************************************************************************/
void CDataConnector::OnCurrentTabChanged(int CurrentTabIndex)
{
    m_CurrentTabIndex = CurrentTabIndex;
}
/****************************************************************************/
/*!
 *  \brief ShowMessageDialog
 */
/****************************************************************************/
void CDataConnector::ShowMessageDialog(Global::GUIMessageType MessageType, QString MessageText)
{
    if (mp_MessageDlg) {
        delete mp_MessageDlg;
        mp_MessageDlg = NULL;
    }
    mp_MessageDlg = new MainMenu::CMessageDlg(mp_MainWindow);
    mp_MessageDlg->HideButtons();
    mp_MessageDlg->setModal(true);

    if (MessageText.length() > 0) {
        switch (MessageType) {
        case Global::GUIMSGTYPE_ERROR:
            mp_MessageDlg->SetIcon(QMessageBox::Critical);
            mp_MessageDlg->SetTitle(m_strError);
            break;
        case Global::GUIMSGTYPE_INFO:
            mp_MessageDlg->SetIcon(QMessageBox::Information);
	    mp_MessageDlg->SetTitle(m_strInformation);
            break;
        case Global::GUIMSGTYPE_WARNING:
            mp_MessageDlg->SetIcon(QMessageBox::Warning);
            mp_MessageDlg->SetTitle(m_strWarning);
            break;
        default:
            break;
        }
        mp_MessageDlg->SetText(MessageText);
        mp_MessageDlg->SetButtonText(1, m_strOK);
        mp_MessageDlg->show();
    }
}

void CDataConnector::CurrentProgramStepInfoHandler(Global::tRefType Ref, const MsgClasses::CmdCurrentProgramStepInfor & Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit CurrentProgramStepInforUpdated(Command);
}

void CDataConnector::EnterCleaningProgramHandler(Global::tRefType Ref, const MsgClasses::CmdEnterCleaningProgram& Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit TakeoutSpecimenWaitRunCleaning(Command.LastReagentGroupID());
}

void CDataConnector::ProgramAcknowledgeHandler(Global::tRefType Ref, const MsgClasses::CmdProgramAcknowledge& Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    switch( Command.AcknownedgeType())
    {
        case DataManager::PROGRAM_READY:
        {
             emit ProgramStartReady();
        }
        break;
        case DataManager::PROGRAM_SELFTEST_FAILED:
        {
             emit ProgramSelfTestFailed();
        }
        break;
        case DataManager::PROGRAM_SELFTEST_PASSED:
        {
             emit ProgramSelfTestPassed();
        }
        break;
        case DataManager::PROGRAM_WILL_COMPLETE:
        {
             emit ProgramWillComplete();
        }
        break;
        case DataManager::PROGRAM_ABORT_BEGIN:
        {
             emit ProgramBeginAbort();
        }
        break;
        case DataManager::PROGRAM_RUN_FINISHED:
        {
             emit ProgramCompleted();
        }
        break;
        case DataManager::PROGRAM_RUN_FINISHED_NO_CONTAMINATED:
        {
            emit ProgramCompleted(DataManager::COMPLETED_PROGRAM_GENERAL, false);
        }
        break;
        case DataManager::CLEANING_PROGRAM_COMPLETE_AS_SAFE_REAGENT:
        {
            emit CleanPrgmCompleteAsSafeReagent();
        }
        break;
        case DataManager::PROGRAM_RUN_FINISHED_AS_SAFE_REAGENT:
        {
            emit ProgramCompleted(DataManager::COMPLETED_PROGRAM_SAFE_REAGENT);
        }
        break;
        case DataManager::PROGRAM_RUN_FINISHED_AS_SAFE_REAGENT_NO_CONTAMINATED:
        {
            emit ProgramCompleted(DataManager::COMPLETED_PROGRAM_SAFE_REAGENT, false);
        }
        break;
        case DataManager::PROGRAM_RUN_FINISHED_AS_POWER_FAILURE:
        {
            emit ProgramCompleted(DataManager::COMPLETED_PROGRAM_POWER_FAILURE);
        }
        break;
        case DataManager::PROGRAM_RUN_FINISHED_AS_POWER_FAILURE_NO_CONTAMINATED:
        {
            emit ProgramCompleted(DataManager::COMPLETED_PROGRAM_POWER_FAILURE, false);
        }
        break;
        case DataManager::PROGRAM_RUN_BEGIN:
        {
             emit ProgramRunBegin();
             (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::ProcessRunning);
        }
        break;
        case DataManager::PROGRAM_PAUSE_FINISHED:
        {
            emit ProgramPaused();
            (void)mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::ProcessRunning);
        }
        break;
        case DataManager::PROGRAM_PAUSE_ENABLE:
        {
            emit EnablePauseButton(true);
        }
        break;
        case DataManager::PROGRAM_PAUSE_DISABLE:
        {
            emit EnablePauseButton(false);
        }
        break;
        case DataManager::PROGRAM_START_ENABLE:
        {
            emit EnableStartButton(true);
        }
        break;
        case DataManager::PROGRAM_START_DISABLE:
        {
            emit EnableStartButton(false);
        }
        break;

        case DataManager::PROGRAM_PAUSE_TIMEOUT_15MINTUES:
        {
            emit PauseTimeout15Mintues();
        }
        break;
        case DataManager::PROGRAM_SYSTEM_EEEOR:
        {
            emit UpdateProgramTimerStatus(false);
        }
        break;
        case DataManager::PROGRAM_SYSTEM_RC_RESTART:
        {
            emit UpdateProgramTimerStatus(true);
        }
        break;
        case DataManager::TISSUE_PROTECT_PASSED:
        {
            emit TissueProtectPassed(true);
        }
        break;
        case DataManager::TISSUE_PROTECT_PASSED_WARNING:
        {
            emit TissueProtectPassed(false);
        }
        break;
		case DataManager::OVEN_COVER_OPEN:
        {
            emit OvenCoverOpen();
        }
        break;
        case DataManager::RETORT_COVER_OPERN:
        {
            emit RetortCoverOpen();
        }
        break;
        case DataManager::PROGRAM_PRETEST_DONE:
        {
            emit PreTestDone();
        }
        break;
        case DataManager::POWER_FAILURE_MSG:
        {
            emit PowerFailureMsg();
        }
        break;
        case DataManager::CANCEL_PROGRAM_WILL_COMPLETE_PROMPT:
        {
            emit CancelProgramWillCompletePrompt();
        }
        break;
        case DataManager::CANCEL_TISSUE_PROTECT_PASSED_PROMPT:
        {
            emit CancelTissueProtectPassedPrompt();
        }
        break;
        case DataManager::CANCEL_RETORT_LID_OPEN_MSG_PROMPT:
        {
            emit CancelRetortCoverOpenMessagePrompt();
        }
        break;
        case DataManager::SHOW_PAUSE_MSG_DLG:
        {
            emit ShowMsgDialog(DataManager::PAUSE_MSG);
        }
        break;
        case DataManager::DISMISS_RESUME_MSG_DLG:
        case DataManager::DISMISS_PAUSING_MSG_DLG:
        {
            emit DismissMsgDialog();
        }
        break;
        case DataManager::SHOW_RESUME_MSG_DLG:
        {
            emit ShowMsgDialog(DataManager::REUSME_MSG);
        }
        break;
        case DataManager::WAIT_ROTARY_VALVE_HEATING_PROMPT:
        {
            emit WaitRotaryValveHeatingPrompt();
        }
        break;
        case DataManager::DISMISS_ROTARY_VALVE_HEATING_PROMPT:
        {
            emit DismissRotaryValveHeatingPrompt();
        }
        break;
        case DataManager::PROGRAM_COOLING_DWON:
        {
            emit CoolingDown();
        }
        break;
        case DataManager::ENABLE_BOTTLE_CHECK:
        {
            emit EnableBottleCheck(true);
        }
        break;
        case DataManager::DISABLE_BOTTLE_CHECK:
        {
            emit EnableBottleCheck(false);
        }
        break;
        case DataManager::PAUSE_HAS_BEGUN:
        {
            emit ProgramActionStopped(DataManager::PROGRAM_STATUS_PAUSED);
        }
        break;
        case DataManager::SYSTEM_BUSY:
        {
            if (mp_MainWindow)
            {
                (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::ProcessRunning);
            }
            Core::CGlobalHelper::SetSystemBusyStatus(true);
        }
        break;
        case DataManager::SYSTEM_IDLE:
        {
            if (mp_MainWindow)
            {
                (void)mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::ProcessRunning);
            }
            Core::CGlobalHelper::SetSystemBusyStatus(false);
        }
        break;
        case DataManager::PROGRAM_RESUME_AFTER_POWER_FAILURE:
        {
            emit ProgramResumeAfterPowerFailure();
        }
        break;
        case DataManager::MAINTAINANCE_TIMECOUNT_START:
        {
            emit MaintainanceTimecountStart();
        }
        break;
        default:
        {
            qDebug() << "Do Nothing";
        }
    }
}

void CDataConnector::ProgramAbortedHandler(Global::tRefType Ref, const MsgClasses::CmdProgramAborted& Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit ProgramAborted(Command.IsRetortContaminated());
}

void CDataConnector::ProgramSelectedReplyHandler(Global::tRefType Ref, const MsgClasses::CmdProgramSelectedReply & Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit ProgramSelectedReply(Command);
}

void CDataConnector::RetortLockStatusHandler(Global::tRefType Ref, const MsgClasses::CmdLockStatus & Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    if (Command.LockType() == DataManager::RETORT_LOCK)
    {
        if (Command.IsLocked())
        {
            mp_MesgBoxManager->EnableAllButton(true,"RT_LID_OPEN_CLOSE");
            mp_MesgBoxManager->SetRTLidLocked(true);

        }
        else
        {
            mp_MesgBoxManager->EnableAllButton(false,"RT_LID_OPEN_CLOSE");
            mp_MesgBoxManager->SetRTLidLocked(false);
        }
        emit RetortLockStatusChanged(Command);
    }
    else if (Command.LockType() == DataManager::PARAFFIN_BATH_LOCK)
    {
        if (Command.IsLocked())
        {
            mp_MesgBoxManager->SetOverDoorStatus(true);
            mp_MesgBoxManager->EnableAllButton(true,"OVEN_DOOR_CLOSED");
            emit OvenCoverClosed(true);
        }
        else
        {
            mp_MesgBoxManager->EnableAllButton(false,"OVEN_DOOR_CLOSED");
            mp_MesgBoxManager->SetOverDoorStatus(false);
            emit OvenCoverClosed(false);
        }
    }
}

void CDataConnector::RecoveryFromPowerFailureHandler(Global::tRefType Ref, const MsgClasses::CmdRecoveryFromPowerFailure & Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit RecoveryFromPowerFailure(Command);
    emit ProgramSelfTestFailed();
    emit ProgramStartReady();
    Core::CGlobalHelper::SetErrorHandlingFailed(Command.IsErrorHandlingFailed());
}

void CDataConnector::StationParaffinBathStatusHandler(Global::tRefType Ref, const MsgClasses::CmdStationSuckDrain & Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit StationSuckDrain(Command);
}

// RemoteCare / arthur 2014/07/14
/****************************************************************************/
/*!
 *  \brief Handler for Remote Care software update.
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = CmdRCSoftwareUpdate class object
 */
/****************************************************************************/
void CDataConnector::OnRCSoftwareUpdateHandler(Global::tRefType Ref, const RemoteCare::CmdRCSoftwareUpdate &Command)
{
    Q_UNUSED(Ref);
    Q_UNUSED(Command);

    if (mp_MessageDlg) {
        delete mp_MessageDlg;
        mp_MessageDlg = NULL;
    }

    bool EnableUpdateButton = false;

    mp_MessageDlg = new MainMenu::CMessageDlg(mp_MainWindow);
    mp_MessageDlg->SetTitle(QApplication::translate("Core::CDataConnector", "Information",
                                                    Q_NULLPTR, -1));
    mp_MessageDlg->SetButtonText(1, QApplication::translate("Core::CDataConnector", "OK",
                                                           Q_NULLPTR, -1));
    mp_MessageDlg->HideButtons();

    if(RemoteCare::SWUpdate_Available == Command.GetUpdateType()) {
        mp_MessageDlg->SetText(QApplication::translate("Core::CDataConnector",
                                                       "New software is available. Click on \"Remote SW Update\" to start the update.",
                                                       Q_NULLPTR, -1));
        EnableUpdateButton = true;
    }
    else if(RemoteCare::SWUpdate_NotAvailable == Command.GetUpdateType()) {
        mp_MessageDlg->SetText(QApplication::translate("Core::CDataConnector",
                        "New software is not available.",
                                                       Q_NULLPTR, -1));
        EnableUpdateButton = false;
    }
    else if(RemoteCare::SWUpdate_DownloadFailed == Command.GetUpdateType()) {
        mp_MessageDlg->SetText(QApplication::translate("Core::CDataConnector",
                        "Failed to download the new software from RCServer. Please contact service.",
                                                       Q_NULLPTR, -1));
        EnableUpdateButton = false;
    } else if (RemoteCare::SWUpdate_DownloadSuccess == Command.GetUpdateType()) {
        mp_MessageDlg->SetText(QApplication::translate("Core::CDataConnector",
                        "Downloading of the new software from remote server is complete.  Press \"Ok\" to start thesoftware update.",
                                                       Q_NULLPTR, -1));
        mp_MessageDlg->HideButtons();
        (void)mp_MessageDlg->exec();
       return;
    } else if (RemoteCare::SWUpdate_UpdateFailed == Command.GetUpdateType()) {
//        mp_MessageDlg->SetText(QApplication::translate("Core::CDataConnector",
//                        "Software update failed.",
//                                                       Q_NULLPTR, -1));
//        mp_MessageDlg->HideButtons();
//        (void)mp_MessageDlg->exec();
       return;
    } else {
        return;
    }

    mp_MessageDlg->HideButtons();
    if (mp_MessageDlg->exec() == (int)QDialog::Accepted) {
        emit EnableRemoteSWButton(EnableUpdateButton);
    }
}

/****************************************************************************/
/*!
 *  \brief Send Software Update from RemoteCare command to Main.
 */
/****************************************************************************/
void CDataConnector::SendRCSWUpdate()
{
    if (mp_MessageDlg) {
        delete mp_MessageDlg;
        mp_MessageDlg = NULL;
    }

    mp_MessageDlg = new MainMenu::CMessageDlg(mp_MainWindow);
    mp_MessageDlg->SetTitle(QApplication::translate("Core::CDataConnector", "Information",
                                                    Q_NULLPTR, -1));
    mp_MessageDlg->SetText(QApplication::translate("Core::CDataConnector",
                    "Now download the update package from the remote server. Please wait...",
                                                   Q_NULLPTR, -1));
    mp_MessageDlg->HideAllButtons();
    (void)mp_MessageDlg->show();

    RemoteCare::CmdRCSoftwareUpdate Command(COMMAND_TIME_OUT, RemoteCare::SWUpdate_StartDownload);
    (void)m_NetworkObject.SendCmdToMaster(Command, &CDataConnector::OnAckTwoPhase, this);
}

/****************************************************************************/
/*!
 *  \brief Handler for Remote Care software update.
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = CmdRCRequestRemoteSession class object
 */
/****************************************************************************/
void CDataConnector::OnRCRequestRemoteSessionHandler(Global::tRefType Ref, const RemoteCare::CmdRCRequestRemoteSession &Command)
{
    Q_UNUSED(Ref);
    Q_UNUSED(Command);

    if (mp_MessageDlg) {
        delete mp_MessageDlg;
        mp_MessageDlg = NULL;
    }

    if(RemoteCare::RemoteSession_Requested == Command.GetRequestType()) {
        mp_MessageDlg = new MainMenu::CMessageDlg(mp_MainWindow);
        mp_MessageDlg->SetTitle(QApplication::translate("Core::CDataConnector", "Confirmation Message",
                                                         Q_NULLPTR, -1));
        mp_MessageDlg->SetButtonText(1, QApplication::translate("Core::CDataConnector", "OK",
                                                               Q_NULLPTR, -1));
        mp_MessageDlg->SetButtonText(3, QApplication::translate("Core::CDataConnector", "Cancel",
                                                               Q_NULLPTR, -1));
        mp_MessageDlg->HideCenterButton();
        mp_MessageDlg->SetText(QApplication::translate("Core::CDataConnector", "Remote Session has been requested by a remote user.",
                                                        Q_NULLPTR, -1));
        if (mp_MessageDlg->exec() == (int)QDialog::Accepted) {
            // Send command CmdRCRequestRemoteSession on "ok" pressed.
            RemoteCare::CmdRCRequestRemoteSession Cmd(RC_REQUEST_COMMAND_TIME_OUT, RemoteCare::RemoteSession_Accepted);
            (void)m_NetworkObject.SendCmdToMaster(Cmd, &CDataConnector::OnAckTwoPhase, this);
        }
        else {
            // Send command CmdRCRequestRemoteSession on "Cancel" pressed.
            RemoteCare::CmdRCRequestRemoteSession Cmd(RC_REQUEST_COMMAND_TIME_OUT, RemoteCare::RemoteSession_Denied);
            (void)m_NetworkObject.SendCmdToMaster(Cmd, &CDataConnector::OnAckTwoPhase, this);
        }
    }
    else if(RemoteCare::RemoteSession_Ended == Command.GetRequestType()) {

        mp_MessageDlg = new MainMenu::CMessageDlg(mp_MainWindow);
        mp_MessageDlg->SetTitle(QApplication::translate("Core::CDataConnector", "Information Message",
                                                         Q_NULLPTR, -1));
        mp_MessageDlg->SetButtonText(1, QApplication::translate("Core::CDataConnector", "OK",
                                                               Q_NULLPTR, -1));
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(QApplication::translate("Core::CDataConnector", "Remote session has been ended by the remote user.",
                                                        Q_NULLPTR, -1));
        (void)mp_MessageDlg->exec();
    }
}

/****************************************************************************/
/*!
 *  \brief Handles incoming Remote care state commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = RemoteCare State command
 */
/****************************************************************************/
void CDataConnector::RemoteCareStateHandler(Global::tRefType Ref, const NetCommands::CmdRemoteCareState &Command)
{
    bool Result = false;

    Result = Command.GetRemoteCareState();
    emit SetRemoteCareIcon(Result);
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    return;
}

/****************************************************************************/
/*!
 *  \brief Handles incoming Bottle Check reply commands
 *
 *  \iparam Ref = Command reference
 *  \iparam Command = bottle check command
 */
/****************************************************************************/
void CDataConnector::BottleCheckReplyHandler(Global::tRefType Ref, const MsgClasses::CmdBottleCheckReply &Command)
{
    m_NetworkObject.SendAckToMaster(Ref, Global::AckOKNOK(true));
    emit BottleCheckReply(Command.StationID(), Command.BottleCheckStatusType());
    return;
}


} // end namespace Core

