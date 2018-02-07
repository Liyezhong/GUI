/****************************************************************************/
/*! \file DataConnector.h
 *
 *  \brief DataConnector definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-02-24 - 29-Nov-2012
 *   $Author:  $ M.Scherer,Ningu
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
#ifndef CORE_DATACONNECTOR_H
#define CORE_DATACONNECTOR_H

#include "Core/Include/DataContainer.h"
#include "Global/Include/Translator.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/MessageDlg.h"
#include "MainMenu/Include/WaitDialog.h"
#include "NetworkLayer/Include/MasterLinkDevice.h"

/************************** Commands ***********************************************/
#include "DataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h"
#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdProgramUpdate.h"
#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdNewProgram.h"
#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdProgramDeleteItem.h"
#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentAdd.h"
#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentUpdate.h"
#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentRemove.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Commands/Include/CmdReagentGroupUpdate.h"
#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationChangeReagent.h"
#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationResetData.h"
#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationSetAsEmpty.h"
#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationSetAsFull.h"
#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdUpdateStationReagentStatus.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdUpdateProgramEndTime.h"
#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataImportFiles.h"

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdCurrentProgramStepInfor.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAction.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdStationSuckDrain.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramSelectedReply.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdRecoveryFromPowerFailure.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdLockStatus.h"

#include <NetCommands/Include/CmdSWUpdate.h>

/********************** Net Commands ************************************************/
#include "NetCommands/Include/CmdConfigurationFile.h"
#include "NetCommands/Include/CmdProcessState.h"
#include "NetCommands/Include/CmdRemoteCareState.h"
#include "NetCommands/Include/CmdEventStrings.h"
#include "NetCommands/Include/CmdExecutionStateChanged.h"
#include "NetCommands/Include/CmdLanguageFile.h"
#include <NetCommands/Include/CmdEventReport.h>
#include <NetCommands/Include/CmdChangeUserLevel.h>
#include <NetCommands/Include/CmdChangeAdminPassword.h>
#include <NetCommands/Include/CmdChangeAdminPasswordReply.h>
#include <NetCommands/Include/CmdChangeUserLevelReply.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <NetCommands/Include/CmdDayRunLogReply.h>
#include <NetCommands/Include/CmdDayRunLogReplyFile.h>
#include <NetCommands/Include/CmdDayRunLogRequest.h>
#include <NetCommands/Include/CmdDayRunLogRequestFile.h>
#include <NetCommands/Include/CmdGuiInit.h>
#include <NetCommands/Include/CmdDataUpdate.h>
#include <NetCommands/Include/CmdSWUpdate.h>
#include "NetCommands/Include/CmdRCSoftwareUpdate.h"
#include "NetCommands/Include/CmdRCRequestRemoteSession.h"

#include "MainMenu/Include/MsgBoxManager.h"

namespace MsgClasses
{
    class CmdProgramAcknowledge;
    class CmdEnterCleaningProgram;
    class CmdProgramAborted;
    class CmdLockStatus;
    class CmdBottleCheckReply;
}



namespace Core {



///Leica consumables type
typedef enum {
    KIT,         ///< Leica Kit
    BOTTLE       ///< Leica Bottle
} LeicaConsumableType_t;
///Message box buttons
enum ClickedButton_t {
    OK_BUTTON,
    YES_BUTTON,
    CONTINUE_BUTTON,
    NO_BUTTON,
    CANCEL_BUTTON,
    STOP_BUTTON
};



typedef QHash<qint16, QString> ScheduledStations_t; //!< Key- Step number, Value- Station ID

/****************************************************************************/
/**
 *  \brief Controller connecting the data model to various views
 *
 *      This class receives commands from the network layer, stores the
 *      data in the data manager and notifies the different views of the
 *      changes.
 */
/****************************************************************************/
class CDataConnector : public DataManager::CDataContainer
{
    Q_OBJECT

public:
    CDataConnector(MainMenu::CMainWindow *p_Parent);
    virtual ~CDataConnector();

    void OnAckDateAndTime(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    void OnAckTwoPhase(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function EmitUpdateModeSignal
     *
     *  \param Mode = QString type parameter
     *
     *  \return from EmitUpdateModeSignal
     */
    /****************************************************************************/
    void EmitUpdateModeSignal(QString Mode);
    void OnAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    void RequestDayRunLogFileNames();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of function SendTakeOutSpecimenFinishedCMD
     *
     *
     *  \return from SendTakeOutSpecimenFinishedCMD
     */
    /****************************************************************************/
    void SendTakeOutSpecimenFinishedCMD();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of function OnUserSettingsAck
     *
     *  \param Ref = Ref will not be used
     *
     *  \param Ack = Acknowledge to be processed
     */
    /****************************************************************************/
    void OnUserSettingsAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of function OnProgramAck
     *
     *  \param Ref = Ref will not be used
     *
     *  \param Ack = Acknowledge to be processed
     */
    /****************************************************************************/
    void OnProgramAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    /****************************************************************************/
    /*!
     *  \brief Process the acknowledge of adding reagent
     *  \param Ref = reference to this acknowledge
     *  \param Ack = ok or no acknowledge
     */
    /****************************************************************************/
    void OnReagentAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);


public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of IsOvenDoorLocked
     */
    /****************************************************************************/
    bool IsOvenDoorLocked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendDateTime
     */
    /****************************************************************************/
    void SendDateTime(QDateTime DateTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendReagentUpdate
     */
    /****************************************************************************/
    void SendReagentUpdate(DataManager::CReagent &Reagent);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendReagentGroupUpdate
     */
    /****************************************************************************/
    void SendReagentGroupUpdate(DataManager::CReagentGroup &ReagentGroup);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendStationChangeReagent
     */
    /****************************************************************************/
    void SendStationChangeReagent(const QString& StationId, const QString& ReagentId);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendStationResetData
     *  \param  StationId = Which station will be reset
     */
    /****************************************************************************/
    void SendStationResetData(const QString& StationId);
    /****************************************************************************/
    /*!
     *  \brief  Send the inforamtion about set some station as empty
     *  \param  StationId for the operated station
     */
    /****************************************************************************/
    void SendStationSetAsEmpty(const QString StationId);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of SendStationSetAsFull
     *  \param  StationId = Station Id to set full
     */
    /****************************************************************************/
    void SendStationSetAsFull(const QString& StationId);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendRMSChanged
     */
    /****************************************************************************/
    void SendRMSChanged(Global::RMSOptions_t);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendUpdatedSettings
     */
    /****************************************************************************/
    void SendUpdatedSettings(DataManager::CUserSettings &Settings);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendCmdPlayTestToneAlarm
     */
    /****************************************************************************/
    void SendCmdPlayTestToneAlarm(quint8 Volume, quint8 Sound, Global::AlarmType Type);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendReagentAdd
     */
    /****************************************************************************/
    void SendReagentAdd(DataManager::CReagent &Reagent);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendReagentRemove
     */
    /****************************************************************************/
    void SendReagentRemove(QString ReagentID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendProgramUpdate
     */
    /****************************************************************************/
    void SendProgramUpdate(DataManager::CProgram &Program);
    /******************const int RC_REQUEST_COMMAND_TIME_OUT = 30000;        //!< Remote Care Command Timeout**********************************************************/
    /*!
     *  \brief  Definition/Declaration of SendProgramAdd
     */
    /****************************************************************************/
    void SendProgramAdd(DataManager::CProgram &Program);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendProgramRemove
     */
    /****************************************************************************/
    void SendProgramRemove(QString &ProgramID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendDataImportExport
     */
    /****************************************************************************/
    void SendDataImportExport(const QString Name, const QStringList &ParamList);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendSWUpdate
     */
    /****************************************************************************/
    void SendSWUpdate(bool USBUpdate);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendRCSWUpdate
     */
    /****************************************************************************/
    void SendRCSWUpdate();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendProgramAction
     *  \param  ProgramID = the selected Program ID
     *  \param  ActionType = operation type, eg.start/pause/abort
     *  \param  delayTime = the time to delay
     *  \param  runDuration = the time from now to End datetime
     *  \param  reagentExpiredFlag = the flag of expired reagent.
     */
    /****************************************************************************/
    void SendProgramAction(const QString& ProgramID, DataManager::ProgramActionType_t ActionType,
                           int delayTime = 0, int runDuration = 0, const QString& reagentExpiredFlag = "");
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendKeepCassetteCount
     *  \param CassetteCount = the input cassette count
     */
    /****************************************************************************/
    void SendKeepCassetteCount(int CassetteCount);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendProgramSelected
     *  \param ProgramID = the current Program ID
     *  \param ParaffinStepIndex = the first used paraffin step
     */
    /****************************************************************************/
    void SendProgramSelected(const QString& ProgramID, int ParaffinStepIndex);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendUserLevel
     */
    /****************************************************************************/
    void SendUserLevel(QDataStream &DataStream);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendChangepassword
     */
    /****************************************************************************/
    void SendChangepassword(QDataStream &DataStream);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendSelectedDayRunLogFile
     */
    /****************************************************************************/
    void SendSelectedDayRunLogFile(const QString &FileName);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnCurrentTabChanged
     */
    /****************************************************************************/
    void OnCurrentTabChanged(int CurrentTabIndex);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of SendAppQuitSystemPrepareShutdown
     *  \param  quitAppShutdownActionType = quit application or prepare shutdown;
     */
    /****************************************************************************/
    void SendAppQuitSystemPrepareShutdown(DataManager::QuitAppShutdownActionType_t quitAppShutdownActionType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendResetOperationDays
     */
    /****************************************************************************/
    void SendResetOperationDays(DataManager::ResetOperationHoursType_t);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of SendRequestedFilesToImport
     *  \param  FileList = requested file list
     */
    /****************************************************************************/
    void SendRequestedFilesToImport(QStringList FileList);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SendParaffinTemperatureChanged
     *  \param lastMeltPoint = the paraffin bath temprature in last set
     *  \param currentMeltPoint = the set paraffin bath temprature currently.
     *  \param reheat = true - reheat paraffin
     */
    /****************************************************************************/
    void SendParaffinTemperatureChanged(int lastMeltPoint, int currentMeltPoint, bool reheat);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of SendBotttleCheck
     *
     */
    /****************************************************************************/
    void SendBottleCheck();

signals:

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ReagentGroupUpdated
     */
    /****************************************************************************/
    void ReagentGroupUpdated();
    /****************************************************************************/
    /*!
     *  \brief Emit this signal when Stations XML sent has been sent by Main SW
     */
    /****************************************************************************/
    void DashboardStationsUpdated();

    /****************************************************************************/
    /*!
     *  \brief Emit this signal when the reagent is changed in the Station
     */
    /****************************************************************************/
    void DashboardStationChangeReagent(const QString&);

    /****************************************************************************/
    /*!
     *  \brief Emit this signal when the reset data command has been invoked
     *  from reagent screen
     */
    /****************************************************************************/
    void DashboardStationResetData();

    /****************************************************************************/
    /*!
     *  \brief Emit this signal when the set as empty command has been invoked
     *  from reagent screen
     */
    /****************************************************************************/
    void DashboardStationSetAsEmpty();

    /****************************************************************************/
    /*!
     *  \brief Emit this signal when the set as full command has been invoked
     *  from reagent screen
     */
    /****************************************************************************/
    void DashboardStationSetAsFull();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ReagentGroupColorUpdated
     */
    /****************************************************************************/
    void ReagentGroupColorUpdated();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramUpdated
     */
    /****************************************************************************/
    void ProgramUpdated();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramAdded
     */
    /****************************************************************************/
    void ProgramAdded();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramsInitialized
     */
    /****************************************************************************/
    void ProgramsInitialized();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramDeleted
     */
    /****************************************************************************/
    void ProgramDeleted();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ReagentUpdated
     */
    /****************************************************************************/
    void ReagentUpdated();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ReagentAdded
     */
    /****************************************************************************/
    void ReagentAdded();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UserSettingsUpdated
     */
    /****************************************************************************/
    void UserSettingsUpdated();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateMode
     */
    /****************************************************************************/
    void UpdateMode(const QString &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal DateTimeAcked
     */
    /****************************************************************************/
    void DateTimeAcked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RemoveProgram
     */
    /****************************************************************************/
    void RemoveProgram(QString &);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam EventsStringsData = Events data
     */
    /****************************************************************************/
    void UpdateEventStrings(QByteArray EventsStringsData);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal StopTimers
     */
    /****************************************************************************/
    void StopTimers();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ReCalculateEndTimes
     */
    /****************************************************************************/
    void ReCalculateEndTimes();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ShowManualProgramDlg
     */
    /****************************************************************************/
    void ShowManualProgramDlg(QString);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam AuthenticatedLevel = AuthenticatedLevel
     */
    /****************************************************************************/
    void UserAuthentication(const qint32 &AuthenticatedLevel);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam IsAuthenticated = Authenticated
     */
    /****************************************************************************/
    void ChangeAdminPassword(const QString &IsAuthenticated);

    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam FileNames = Dailyrunlog file names
     */
    /****************************************************************************/
    void UpdateDayRunLogFileNames(const QStringList &FileNames);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam DataStream = Dailyrunlog data stream
     */
    /****************************************************************************/
    void UpdateDayRunLogFileContent(const QDataStream &DataStream);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam IsBlgChangesDlg = Bathlayout changes dialog display flag
     */
    /****************************************************************************/
    void ShowPrioritizeScreenDlg(bool IsBlgChangesDlg);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal DeviceConfigurationUpdated
     */
    /****************************************************************************/
    void DeviceConfigurationUpdated();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RevertChangedUserSettings
     */
    /****************************************************************************/
    void RevertChangedUserSettings();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RevertChangedProgram
     */
    /****************************************************************************/
    void RevertChangedProgram();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RevertChangedReagent
     */
    /****************************************************************************/
    void RevertChangedReagent();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal EndGUIInitialization
     */
    /****************************************************************************/
    void EndGUIInitialization();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal StartProgramAction
     *  \param ActionType = program operation type(start/pause/abort)
     */
    /****************************************************************************/
    void StartProgramAction(DataManager::ProgramActionType_t ActionType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal CurrentProgramStepInforUpdated
     *  \param  Command = update the current program step information
     */
    /****************************************************************************/
    void CurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor & Command);
	/****************************************************************************/
	/*!
	 *  \brief  Definition/Declaration of signal ProgramStartReady
	 */
	/****************************************************************************/
	void ProgramStartReady();
    /****************************************************************************/
    /*!
     *  \brief  signal of Program selfTest failed
     */
    /****************************************************************************/
    void ProgramSelfTestFailed();
    /****************************************************************************/
    /*!
     *  \brief  signal of Program selfTest passed
     */
    /****************************************************************************/
    void ProgramSelfTestPassed();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramWillComplete
     */
    /****************************************************************************/
    void ProgramWillComplete();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal CancelProgramWillCompletePrompt
     */
    /****************************************************************************/
    void CancelProgramWillCompletePrompt();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal CancelTissueProtectPassedPrompt
     */
    /****************************************************************************/
    void CancelTissueProtectPassedPrompt();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal CancelRetortCoverOpenMessagePrompt
     */
    /****************************************************************************/
    void CancelRetortCoverOpenMessagePrompt();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal ShowMsgDialog
     *  \param  msgBoxType = message box type
     */
    /****************************************************************************/
    void ShowMsgDialog(DataManager::MsgBoxType_t msgBoxType);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal DismissMsgDialog
     */
    /****************************************************************************/
    void DismissMsgDialog();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal TissueProtectPassed
     */
    /****************************************************************************/
    void TissueProtectPassed(bool);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal OvenCoverOpen
     */
    /****************************************************************************/
    void OvenCoverOpen();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal OvenCoverClosed
     */
    /****************************************************************************/
    void OvenCoverClosed(bool cosed);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RetortCoverOpen
     */
    /****************************************************************************/
    void RetortCoverOpen();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal PowerFailureMsg
     */
    /****************************************************************************/
    void PowerFailureMsg();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramAborted
     *  \param  IsRetortContaminated = is retort contaminated?
     */
    /****************************************************************************/
    void ProgramAborted(bool IsRetortContaminated);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramBeginAbort
     */
    /****************************************************************************/
    void ProgramBeginAbort();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramCompleted
     *  \param  completedProgramType = the type of completed program
     *  \param  IsRetortContaminated =  whether Retort is contaminated or not
     */
    /****************************************************************************/
    void ProgramCompleted(DataManager::CompletedProgramType_t completedProgramType
                          = DataManager::COMPLETED_PROGRAM_GENERAL, bool IsRetortContaminated = true);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal ProgramCompleted
     */
    /****************************************************************************/
    void CleanPrgmCompleteAsSafeReagent();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramRunBegin
     */
    /****************************************************************************/
    void ProgramRunBegin();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramPaused
     */
    /****************************************************************************/
    void ProgramPaused();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal TakeoutSpecimenWaitRunCleaning
     *  \param lastReagentGroupID = lastReagentGroupID
     */
    /****************************************************************************/
    void TakeoutSpecimenWaitRunCleaning(const QString& lastReagentGroupID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RetortLockStatusChanged
     *  \param Command = command for retort lock status
     */
    /****************************************************************************/
    void RetortLockStatusChanged(const MsgClasses::CmdLockStatus& Command);
	
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted Remote care is started
     *
     *  \iparam ProcessState = True or False
     *
     */
    /****************************************************************************/
    void SetRemoteCareIcon(bool &ProcessState);
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted Process is started.
     *
     *  \iparam RemoteCareState = True or False
     */
    /****************************************************************************/
    void SetProcessStateIcon(bool &RemoteCareState);	
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramSelectedReply
     *  \param  Command = Program selection reply command
     */
    /****************************************************************************/
    void ProgramSelectedReply(const MsgClasses::CmdProgramSelectedReply& Command);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal StationSuckDrain, for animation start/stop
     *  \param  Command = command for suck or drain
     */
    /****************************************************************************/
    void StationSuckDrain(const MsgClasses::CmdStationSuckDrain& Command);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when
     *         GUI receives to import multiple files
     *
     *  \iparam FileList    List of files
     */
    /****************************************************************************/
    void DisplaySelectionDialog(QStringList FileList);
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when GUI receives remote care software update.
     *
     *  \iparam Status = True for enabling the checkbox else False.
     *
     */
    /****************************************************************************/
    void EnableRemoteSWButton(bool Status);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when Language changed acknowledgement is
     *        sent from main to GUI.
     *
     * \iparam CurrentLanguage
     */
    /****************************************************************************/
    void LanguageChanged(const QString& CurrentLanguage);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal EnablePauseButton
     * \iparam  bEnable enabled/disabled
     */
    /****************************************************************************/
    void EnablePauseButton(bool bEnable);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal EnableStartButton
     * \iparam  bEnable enabled/disabled
     */
    /****************************************************************************/
    void EnableStartButton(bool bEnable);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal PauseTimeout15Mintues
     */
    /****************************************************************************/
    void PauseTimeout15Mintues();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateProgramEndTime
     *  \param  EndTimeDiff = end time diff
     */
    /****************************************************************************/
    void UpdateProgramEndTime(int EndTimeDiff);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateProgramTimerStatus
     *  \param  enabled = enable/disable the timer for prorgram running status.
     */
    /****************************************************************************/
    void UpdateProgramTimerStatus(bool enabled);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal RecoveryFromPowerFailure, for enter system after power failure
     *  \param  Command = command for recovery from power failure
     */
    /****************************************************************************/
    void RecoveryFromPowerFailure(const MsgClasses::CmdRecoveryFromPowerFailure& Command);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal PreTestDone, Get the notification of pretest from Master
     *
     */
    /****************************************************************************/
    void PreTestDone();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal WaitRotaryValveHeatingPrompt
     */
    /****************************************************************************/
    void WaitRotaryValveHeatingPrompt();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal DismissRotaryValveHeatingPrompt
     */
    /****************************************************************************/
    void DismissRotaryValveHeatingPrompt();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal CoolingDown
     */
    /****************************************************************************/
    void CoolingDown();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal BottleCheckReply
     *  \param  stationID = station ID
     *  \param  bottleCheckStatusType = bottle Check status type
     */
    /****************************************************************************/
    void BottleCheckReply(const QString& stationID, DataManager::BottleCheckStatusType_t bottleCheckStatusType);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal EnableBottleCheck
     *  \param  bEnabled = true is enabled, false is disabled.
     *
     */
    /****************************************************************************/
    void EnableBottleCheck(bool bEnabled);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal ProgramActionStopped
     */
    /****************************************************************************/
    void ProgramActionStopped(DataManager::ProgramStatusType_t);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal ProgramResumeAfterPowerFailure
     */
    /****************************************************************************/
    void ProgramResumeAfterPowerFailure();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal MaintainanceTimecountStart
     */
    /****************************************************************************/
    void MaintainanceTimecountStart();

protected:
    void OnImportFileSelection(Global::tRefType Ref, const MsgClasses::CmdDataImportFiles &Command);

private:
    void ServiceStart();
    void ConfFileHandler(Global::tRefType Ref, const NetCommands::CmdConfigurationFile &Command);
    void ProcessStateHandler(Global::tRefType ref, const NetCommands::CmdProcessState &Command);
    void UpdateProgramHandler(Global::tRefType Ref, const MsgClasses::CmdProgramUpdate &Command);
    void AddNewProgramHandler(Global::tRefType Ref, const MsgClasses::CmdNewProgram &Command);
    void ProgramRemoveHandler(Global::tRefType Ref, const MsgClasses::CmdProgramDeleteItem &Command);
    void AddNewReagentHandler(Global::tRefType Ref, const MsgClasses::CmdReagentAdd &Command);
    void UpdateReagentHandler(Global::tRefType Ref, const MsgClasses::CmdReagentUpdate &Command);
    void UpdateReagentGroupHandler(Global::tRefType Ref, const MsgClasses::CmdReagentGroupUpdate &Command);
    void UpdateEndTimeHandler(Global::tRefType Ref, const MsgClasses::CmdUpdateProgramEndTime &Command);
    void UpdateStationChangeReagentHandler(Global::tRefType Ref, const MsgClasses::CmdStationChangeReagent &Command);
    void RemoteCareStateHandler(Global::tRefType ref, const NetCommands::CmdRemoteCareState &Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateStationResetDataHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdStationResetData type parameter
     *
     *  \return from UpdateStationResetDataHandler
     */
    /****************************************************************************/
    void UpdateStationResetDataHandler(Global::tRefType Ref, const MsgClasses::CmdStationResetData &Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateStationSetAsEmptyHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdStationSetAsEmpty type parameter
     *
     *  \return from UpdateStationSetAsEmptyHandler
     */
    /****************************************************************************/
    void UpdateStationSetAsEmptyHandler(Global::tRefType Ref, const MsgClasses::CmdStationSetAsEmpty &Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateStationSetAsFullHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdStationSetAsFull type parameter
     *
     *  \return from UpdateStationSetAsFullHandler
     */
    /****************************************************************************/
    void UpdateStationSetAsFullHandler(Global::tRefType Ref, const MsgClasses::CmdStationSetAsFull &Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateStationReagentStatus
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdUpdateStationReagentStatus type parameter
     *
     *  \return from UpdateStationReagentStatus
     */
    /****************************************************************************/
    void UpdateStationReagentStatus(Global::tRefType Ref, const MsgClasses::CmdUpdateStationReagentStatus &Command);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CurrentProgramStepInfoHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdCurrentProgramStepInfor type parameter
     *
     *  \return from CurrentProgramStepInfoHandler
     */
    /****************************************************************************/
    void CurrentProgramStepInfoHandler(Global::tRefType Ref, const MsgClasses::CmdCurrentProgramStepInfor & Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ProgramAcknowledgeHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdProgramAcknowledge type parameter
     *
     *  \return from ProgramAcknowledgeHandler
     */
    /****************************************************************************/
    void ProgramAcknowledgeHandler(Global::tRefType Ref, const MsgClasses::CmdProgramAcknowledge& Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function EnterCleaningProgramHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdEnterCleaningProgram type parameter
     *
     *  \return from EnterCleaningProgramHandler
     */
    /****************************************************************************/
    void EnterCleaningProgramHandler(Global::tRefType Ref, const MsgClasses::CmdEnterCleaningProgram& Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ProgramAbortedHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdProgramAborted type parameter
     *
     *  \return from ProgramAbortedHandler
     */
    /****************************************************************************/
    void ProgramAbortedHandler(Global::tRefType Ref, const MsgClasses::CmdProgramAborted& Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StationParaffinBathStatusHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdStationSuckDrain type parameter
     *
     *  \return from StationParaffinBathStatusHandler
     */
    /****************************************************************************/
    void StationParaffinBathStatusHandler(Global::tRefType Ref, const MsgClasses::CmdStationSuckDrain & Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ProgramSelectedReplyHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdProgramSelectedReply type parameter
     *
     *  \return from ProgramSelectedReplyHandler
     */
    /****************************************************************************/
    void ProgramSelectedReplyHandler(Global::tRefType Ref, const MsgClasses::CmdProgramSelectedReply & Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RetortLockStatusHandler
     *
     *  \param Ref = Global::tRefType type parameter
     *  \param Command =  MsgClasses::CmdLockStatus type parameter
     *
     *  \return from RetortLockStatusHandler
     */
    /****************************************************************************/
    void RetortLockStatusHandler(Global::tRefType Ref, const MsgClasses::CmdLockStatus & Command);
    void RecoveryFromPowerFailureHandler(Global::tRefType Ref, const MsgClasses::CmdRecoveryFromPowerFailure & Command);
    void EventStringHandler(Global::tRefType ref, const NetCommands::CmdEventStrings &Command);
    void ReagentRemoveHandler(Global::tRefType Ref, const MsgClasses::CmdReagentRemove &Command);
    void ExecutionStateHandler(Global::tRefType ref, const NetCommands::CmdExecutionStateChanged &Command);
    void LanguageFileHandler(Global::tRefType Ref, const NetCommands::CmdLanguageFile &Command);
    void SettingsUpdateHandler(Global::tRefType Ref, const MsgClasses::CmdChangeUserSettings &Command);
    void EventReportHandler(Global::tRefType Ref, const NetCommands::CmdEventReport &Command);
    void UserAuthenticationHandler(Global::tRefType Ref, const NetCommands::CmdChangeUserLevelReply &Command);
    void AdminPasswordChangeHandler(Global::tRefType Ref, const NetCommands::CmdChangeAdminPasswordReply &Command);
    void DayRunLogReplyFileHandler(Global::tRefType Ref, const NetCommands::CmdDayRunLogReplyFile &Command);
    void DayRunLogReplyHandler(Global::tRefType Ref, const NetCommands::CmdDayRunLogReply &Command);
    void OnCmdGuiInit(Global::tRefType Ref, const NetCommands::CmdGuiInit &Command);
    void ShowMessageDialog(Global::GUIMessageType MessageType, QString MessageText);
    void OnRCSoftwareUpdateHandler(Global::tRefType Ref, const RemoteCare::CmdRCSoftwareUpdate &Command);
    void OnRCRequestRemoteSessionHandler(Global::tRefType Ref,
                                          const RemoteCare::CmdRCRequestRemoteSession &Command);
    void BottleCheckReplyHandler(Global::tRefType Ref, const MsgClasses::CmdBottleCheckReply &Command);

    NetLayer::CMasterLinkDevice m_NetworkObject;    //!< The network layer
    MainMenu::CMainWindow *mp_MainWindow;           //!< Main window of the GUI
    MainMenu::CMessageDlg *mp_MessageDlg;           //!< Error message box
    MainMenu::CWaitDialog *mp_WaitDialog;           //!< Displayed when busy
    MainMenu::CWaitDialog *mp_UserSettingWaitDialog;           //!< Displayed when busy

    QFile *mp_LanguageFile;                         //!< Lanugage file object (qm file)
    QFile *mp_OldFile;                              //!< Old/Previous language file(qm file)
    QTranslator m_Translator;                       //!< Language translator
    QProcess* m_pServiceProcess;       ///<  Definition/Declaration of variable m_pServiceProcess
    quint32 m_LanguageChangeCount;                  //!< Count of language changes
    QHash<quint32, ScheduledStations_t> m_ScheduledStationsHash;    //!< Scheduled Stations hash
    MainMenu::CMsgBoxManager *mp_MesgBoxManager;              //!< Msg Box manager for CmdEventReport
    LeicaConsumableType_t m_ConsumableType;         ///< Leica Consumable type
    quint32 m_BottleCount;                          ///< Leica Bottle count
    bool m_GuiAllConfigurationReceived;             //!< True -if all configuration data (XML)from Main is received
    bool m_GuiInit;                                 //!< True - if Gui is getting initialized
    int m_CurrentTabIndex;       ///<  Definition/Declaration of variable m_CurrentTabIndex
    QString m_strCommunicationErrorTitle;       ///<  the title string for a messageBox
    QString m_strCommunicationError;       ///<  Definition/Declaration of variable m_strCommunicationError
    QString m_strChangeNotSave;       ///<  Definition/Declaration of variable m_strChangeNotSave
    QString m_strStartup;       ///<  Definition/Declaration of variable m_strStartup
    QString m_strInitDevCom;       ///<  Definition/Declaration of variable m_strInitDevCom
    QString m_strDisconnect;       ///<  Definition/Declaration of variable m_strDisconnect
    QString m_strReinitDeviceCommunication;       ///<  Definition/Declaration of variable m_strReinitDeviceCommunication
    QString m_strError;       ///<  Definition/Declaration of variable m_strError
    QString m_strInformation;       ///<  Definition/Declaration of variable m_strInformation
    QString m_strWarning;       ///<  Definition/Declaration of variable m_strWarning
    QString m_strOK;       ///<  Definition/Declaration of variable m_strOK
    QString m_strDeviceCommunication;       ///<  Definition/Declaration of variable m_strDeviceCommunication
    QString m_strSavingSettings;       ///<  Definition/Declaration of variable m_strSavingSettings
    QString m_strUserExport;            ///<  Definition/Declaration of variablem_strUserExport
    QString m_strExportUserData;       ///<  Definition/Declaration of variable m_strExportUserData
    QString m_strServiceExport;         ///<  Definition/Declaration of variable m_strServiceExport
    QString m_strExportServiceData;       ///<  Definition/Declaration of variable m_strExportServiceData
    QString m_strImport;            ///<  Definition/Declaration of variable m_strImport
    QString m_strImportData;       ///<  Definition/Declaration of variable m_strImportData
    QString m_strLogFile;       ///<  Definition/Declaration of variable m_strLogFile
    QString m_strGettingDailyLog;       ///<  Definition/Declaration of variable m_strGettingDailyLog
    MainMenu::CWaitDialog *mp_WaitDlgExecChanged;    //!< Displayed when Execution Chaged command recved

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnMasterConnected
     */
    /****************************************************************************/
    void OnMasterConnected(const QString &name);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnMasterDisconnected
     */
    /****************************************************************************/
    void OnMasterDisconnected(const QString &name);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnCmdTimeout
     */
    /****************************************************************************/
    void OnCmdTimeout(Global::tRefType Ref, const QString &CommandName);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SetDateTime
     */
    /****************************************************************************/
    void SetDateTime(QDateTime DateTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of RetranslateUI
     */
    /****************************************************************************/
    void RetranslateUI();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnEventReportAck
     */
    /****************************************************************************/
    void OnEventReportAck(NetCommands::ClickedButton_t ClickedButton, Global::tRefType CmdRef, quint64 EventKey);

};

} // end namespace Core

#endif // CORE_DATACONNECTOR_H

