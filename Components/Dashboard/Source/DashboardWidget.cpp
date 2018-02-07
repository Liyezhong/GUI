/****************************************************************************/
/*! \file Himalaya/HimalayaMain/Gui/Components/Dashboard/Source/DashboardWidget.cpp
 *
 *  \brief ReagentWidget Implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-10-22
 *   $Author:  $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "Dashboard/Include/DashboardWidget.h"
#include "ui_DashboardWidget.h"
#include "Core/Include/DataConnector.h"
#include "MainMenu/Include/MainWindow.h"
#include <Dashboard/Include/CommonString.h>
#include <Dashboard/Include/FavoriteProgramsPanelWidget.h>
#include "Dashboard/Include/CassetteNumberInputWidget.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdLockStatus.h"
#include "Core/Include/GlobalHelper.h"
#include "Dashboard/Include/DashboardDateTimeWidget.h"

//lint -e613

using namespace Dashboard;

QString CDashboardWidget::m_SelectedProgramId = "";

CDashboardWidget::CDashboardWidget(Core::CDataConnector *p_DataConnector,
                                     MainMenu::CMainWindow *p_Parent) :
    ui(new Ui::CDashboardWidget),
    mp_DataConnector(p_DataConnector),
    mp_MainWindow(p_Parent),
    m_ParaffinStepIndex(-1),
    mp_ProgramWillCompleteMsgDlg(NULL),
    mp_OvenLidOpenMsgDlg(NULL),
    mp_RemoveSpecimenDlg(NULL),
    mp_RemoveSpecimenWhenCompletedDlg(NULL),
    mp_RemoveSpecimenWhenAbortedDlg(NULL),
    m_TimeProposedForProgram(0),
    m_CostedTimeBeforeParaffin(0),
    m_ParaffinHeatingDuration(0),
    m_ProgramStartReady(false),
    m_CurProgramStepIndex(-1),
    m_ProcessRunning(false),
    m_IsDrainingWhenPrgrmCompleted(false),
    m_bIsFirstStepFixation(false),
    m_TotalCassette(0),
    m_HaveSucked(false),
    m_ProgramStageStatus(Undefined),
    m_ProgramStatus(Undefined_ProgramStatus),
    m_IsProgramAbortedOrCompleted(false),
    m_IsInAppendCasseteStatus(false),
    m_bWaitRotaryValveHeatingPrompt(false),
    m_ShowReadyStartPrompt(false)

{
    ui->setupUi(this);
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());

    ui->containerPanelWidget->SetPtrToMainWindow(mp_MainWindow, mp_DataConnector);
    ui->programPanelWidget->SetPtrToMainWindow(mp_MainWindow, mp_DataConnector);
    m_pUserSetting = mp_DataConnector->SettingsInterface->GetUserSettings();
    mp_ProgramList = mp_DataConnector->ProgramList;

    mp_wdgtDateTime = new Dashboard::CDashboardDateTimeWidget(this, mp_MainWindow);
    mp_wdgtDateTime->setModal(true);
    mp_wdgtDateTime->SetUserSettings(m_pUserSetting);

    m_pCheckRetortLidTimer = new QTimer(this);
    m_pCheckRetortLidTimer->setInterval(1000);
    CONNECTSIGNALSLOT(m_pCheckRetortLidTimer, timeout(), this, OnTimerCheckRetortLid());

    CONNECTSIGNALSLOT(mp_wdgtDateTime, OnSelectDateTime(const QDateTime &), this, OnSelectEndDateTime(const QDateTime &));
    CONNECTSIGNALSIGNAL(mp_wdgtDateTime, OnSelectDateTime(const QDateTime &), ui->programPanelWidget, OnSelectEndDateTime(const QDateTime &));

    CONNECTSIGNALSLOT(mp_wdgtDateTime, RequestAsapDateTime(), this, RequestAsapDateTime());
    CONNECTSIGNALSLOT(this, SendAsapDateTime(int, bool, bool), mp_wdgtDateTime, OnGetASAPDateTime(int, bool, bool));

    CONNECTSIGNALSIGNAL(this, ResetFocus(bool), ui->programPanelWidget, ResetFocus(bool));
    CONNECTSIGNALSIGNAL(this, AddItemsToFavoritePanel(bool), ui->programPanelWidget, AddItemsToFavoritePanel(bool));
    CONNECTSIGNALSIGNAL(this, UpdateItemsToFavoritePanel(), ui->programPanelWidget, AddItemsToFavoritePanel());

    CONNECTSIGNALSLOT(ui->programPanelWidget, PrepareSelectedProgramChecking(const QString&), this, PrepareSelectedProgramChecking(const QString&));
    CONNECTSIGNALSLOT(mp_DataConnector, ProgramSelectedReply(const MsgClasses::CmdProgramSelectedReply &),
                      this, OnProgramSelectedReply(const MsgClasses::CmdProgramSelectedReply&));

    CONNECTSIGNALSLOT(mp_DataConnector, RecoveryFromPowerFailure(const MsgClasses::CmdRecoveryFromPowerFailure &),
                      this, OnRecoveryFromPowerFailure(const MsgClasses::CmdRecoveryFromPowerFailure&));

    CONNECTSIGNALSIGNAL(this, ProgramSelected(QString&, int, bool, QList<QString>&, int),
                       ui->programPanelWidget, ProgramSelected(QString&, int, bool, QList<QString>&, int));

    CONNECTSIGNALSIGNAL(this, UndoProgramSelection(),
                       ui->programPanelWidget, UndoProgramSelection());

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramStartReady(),
                      this, OnProgramStartReadyUpdated());

    CONNECTSIGNALSLOT(mp_DataConnector, RetortLockStatusChanged(const MsgClasses::CmdLockStatus &),
                      this, OnRetortLockStatusChanged(const MsgClasses::CmdLockStatus&));


    CONNECTSIGNALSIGNAL(this, ProgramSelected(QString&, QList<QString>&),
                       ui->containerPanelWidget, ProgramSelected(QString&, QList<QString>&));

    CONNECTSIGNALSIGNAL(this, ProgramActionStarted(DataManager::ProgramActionType_t, int, const QDateTime&, bool),
                        ui->programPanelWidget, ProgramActionStarted(DataManager::ProgramActionType_t, int,
                                                                     const QDateTime&, bool));

    CONNECTSIGNALSIGNAL(this, ProgramActionStopped(DataManager::ProgramStatusType_t),
                        ui->programPanelWidget, ProgramActionStopped(DataManager::ProgramStatusType_t));

    CONNECTSIGNALSLOT(ui->programPanelWidget, CheckPreConditionsToRunProgram(),
                      this, CheckPreConditionsToRunProgram());

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramWillComplete(),
                      this, OnProgramWillComplete());

    CONNECTSIGNALSLOT(mp_DataConnector, CancelProgramWillCompletePrompt(),
                      this, CancelProgramWillCompletePrompt());

    CONNECTSIGNALSLOT(mp_DataConnector, CancelRetortCoverOpenMessagePrompt(),
                      this, CancelRetortCoverOpenMessage());

    CONNECTSIGNALSLOT(mp_DataConnector, CancelTissueProtectPassedPrompt(),
                      this, CancelTissueProtectPassedPrompt());

    CONNECTSIGNALSLOT(mp_DataConnector, TissueProtectPassed(bool),
                      this, OnTissueProtectPassed(bool));

    CONNECTSIGNALSLOT(mp_DataConnector, OvenCoverOpen(),
                      this, OnOvenCoverOpen());
    CONNECTSIGNALSLOT(mp_DataConnector, OvenCoverClosed(bool),
                      this, OnOvenCoverClosed(bool));

    CONNECTSIGNALSLOT(mp_DataConnector, RetortCoverOpen(),
                      this, OnRetortCoverOpen());

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramAborted(bool),
                      this, OnProgramAborted(bool));

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramBeginAbort(),
                              this, OnProgramBeginAbort());

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramCompleted(DataManager::CompletedProgramType_t, bool),
                              this, OnProgramCompleted(DataManager::CompletedProgramType_t, bool));
    CONNECTSIGNALSLOT(mp_DataConnector, CleanPrgmCompleteAsSafeReagent(),
                              this, OnCleanPrgmCompleteAsSafeReagent());
    CONNECTSIGNALSLOT(mp_DataConnector, ProgramRunBegin(),
                              this, OnProgramRunBegin());

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramPaused(),
                              this, OnProgramPaused());

    CONNECTSIGNALSLOT(mp_DataConnector, EnableStartButton(bool),
                              this, OnStartButtonEnable(bool));

    CONNECTSIGNALSLOT(mp_DataConnector, EnablePauseButton(bool),
                              this, OnPauseButtonEnable(bool));

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramResumeAfterPowerFailure(),
                              this, OnProgramResumeAfterPowerFailure());

    CONNECTSIGNALSLOT(mp_DataConnector, PauseTimeout15Mintues(),
                              this, OnPauseTimeout15Mintues());

    CONNECTSIGNALSLOT(mp_DataConnector, TakeoutSpecimenWaitRunCleaning(const QString&),
                              this, TakeOutSpecimenAndWaitRunCleaning(const QString&));

    CONNECTSIGNALSIGNAL(mp_DataConnector, CurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor &),
                      ui->programPanelWidget, CurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor &));

    CONNECTSIGNALSLOT(mp_DataConnector, PreTestDone(),
                      ui->programPanelWidget, OnPreTestDone());

    CONNECTSIGNALSLOT(mp_DataConnector, PowerFailureMsg(),
                      this, OnPowerFailureMsg());

    CONNECTSIGNALSLOT(mp_DataConnector, ShowMsgDialog(DataManager::MsgBoxType_t),
                      this, OnShowMsgDialog(DataManager::MsgBoxType_t));

    CONNECTSIGNALSLOT(mp_DataConnector, DismissMsgDialog(),
                      this, OnDismissMsgDialog());

    CONNECTSIGNALSLOT(mp_DataConnector, WaitRotaryValveHeatingPrompt(),
                      this, OnWaitRotaryValveHeatingPrompt());

    CONNECTSIGNALSLOT(mp_DataConnector, DismissRotaryValveHeatingPrompt(),
                      this, OnDismissRotaryValveHeatingPrompt());

    CONNECTSIGNALSLOT(this, SwitchToFavoritePanel(),
                      ui->programPanelWidget, SwitchToFavoritePanel());

    CONNECTSIGNALSLOT(mp_DataConnector, CurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor &),
                      this, OnCurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor &));

    CONNECTSIGNALSLOT(this, UpdateUserSetting(DataManager::CUserSettings&), mp_DataConnector, SendUpdatedSettings(DataManager::CUserSettings&));
    CONNECTSIGNALSLOT(mp_DataConnector, StationSuckDrain(const MsgClasses::CmdStationSuckDrain &),
                      this, OnStationSuckDrain(const MsgClasses::CmdStationSuckDrain &));
    CONNECTSIGNALSIGNAL(this, OnInteractStart(), ui->containerPanelWidget, OnInteractStart());

    CONNECTSIGNALSIGNAL(this, UpdateProgram(DataManager::CProgram&), ui->programPanelWidget, UpdateProgram(DataManager::CProgram&));
	
	CONNECTSIGNALSLOT(mp_DataConnector, CoolingDown(), this, OnCoolingDown());
	
    CONNECTSIGNALSIGNAL(mp_DataConnector, ProgramActionStopped(DataManager::ProgramStatusType_t),
                        ui->programPanelWidget, ProgramActionStopped(DataManager::ProgramStatusType_t));

}

CDashboardWidget::~CDashboardWidget()
{
    try {
            delete ui;
			delete m_pCheckRetortLidTimer;
        }
        catch (...) {
            // to please Lint.
        }
}

void CDashboardWidget::OnCurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor & cmd)
{
    if (!m_SelectedProgramId.isEmpty() && m_SelectedProgramId.at(0) == 'C')
    return;

    if (m_ProgramStatus != ProgramRunning)
     return;

    m_CurProgramStepIndex = cmd.CurProgramStepIndex();
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
    {
        if ((cmd.CurProgramStepIndex() < 3) && (m_ProgramStageStatus == Enabled))
        {
            ui->programPanelWidget->EnablePauseButton(true);
        }

        if (cmd.CurProgramStepIndex() > 2)
        {
            ui->programPanelWidget->EnablePauseButton(false);
        }

        if (m_CurProgramStepIndex == 0)
        {
            ui->programPanelWidget->EnableStartButton(true);
        }
        else
        {
            ui->programPanelWidget->EnableStartButton(false);
        }
    }
    else//supervisor
    {
         if (m_ProgramStageStatus == Enabled)
         {
            ui->programPanelWidget->EnablePauseButton(true);
         }
    }
}

void CDashboardWidget::OnRetortLockStatusChanged(const MsgClasses::CmdLockStatus& cmd)
{
    if (m_IsInAppendCasseteStatus)
        return;

    m_IsInAppendCasseteStatus = true;
    m_bRetortLocked = cmd.IsLocked();

    if (cmd.IsLocked())
    {
        if ((m_CurProgramStepIndex < 3) && (m_ProgramStatus == Paused) && (m_pUserSetting->GetModeRMSProcessing() == Global::RMS_CASSETTES) && (m_SelectedProgramId.at(0) != 'C'))
        {
            MainMenu::CMessageDlg messageDlg(this);
            messageDlg.SetIcon(QMessageBox::Information);
            messageDlg.SetTitle(CommonString::strInforMsg);
            messageDlg.SetText(m_strAddCassete);
            messageDlg.SetButtonText(1, CommonString::strYes);
            messageDlg.SetButtonText(3, CommonString::strNo);
            messageDlg.HideCenterButton();
            if (messageDlg.exec())
            {
                SetCassetteNumber();
            }
        }
    }
    m_IsInAppendCasseteStatus = false;
}

void CDashboardWidget::OnProgramStartReadyUpdated()
{
    m_ProgramStartReady = true;
    ui->programPanelWidget->ProgramStartReady(true);
	m_bWaitRotaryValveHeatingPrompt = false;
    if (m_ProgramStatus != Aborting)
    {
        ui->programPanelWidget->OnProgramStartReadyUpdated();
        if (m_ShowReadyStartPrompt)
        {
            MainMenu::CMessageDlg ConfirmationMessageDlg;
            ConfirmationMessageDlg.SetTitle(CommonString::strConfirmMsg);
            ConfirmationMessageDlg.SetText(m_strReadyStartProgram);
            ConfirmationMessageDlg.SetIcon(QMessageBox::Information);
            ConfirmationMessageDlg.SetButtonText(1, CommonString::strOK);
            ConfirmationMessageDlg.HideButtons();
            (void)ConfirmationMessageDlg.exec();
            m_ShowReadyStartPrompt = false;
        }
    }
}

void CDashboardWidget::CancelProgramWillCompletePrompt()
{
    if (mp_ProgramWillCompleteMsgDlg)
        mp_ProgramWillCompleteMsgDlg->reject();
}

void CDashboardWidget::OnProgramWillComplete()
{
    mp_ProgramWillCompleteMsgDlg = new MainMenu::CMessageDlg(this);
    mp_ProgramWillCompleteMsgDlg->SetIcon(QMessageBox::Information);
    mp_ProgramWillCompleteMsgDlg->SetTitle(CommonString::strConfirmMsg);
    QString strTemp(m_strProgramWillComplete);
    strTemp = strTemp.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
    mp_ProgramWillCompleteMsgDlg->SetText(strTemp);
    mp_ProgramWillCompleteMsgDlg->SetButtonText(1, CommonString::strOK);
    mp_ProgramWillCompleteMsgDlg->HideButtons();

    emit ProgramActionStopped(DataManager::PROGRAM_STATUS_PAUSED);//pause ProgressBar and EndTime countdown

    if (mp_ProgramWillCompleteMsgDlg->exec())
    {
        m_IsDrainingWhenPrgrmCompleted = true;

        //Resume EndTime countdown
        emit ProgramActionStarted(DataManager::PROGRAM_START, 0, Global::AdjustedTime::Instance().GetCurrentDateTime(), true);

        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_DRAIN);
        //disable pause and abort
        ui->programPanelWidget->EnableStartButton(false);
        ui->programPanelWidget->EnablePauseButton(false);
        delete mp_ProgramWillCompleteMsgDlg;
        mp_ProgramWillCompleteMsgDlg = NULL;
        return;
    }
    delete mp_ProgramWillCompleteMsgDlg;
    mp_ProgramWillCompleteMsgDlg = NULL;
}

void CDashboardWidget::CancelTissueProtectPassedPrompt()
{
    if (mp_TissueProtectPassedMsgDlg)
        mp_TissueProtectPassedMsgDlg->reject();
}

void CDashboardWidget::OnTissueProtectPassed(bool flag)
{
    mp_TissueProtectPassedMsgDlg = new MainMenu::CMessageDlg(this);
    if (flag)
    {
        mp_TissueProtectPassedMsgDlg->SetIcon(QMessageBox::Information);
        mp_TissueProtectPassedMsgDlg->SetText(m_strTissueProtectPassed);
        mp_TissueProtectPassedMsgDlg->SetTitle(CommonString::strConfirmMsg);
    }
    else
    {
        mp_TissueProtectPassedMsgDlg->SetIcon(QMessageBox::Warning);
        mp_TissueProtectPassedMsgDlg->SetText(m_strTissueProtectPassed_Warning);
        mp_TissueProtectPassedMsgDlg->SetTitle(CommonString::strWarning);
    }

    mp_TissueProtectPassedMsgDlg->SetButtonText(1, CommonString::strOK);
    mp_TissueProtectPassedMsgDlg->HideButtons();

    if (mp_TissueProtectPassedMsgDlg->exec())
    {
        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_DRAIN_SR);
        delete mp_TissueProtectPassedMsgDlg;
        mp_TissueProtectPassedMsgDlg = NULL;
        return;
    }

    delete mp_TissueProtectPassedMsgDlg;
    mp_TissueProtectPassedMsgDlg = NULL;
}

void CDashboardWidget::OnOvenCoverOpen()
{
    if(mp_OvenLidOpenMsgDlg)
    {
        delete mp_OvenLidOpenMsgDlg;
        mp_OvenLidOpenMsgDlg = NULL;
    }
    mp_OvenLidOpenMsgDlg = new MainMenu::CMessageDlg(this);
    mp_OvenLidOpenMsgDlg->SetIcon(QMessageBox::Information);
    mp_OvenLidOpenMsgDlg->SetTitle(CommonString::strConfirmMsg);
    mp_OvenLidOpenMsgDlg->SetText(m_strOvenCoverOpen);
    mp_OvenLidOpenMsgDlg->SetButtonText(1, CommonString::strOK);
    if(mp_DataConnector && !mp_DataConnector->IsOvenDoorLocked())
    {
        mp_OvenLidOpenMsgDlg->EnableButton(1, false);
    }
    mp_OvenLidOpenMsgDlg->HideButtons();
    if (mp_OvenLidOpenMsgDlg->exec())
    {
        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_OVEN_COVER_OPEN);
        delete mp_OvenLidOpenMsgDlg;
        mp_OvenLidOpenMsgDlg = NULL;
        return;
    }
}

void CDashboardWidget::OnOvenCoverClosed(bool closed)
{
    if(mp_OvenLidOpenMsgDlg)
    {
        mp_OvenLidOpenMsgDlg->EnableButton(1,closed);
    }
}

void CDashboardWidget::CancelRetortCoverOpenMessage()
{
    if(mp_RetortLidOpenMsgDlg)
        mp_RetortLidOpenMsgDlg->reject();
}

void CDashboardWidget::OnRetortCoverOpen()
{
    mp_RetortLidOpenMsgDlg = new MainMenu::CMessageDlg(this);
    mp_RetortLidOpenMsgDlg->SetIcon(QMessageBox::Critical);
    QString strTemp(m_strRetortCoverOpen);
    mp_RetortLidOpenMsgDlg->SetText(strTemp);
    mp_RetortLidOpenMsgDlg->SetButtonText(1, CommonString::strOK);
    mp_RetortLidOpenMsgDlg->HideButtons();

    if (mp_RetortLidOpenMsgDlg->exec())
    {
        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_RETORT_COVER_OPEN);
        delete mp_RetortLidOpenMsgDlg;
        mp_RetortLidOpenMsgDlg = NULL;
        return;
    }
    delete mp_RetortLidOpenMsgDlg;
    mp_RetortLidOpenMsgDlg = NULL;
}

void CDashboardWidget::OnPowerFailureMsg()
{
    MainMenu::CMessageDlg messageDlg(this);
    messageDlg.SetIcon(QMessageBox::Critical);
    messageDlg.SetTitle(CommonString::strConfirmMsg);
    QString strTemp(m_PowerFailureBoxTitle);
    messageDlg.SetText(strTemp);
    messageDlg.SetButtonText(1, CommonString::strOK);
    messageDlg.HideButtons();

    if (messageDlg.exec())
    {
        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_POWER_FAILURE_MSG);
        return;
    }
}


void CDashboardWidget::OnShowMsgDialog(DataManager::MsgBoxType_t msgBoxType_t)
{
    mp_PausingMsgDlg = new MainMenu::CMessageDlg(this);
    mp_PausingMsgDlg->SetIcon(QMessageBox::Information);
    if (DataManager::PAUSE_MSG == msgBoxType_t)
    {
         mp_PausingMsgDlg->SetText(m_strItIsPausing);
         mp_PausingMsgDlg->SetTextFormat(Qt::PlainText);
    }
    else if (DataManager::REUSME_MSG == msgBoxType_t)
    {
         mp_PausingMsgDlg->SetText(m_strItIsResuming);
    }

    mp_PausingMsgDlg->SetTextFormat(Qt::PlainText);
    mp_PausingMsgDlg->HideAllButtons();
    (void)mp_PausingMsgDlg->exec();
    delete mp_PausingMsgDlg;
    mp_PausingMsgDlg = NULL;
}

void CDashboardWidget::OnDismissMsgDialog()
{
    if (mp_PausingMsgDlg)
    {
        mp_PausingMsgDlg->accept();
    }
}

void CDashboardWidget::OnWaitRotaryValveHeatingPrompt()
{
    m_bWaitRotaryValveHeatingPrompt = true;
    ui->programPanelWidget->WaitRotaryValveHeatingPrompt(true);
    if (m_ProgramStatus != Aborting)
    {
        ui->programPanelWidget->OnWaitRotaryValveHeatingPrompt();
    }
}

void CDashboardWidget::OnDismissRotaryValveHeatingPrompt()
{
    m_bWaitRotaryValveHeatingPrompt = false;
    ui->programPanelWidget->WaitRotaryValveHeatingPrompt(false);
}

void CDashboardWidget::OnCoolingDown()
{
    MainMenu::CMessageDlg messageDlg(this);
    messageDlg.SetIcon(QMessageBox::Information);
    messageDlg.SetTitle(CommonString::strConfirmMsg);
    messageDlg.SetText(m_strCoolingDown);
    messageDlg.SetButtonText(1, CommonString::strOK);
    messageDlg.HideButtons();
    if (messageDlg.exec())
    {
        return;
    }
}

void CDashboardWidget::OnProgramBeginAbort()
{
    //show "aborting"
    //time countdown
    //Todo:20, Abort time, will be given a rough value later;
    emit ProgramActionStarted(DataManager::PROGRAM_ABORT, 90, Global::AdjustedTime::Instance().GetCurrentDateTime(), false);
    ui->programPanelWidget->EnableStartButton(false);
    ui->programPanelWidget->EnablePauseButton(false);
    m_ProgramStatus = Aborting;
    Core::CGlobalHelper::SetProgramPaused(false);
}

//this function will be invoked after program Abort and completed
void CDashboardWidget::TakeOutSpecimenAndWaitRunCleaning(const QString& lastReagentGroupID)
{
     if (!m_SelectedProgramId.isEmpty() && m_SelectedProgramId.at(0) == 'C')
     {
        m_ProgramStatus = Undefined_ProgramStatus;
        Core::CGlobalHelper::SetProgramPaused(false);
        //represent the retort as contaminated status
        ui->containerPanelWidget->UpdateRetortStatus(DataManager::CONTAINER_STATUS_CONTAMINATED, lastReagentGroupID, "");
        //only show Cleaning program in the favorite panel
        emit AddItemsToFavoritePanel(true);

        ui->programPanelWidget->ChangeStartButtonToStartState();
        ui->programPanelWidget->EnableStartButton(false);
        ui->programPanelWidget->EnablePauseButton(false);
        //show all Stations and pipes
        m_StationList.clear();
        m_SelectedProgramId = "";
        int asapEndTime = 0;
        emit ProgramSelected(m_SelectedProgramId, m_StationList);
        emit ProgramSelected(m_SelectedProgramId, asapEndTime, m_bIsFirstStepFixation, m_StationList, 0);

        //switch to the dashboard page
        mp_MainWindow->SetTabWidgetIndex();
        emit SwitchToFavoritePanel();
     }
     else
     {
            mp_RemoveSpecimenDlg = new MainMenu::CMessageDlg(this);
            mp_RemoveSpecimenDlg->SetIcon(QMessageBox::Information);
            mp_RemoveSpecimenDlg->SetButtonText(1, CommonString::strOK);
            mp_RemoveSpecimenDlg->HideButtons();
#if defined(__arm__)
            mp_RemoveSpecimenDlg->EnableButton(1, false);
#endif
            QString strMsgDisplay(m_strTakeOutSpecimen);
            QString strTemp;
            if (m_ProgramStatus == Completed)
            {
                strTemp = m_strProgramCompleteSuccessfully.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
            }
            else if (m_ProgramStatus == Aborting)
            {
                strTemp = m_strProgramIsAborted.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
            }
            else if (m_ProgramStatus == CompletedAsSafeReagent)
            {

                strTemp = m_strProgramCompleteSafeReagent.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
            }
            else if (m_ProgramStatus == CompletedAsPowerFailure)
            {

                strTemp = m_strProgramComplete.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
            }
            strMsgDisplay = strTemp + " " + m_strTakeOutSpecimen;
            m_ProgramStatus = Undefined_ProgramStatus;
            Core::CGlobalHelper::SetProgramPaused(false);


            mp_RemoveSpecimenDlg->SetTitle(CommonString::strConfirmMsg);
            mp_RemoveSpecimenDlg->SetText(strMsgDisplay);
			m_pCheckRetortLidTimer->start();
            if (mp_RemoveSpecimenDlg->exec())
            {
				m_pCheckRetortLidTimer->stop();
                mp_DataConnector->SendTakeOutSpecimenFinishedCMD();
                //represent the retort as contaminated status
                ui->containerPanelWidget->UpdateRetortStatus(DataManager::CONTAINER_STATUS_CONTAMINATED, lastReagentGroupID, "");

                mp_RemoveSpecimenDlg->SetText(m_strRetortContaminated);
                
                if (mp_RemoveSpecimenDlg->exec())
                {
                    //only show Cleaning program in the favorite panel
                    emit AddItemsToFavoritePanel(true);
                }

                ui->programPanelWidget->ChangeStartButtonToStartState();
                ui->programPanelWidget->EnableStartButton(false);
                ui->programPanelWidget->EnablePauseButton(false);
                //show all Stations and pipes
                m_StationList.clear();
                m_SelectedProgramId = "";
                int asapEndTime = 0;
                emit ProgramSelected(m_SelectedProgramId, m_StationList);
                emit ProgramSelected(m_SelectedProgramId, asapEndTime, m_bIsFirstStepFixation, m_StationList, 0);
                //switch to the dashboard page
                mp_MainWindow->SetTabWidgetIndex();
                emit SwitchToFavoritePanel();
            }
            delete mp_RemoveSpecimenDlg;
            mp_RemoveSpecimenDlg = NULL;
     }
}

void CDashboardWidget::SetCassetteNumber()
{
    if (mp_DataConnector)
    {
        //input cassette number
        CCassetteNumberInputWidget *pCassetteInput = new Dashboard::CCassetteNumberInputWidget(this, mp_MainWindow);
        pCassetteInput->SetCurrentCassette(m_TotalCassette);
        pCassetteInput->ShowLabel();
        pCassetteInput->SetDialogTitle(m_strChangeCassetteBoxTitle);
        (void)pCassetteInput->exec();

        int cassetteNumber = pCassetteInput->CassetteNumber();
        if (-1 == cassetteNumber)
            return;//clicked Cancel button

        m_TotalCassette += cassetteNumber;
        mp_DataConnector->SendKeepCassetteCount(cassetteNumber);
        delete pCassetteInput;
    }
}

void CDashboardWidget::OnProgramAborted(bool IsRetortContaminated)
{
    //progress aborted;
    //aborting time countdown is hidden.
    ui->programPanelWidget->IsResumeRun(false);
    m_CurProgramStepIndex = -1;

    emit ProgramActionStopped(DataManager::PROGRAM_STATUS_ABORTED);
    Core::CGlobalHelper::SetProgramPaused(false);
    if (!IsRetortContaminated)
    {
        mp_RemoveSpecimenWhenAbortedDlg = new MainMenu::CMessageDlg(this);
        mp_RemoveSpecimenWhenAbortedDlg->SetIcon(QMessageBox::Information);
        mp_RemoveSpecimenWhenAbortedDlg->SetTitle(CommonString::strConfirmMsg);
        QString strTemp = m_strProgramIsAborted.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
        mp_RemoveSpecimenWhenAbortedDlg->SetText(strTemp + " " + m_strTakeOutSpecimen);
        mp_RemoveSpecimenWhenAbortedDlg->SetButtonText(1, CommonString::strOK);
        mp_RemoveSpecimenWhenAbortedDlg->HideButtons();
#if defined(__arm__)
        mp_RemoveSpecimenWhenAbortedDlg->EnableButton(1, false);
#endif
        m_pCheckRetortLidTimer->start();
        m_ShowReadyStartPrompt = true;
        if (mp_RemoveSpecimenWhenAbortedDlg->exec())
        {
            m_pCheckRetortLidTimer->stop();
            mp_DataConnector->SendTakeOutSpecimenFinishedCMD();
            ui->programPanelWidget->ChangeStartButtonToStartState();
            ui->programPanelWidget->EnableStartButton(true);
            ui->programPanelWidget->EnablePauseButton(false);
            //switch to the dashboard page
            mp_MainWindow->SetTabWidgetIndex();
            emit SwitchToFavoritePanel();
            OnUnselectProgram();
        }

        delete mp_RemoveSpecimenWhenAbortedDlg;
        mp_RemoveSpecimenWhenAbortedDlg = NULL;
		m_ProgramStatus = Aborted;
    }
}

void CDashboardWidget::OnCleanPrgmCompleteAsSafeReagent()
{
    ui->programPanelWidget->IsResumeRun(false);
    m_CurProgramStepIndex = -1;
    m_IsDrainingWhenPrgrmCompleted = false;
    emit ProgramActionStopped(DataManager::PROGRAM_STATUS_COMPLETED);
    m_ProgramStatus = Completed;
    Core::CGlobalHelper::SetProgramPaused(false);

    MainMenu::CMessageDlg messageDlg(this);
    messageDlg.SetIcon(QMessageBox::Information);
    messageDlg.SetTitle(CommonString::strConfirmMsg);
    messageDlg.SetText(m_strRetortContaminated);
    messageDlg.SetButtonText(1, CommonString::strOK);
    messageDlg.HideButtons();
    (void)messageDlg.exec();
}

void CDashboardWidget::OnTimerCheckRetortLid()
{
    if (!m_bRetortLocked && mp_RemoveSpecimenDlg)
    {
        mp_RemoveSpecimenDlg->EnableButton(1, true);
    }

    if (!m_bRetortLocked && mp_RemoveSpecimenWhenAbortedDlg)
    {
        mp_RemoveSpecimenWhenAbortedDlg->EnableButton(1, true);
    }

    if (!m_bRetortLocked && mp_RemoveSpecimenWhenCompletedDlg)
    {
        mp_RemoveSpecimenWhenCompletedDlg->EnableButton(1, true);
    }
}

void CDashboardWidget::OnProgramResumeAfterPowerFailure()
{
    ui->programPanelWidget->ChangeStartButtonToStopState();
}

void CDashboardWidget::OnProgramCompleted(DataManager::CompletedProgramType_t completedProgramType,
                                          bool IsRetortContaminated)
{
    ui->programPanelWidget->IsResumeRun(false);
    m_CurProgramStepIndex = -1;
    m_IsDrainingWhenPrgrmCompleted = false;
    QString strTemp;
    if (DataManager::COMPLETED_PROGRAM_SAFE_REAGENT == completedProgramType)
    {
        strTemp = m_strProgramCompleteSafeReagent.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
    }
    else if (DataManager::COMPLETED_PROGRAM_POWER_FAILURE == completedProgramType)
    {
        strTemp = m_strProgramComplete.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
    }
    else
    {
        strTemp = m_strProgramCompleteSuccessfully.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
    }

    bool bExecSubsequent = false;
    mp_RemoveSpecimenWhenCompletedDlg = new MainMenu::CMessageDlg(this);
    mp_RemoveSpecimenWhenCompletedDlg->SetIcon(QMessageBox::Information);
    mp_RemoveSpecimenWhenCompletedDlg->SetButtonText(1, CommonString::strOK);
    mp_RemoveSpecimenWhenCompletedDlg->HideButtons();
    if (!m_SelectedProgramId.isEmpty() && m_SelectedProgramId.at(0) == 'C')
    {
        mp_RemoveSpecimenWhenCompletedDlg->SetTitle(CommonString::strInforMsg);
        mp_RemoveSpecimenWhenCompletedDlg->SetText(strTemp);
        m_ShowReadyStartPrompt = true;
        (void)mp_RemoveSpecimenWhenCompletedDlg->exec();
        bExecSubsequent = true;
    }
    else if (!IsRetortContaminated && (!m_SelectedProgramId.isEmpty() && m_SelectedProgramId.at(0) != 'C'))
    {
        mp_RemoveSpecimenWhenCompletedDlg->SetTitle(CommonString::strConfirmMsg);
        mp_RemoveSpecimenWhenCompletedDlg->SetText(strTemp + " " + m_strTakeOutSpecimen);
#if defined(__arm__)
        mp_RemoveSpecimenWhenCompletedDlg->EnableButton(1, false);
#endif
        m_pCheckRetortLidTimer->start();
        m_ShowReadyStartPrompt = true;
        (void)mp_RemoveSpecimenWhenCompletedDlg->exec();
        m_pCheckRetortLidTimer->stop();
        bExecSubsequent = true;
        mp_DataConnector->SendTakeOutSpecimenFinishedCMD();
    }

    delete mp_RemoveSpecimenWhenCompletedDlg;
    mp_RemoveSpecimenWhenCompletedDlg = NULL;

    if (bExecSubsequent)
    {
        ui->programPanelWidget->EnablePauseButton(false);
        emit AddItemsToFavoritePanel();
        ui->programPanelWidget->ChangeStartButtonToStartState();

        ui->programPanelWidget->EnableStartButton(false);

        m_StationList.clear();
        m_SelectedProgramId = "";
        int asapEndTime = 0;
        emit ProgramSelected(m_SelectedProgramId, m_StationList);
        emit ProgramSelected(m_SelectedProgramId, asapEndTime, m_bIsFirstStepFixation, m_StationList, 0);
    }

    emit ProgramActionStopped(DataManager::PROGRAM_STATUS_COMPLETED);
    if (DataManager::COMPLETED_PROGRAM_SAFE_REAGENT == completedProgramType)
        m_ProgramStatus = CompletedAsSafeReagent;
    else if (DataManager::COMPLETED_PROGRAM_POWER_FAILURE == completedProgramType)
        m_ProgramStatus = CompletedAsPowerFailure;
    else
        m_ProgramStatus = Completed;
    Core::CGlobalHelper::SetProgramPaused(false);
}

void CDashboardWidget::OnProgramRunBegin()
{
    bool isResumeRun = ui->programPanelWidget->IsResumeRun();
    if (!isResumeRun)
    {
        m_HaveSucked = false;
    }
    QDateTime curDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    int remainingTime = curDateTime.secsTo(m_EndDateTime);
    emit ProgramActionStarted(DataManager::PROGRAM_START, remainingTime, curDateTime, isResumeRun);

    if (m_SelectedProgramId.at(0) == 'C')
    {
        ui->programPanelWidget->EnablePauseButton(false);//disable pause button
        ui->programPanelWidget->EnableStartButton(false);//disable Stop button
    }
    else
    {
        if (isResumeRun)
        {
            if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
            {
                if (m_CurProgramStepIndex < 3)
                {
                    ui->programPanelWidget->EnablePauseButton(true);
                }
            }
            else
            {
                ui->programPanelWidget->EnablePauseButton(true);
            }
        }

        if (m_CurProgramStepIndex > 0 && MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator) // operator can't abort program when beginning the second step.
            ui->programPanelWidget->EnableStartButton(false);
        else
            ui->programPanelWidget->EnableStartButton(true);//enable stop button
    }
    m_ProgramStatus = ProgramRunning;
    Core::CGlobalHelper::SetProgramPaused(false);
    ui->programPanelWidget->IsResumeRun(true);
    m_StartDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
}

void CDashboardWidget::OnProgramPaused()
{
    ui->programPanelWidget->EnableStartButton(true);//enable Abort button
    ui->programPanelWidget->ChangeStartButtonToStartState();
    qDebug()<< "OnProgramPaused---ChangeStartButtonToStartState";
    m_ProgramStatus = Paused;
    Core::CGlobalHelper::SetProgramPaused(true);
}

void CDashboardWidget::OnPauseButtonEnable(bool bEnable)
{
    if (bEnable)
    {
        m_ProgramStageStatus = Enabled;
        if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
        {
            if (m_CurProgramStepIndex < 3)
            {
                ui->programPanelWidget->EnablePauseButton(true);
            }
        }
        else
        {
            ui->programPanelWidget->EnablePauseButton(true);
        }
    }
    else
    {
        m_ProgramStageStatus = Disabled;
        if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
        {
            if (m_CurProgramStepIndex < 3)
            {
                ui->programPanelWidget->EnablePauseButton(false);
            }
        }
        else
        {
            ui->programPanelWidget->EnablePauseButton(false);
        }
    }

    //ui->programPanelWidget->ResumePauseRunningStatus(bEnable);
}

void CDashboardWidget::OnStartButtonEnable(bool bEnable)
{
    ui->programPanelWidget->EnableStartButton(bEnable);
}

void CDashboardWidget::OnPauseTimeout15Mintues()
{
    ui->programPanelWidget->ChangeStartButtonToStopState();
}

void CDashboardWidget::OnUnselectProgram()
{
    if (!m_SelectedProgramId.isEmpty())
    {
        m_SelectedProgramId = "";
        ui->programPanelWidget->EnableStartButton(false);
        m_StationList.clear();
        int asapEndTime = 0;

        emit ProgramSelected(m_SelectedProgramId, m_StationList);
        emit ProgramSelected(m_SelectedProgramId, asapEndTime, m_bIsFirstStepFixation, m_StationList, 0);
        emit UpdateSelectedStationList(m_StationList);
        emit UndoProgramSelection();
    }
}

bool CDashboardWidget::IsParaffinInProgram(const DataManager::CProgram* p_Program)
{
    m_ParaffinStepIndex = -1;
    if (!mp_DataConnector)
    {
        Q_ASSERT(0);
        return false;
    }

    QStringList ReagentIDList = p_Program->GetReagentIDList();
    for (int i = 0; i < ReagentIDList.size(); ++i)
    {
        QString ReagentGroupID = mp_DataConnector->ReagentList->GetReagent(ReagentIDList.at(i))->GetGroupID();
        DataManager::CReagentGroup* pReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(ReagentGroupID);
        if(pReagentGroup->IsParraffin()){
            m_ParaffinStepIndex = i;
            return true;
        }
    }
    return false;
}

int CDashboardWidget::GetParaffinHeatingDuration()
{
    if (!mp_DataConnector)
        return 0;
    DataManager::CHimalayaUserSettings* userSetting = mp_DataConnector->SettingsInterface->GetUserSettings();
    int paraffinMeltingPoint = userSetting->GetTemperatureParaffinBath();
    if (paraffinMeltingPoint <= 64)
    {
        return 12;
    } else
    {
        return 15;
    }
}

//TimeActual--- get from Scheduler
//TimeDelta
//EndTimeAsap = TimeActual + TimeDelta;
int CDashboardWidget::GetASAPTime(int TimeActual,//TimeActual is seconds
                                  int TimeBeforeUseParaffin,
                                  int TimeCostedParaffinMelting,
                                  int& TimeDelta,
                                  bool& bCanNotRun)
{
    //IsParaffinInProgram()
    //Yes
      //calculate the timeBeforeUseParraffin
      //RemainingTimeMeltParraffin = 12 hour - TimeCosted
      //if RemainingTimeMeltParraffin > 0
        //if RemainingTimeMeltParraffin <= timeBeforeUseParraffin , TDelta = 0;
            //else TDelta = RemainingTimeMeltParraffin - timeBeforeUseParraffin
    //if RemainingTimeMeltParraffin <= 0 Paraffin is Melted, TDelta = 0;

    //No Paraffin TimeDelta = 0;
    bCanNotRun = false;
    if (-1 == m_ParaffinStepIndex)//No Paraffin in all program steps
    {
        TimeDelta = 0;
    }
    else
    {
        //calculate the timeBeforeUseParraffin
        //RemainingTimeMeltParraffin = 12 or 15 hour - TimeCosted
        int RemainingTimeMeltParaffin = m_ParaffinHeatingDuration - TimeCostedParaffinMelting;
        if (RemainingTimeMeltParaffin > 0)
        {
          if (RemainingTimeMeltParaffin <= TimeBeforeUseParaffin)
              TimeDelta = 0;
           else
           {
              TimeDelta = RemainingTimeMeltParaffin - TimeBeforeUseParaffin;
              bCanNotRun = true;
           }
        }
        else
            TimeDelta = 0;  //Paraffin is Melted
    }

    return (TimeActual + TimeDelta);//seconds
}

bool CDashboardWidget::IsOKPreConditionsToRunProgram(QString& reagentExpiredFlag)
{

    if ("" == m_SelectedProgramId)
        return false;

    if (m_SelectedProgramId.at(0) != 'C')
    {
        MainMenu::CMessageDlg msgDlg(this);
        msgDlg.SetIcon(QMessageBox::Warning);
        msgDlg.SetTitle(CommonString::strWarning);
        msgDlg.SetText(m_checkMachinePrompt);
        msgDlg.SetTextFormat(Qt::PlainText);
        msgDlg.SetButtonText(1, CommonString::strOK);
        msgDlg.HideButtons();
        (void)msgDlg.exec();
        msgDlg.SetTextFormat(Qt::RichText);
    }

    MainMenu::CMessageDlg messageDlg(this);
    if (m_bWaitRotaryValveHeatingPrompt)
    {
        messageDlg.SetIcon(QMessageBox::Warning);
        messageDlg.SetTitle(CommonString::strWarning);
        messageDlg.SetText(m_strWaitRotaryValveHeatingPrompt);
        messageDlg.SetButtonText(1, CommonString::strOK);
        messageDlg.HideButtons();
        if (messageDlg.exec())
        return false;
    }

    if (!m_bRetortLocked){
        messageDlg.SetIcon(QMessageBox::Warning);
        messageDlg.SetTitle(CommonString::strWarning);
        messageDlg.SetText(m_strRetortNotLock);
        messageDlg.SetButtonText(1, CommonString::strOK);
        messageDlg.HideButtons();
        if (messageDlg.exec())
        return false;
    }

    const DataManager::CProgram* pSelectedProgram = mp_ProgramList->GetProgram(m_SelectedProgramId);
    Q_ASSERT(pSelectedProgram);
    //Check if Leica program and RMS OFF?
    DataManager::CHimalayaUserSettings* userSetting = mp_DataConnector->SettingsInterface->GetUserSettings();
    bool bShowRMSOffWarning = false;
    bool isLeicaProgram = pSelectedProgram->IsLeicaProgram();
    if (m_SelectedProgramId.at(0) != 'C')
    {
        if ((Global::RMS_OFF == userSetting->GetModeRMSProcessing()) && isLeicaProgram)
        {
            bShowRMSOffWarning = true;
        }
    }

    if (bShowRMSOffWarning)
    {
        messageDlg.SetIcon(QMessageBox::Warning);
        messageDlg.SetTitle(CommonString::strWarning);
        messageDlg.SetText(m_strNotStartRMSOFF);
        messageDlg.SetButtonText(1, CommonString::strOK);
        messageDlg.HideButtons();
        if (messageDlg.exec())
        return false;
    }

    DataManager::ReagentStatusType_t reagentStatus = DataManager::REAGENT_STATUS_NORMAL;

    for(int i = 0; i<m_StationList.size(); i++)
    {
        QString stationID = m_StationList.at(i);
        DataManager::CDashboardStation *p_Station = mp_DataConnector->DashboardStationList->GetDashboardStation(stationID);
        QString reagentID = p_Station->GetDashboardReagentID();
        DataManager::CReagent const *p_Reagent = mp_DataConnector->ReagentList->GetReagent(reagentID);

        DataManager::CReagentGroup const *p_ReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID());
        bool isClearingGroup = p_ReagentGroup->IsCleaningReagentGroup();
        Global::RMSOptions_t RMSOption = Global::RMS_OFF;
        if ( isClearingGroup )
            RMSOption = userSetting->GetModeRMSCleaning();
        else
            RMSOption = userSetting->GetModeRMSProcessing();

        reagentStatus = p_Station->GetReagentStatus(*p_Reagent, RMSOption);

        if ( reagentStatus == DataManager::REAGENT_STATUS_EXPIRED )
        {
            reagentExpiredFlag = p_Reagent->GetReagentID() + ":" + stationID;
            break;
        }
    }

    if ( reagentStatus == DataManager::REAGENT_STATUS_EXPIRED ) {
        MainMenu::CMainWindow::UserRole_t userRole = MainMenu::CMainWindow::GetCurrentUserRole();
        if (userRole == MainMenu::CMainWindow::Operator)
        {
            messageDlg.SetIcon(QMessageBox::Warning);
            messageDlg.SetTitle(CommonString::strWarning);
            messageDlg.SetText(m_strNotStartExpiredReagent);
            messageDlg.SetButtonText(1, CommonString::strOK);
            messageDlg.HideButtons();
            if (messageDlg.exec())
                return false;
        }
        else if(userRole == MainMenu::CMainWindow::Admin ||
            userRole == MainMenu::CMainWindow::Service)
        {
            messageDlg.SetIcon(QMessageBox::Warning);
            messageDlg.SetTitle(CommonString::strWarning);
            messageDlg.SetText(m_strStartExpiredReagent);
            messageDlg.SetButtonText(3, CommonString::strNo);
            messageDlg.SetButtonText(1, CommonString::strYes);
            messageDlg.HideCenterButton();    // Hiding First Two Buttons in the Message Dialog

            if (!messageDlg.exec())
                return false;
        }
    }

    //Check the diff between paraffin melting point and the temprature of paraffin in program step
    for (int stepIndex = 0; stepIndex < pSelectedProgram->GetNumberOfSteps(); stepIndex++) {
        const DataManager::CProgramStep *step = pSelectedProgram->GetProgramStep(stepIndex);
        if (step) {
            DataManager::CReagent const *p_Reagent = mp_DataConnector->ReagentList->GetReagent(step->GetReagentID());
            DataManager::CReagentGroup const *p_ReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID());
            if (p_ReagentGroup->IsParraffin())
            {
                 int paraffinBathTemp = userSetting->GetTemperatureParaffinBath();
                 int stepTemp = step->GetTemperature().toInt();
                 double diffSetting = qAbs(stepTemp - paraffinBathTemp);

                 if (diffSetting > 2.0)
                 {
                     QString strTemp;
                     if (userSetting->GetTemperatureFormat() == Global::TEMP_FORMAT_FAHRENHEIT) {
                        strTemp = m_strDiffTemp.arg("35.6\260F");
                     }
                     else {
                        strTemp = m_strDiffTemp.arg("2\260C");
                     }

                    messageDlg.SetIcon(QMessageBox::Warning);
                    messageDlg.SetTitle(CommonString::strInforMsg);
                    messageDlg.SetText(strTemp);
                    messageDlg.SetButtonText(1, CommonString::strOK);
                    messageDlg.HideButtons();
                    if (messageDlg.exec())
                        return false;
                 }
            }
        }
    }

    //Check safe reagent
    if ((m_SelectedProgramId.at(0) != 'C') && (m_iWhichStepHasNoSafeReagent  != -1))
    {
        messageDlg.SetIcon(QMessageBox::Warning);
        messageDlg.SetTitle(CommonString::strWarning);
        QString strTemp = m_strCheckSafeReagent.arg(QString::number(m_iWhichStepHasNoSafeReagent +1)).arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
        messageDlg.SetText(strTemp);
        messageDlg.SetButtonText(1, CommonString::strYes);
        messageDlg.SetButtonText(3, CommonString::strNo);
        messageDlg.HideCenterButton();
        if (!messageDlg.exec())
        {
            return false;
        }
    }

    bool bCanNotRun = true;
    int timeDelta = 0;
    if (m_SelectedProgramId.at(0) != 'C')
    {
        int paraffinMeltCostedTime = Global::AdjustedTime::Instance().GetCurrentDateTime().secsTo(m_ParaffinStartHeatingTime);
        (void)GetASAPTime(m_TimeProposedForProgram,
                                      m_CostedTimeBeforeParaffin, -paraffinMeltCostedTime, timeDelta, bCanNotRun);
    }
    else
    {
        bCanNotRun = false;
    }

    if (!m_bIsFirstStepFixation && bCanNotRun)
    {
        messageDlg.SetIcon(QMessageBox::Warning);
        messageDlg.SetTitle(CommonString::strWarning);
        QTime time(0,0,0);
        time = time.addSecs(timeDelta);
        QString waitTime = m_strWaitTimeforParaffinMelt.arg(QString::number(time.hour())).arg(QString::number(time.minute())).arg(QString::number(time.second()));
        messageDlg.SetText(m_strCannotStartParaffinMelt + '\n' + waitTime);
        messageDlg.SetButtonText(1, CommonString::strOK);
        messageDlg.HideButtons();
        if (messageDlg.exec())
        {
            return false;
        }
    }
    else if (m_bIsFirstStepFixation && bCanNotRun)
    {
        messageDlg.SetIcon(QMessageBox::Information);
        messageDlg.SetTitle(CommonString::strConfirmMsg);
        messageDlg.SetText(m_strPromptProgramDelay);
        messageDlg.SetButtonText(1, CommonString::strYes);
        messageDlg.SetButtonText(3, CommonString::strNo);
        messageDlg.HideCenterButton();
        if (!messageDlg.exec())
        {
            return false;
        }
    }

    if ( mp_DataConnector)
    {
        //input cassette number
        if (m_SelectedProgramId.at(0) != 'C' && Global::RMS_CASSETTES == mp_DataConnector->SettingsInterface->GetUserSettings()->GetModeRMSProcessing())
        {
            CCassetteNumberInputWidget *pCassetteInput = new Dashboard::CCassetteNumberInputWidget(this, mp_MainWindow);

            pCassetteInput->ShowLabel(false);
            pCassetteInput->SetDialogTitle(m_strInputCassetteBoxTitle);
            (void)pCassetteInput->exec();

            int cassetteNumber = pCassetteInput->CassetteNumber();
            if (-1 == cassetteNumber)
                return false;//clicked Cancel button

            m_TotalCassette = cassetteNumber;
            mp_DataConnector->SendKeepCassetteCount(cassetteNumber);
            delete pCassetteInput;
        }
    }

    //show Datetime dialog
    if (m_SelectedProgramId.at(0) != 'C')
    {
        mp_wdgtDateTime->UpdateProgramName();
        if (mp_wdgtDateTime->exec() == (int)QDialog::Rejected)
        {
            return false;
        }
    }
    else
    {
        m_AsapEndDateTime = m_EndDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(m_TimeProposedForProgram);
    }
    return true;
}

void CDashboardWidget::CheckPreConditionsToRunProgram()
{
    if (ui->programPanelWidget->IsResumeRun())
    {
        if (!m_bRetortLocked){
            MainMenu::CMessageDlg messageDlg(this);
            messageDlg.SetIcon(QMessageBox::Warning);
            messageDlg.SetTitle(CommonString::strWarning);
            messageDlg.SetText(m_strRetortNotLock);
            messageDlg.SetButtonText(1, CommonString::strOK);
            messageDlg.HideButtons();
            if (messageDlg.exec())
            {
                ui->programPanelWidget->EnableStartButton(true);
                return;
            }
        }


        mp_DataConnector->SendProgramAction("", DataManager::PROGRAM_START, 0, 0);
        ui->programPanelWidget->ChangeStartButtonToStopState();
        return;
    }

    QString reagentExpiredFlag;
    if (IsOKPreConditionsToRunProgram(reagentExpiredFlag))
    {
        int EndTimeDelta = m_AsapEndDateTime.secsTo(m_EndDateTime);
        int delayTime = EndTimeDelta + m_TimeDelta;
        int runDuration = Global::AdjustedTime::Instance().GetCurrentDateTime().secsTo(m_EndDateTime);
        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_START, delayTime, runDuration, reagentExpiredFlag);
        ui->programPanelWidget->ChangeStartButtonToStopState();
    }
    else
    {
        if (!Core::CGlobalHelper::GetSystemErrorStatus())
        {
            ui->programPanelWidget->EnableStartButton(true, true);
        }
        ui->programPanelWidget->ResetInFavProgramButtonClicked();
    }
}

void CDashboardWidget::PrepareSelectedProgramChecking(const QString& selectedProgramId)
{
    m_NewSelectedProgramId = selectedProgramId;
    (void)this->IsParaffinInProgram(mp_ProgramList->GetProgram(selectedProgramId));//to get m_ParaffinStepIndex
    //Notify Master, to get the time costed for paraffin Melting
    mp_DataConnector->SendProgramSelected(selectedProgramId, m_ParaffinStepIndex);
}

void CDashboardWidget::OnRecoveryFromPowerFailure(const MsgClasses::CmdRecoveryFromPowerFailure& cmd)
{
    ui->programPanelWidget->SwitchToProgramRunningStatus(cmd);
    m_SelectedProgramId = cmd.GetProgramID();
    if (!m_SelectedProgramId.isEmpty())
    {
        CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME = mp_ProgramList->GetProgram(m_SelectedProgramId)->GetName();
    }
    emit ProgramSelected(m_SelectedProgramId, const_cast<QList<QString>&>(cmd.GetStationList()));
    QString scenarioID = QString::number(cmd.GetScenario());
    if ('4' == scenarioID.at(scenarioID.count() - 1))
    {
        ui->containerPanelWidget->UpdateRetortStatus(DataManager::CONTAINER_STATUS_FULL, cmd.GetLastReagentGroupID(), cmd.GetStationID());
    }
}

void CDashboardWidget::OnProgramSelectedReply(const MsgClasses::CmdProgramSelectedReply& cmd)
{
    ui->programPanelWidget->ResetInFavProgramButtonClicked();
    MainMenu::CMessageDlg messageDlg(this);
    const QList<QString>& stationList = cmd.StationList();
    if (stationList.count() == 0)
    {
        if (m_NewSelectedProgramId.at(0) == 'C')
        {
            messageDlg.SetIcon(QMessageBox::Warning);
            messageDlg.SetTitle(CommonString::strWarning);
            messageDlg.SetText("CleaningProgram does not need to be run as the reagent compatibility");
            messageDlg.SetButtonText(1, CommonString::strOK);
            messageDlg.HideButtons();
            if (messageDlg.exec())
            {
                emit AddItemsToFavoritePanel();
                ui->programPanelWidget->ChangeStartButtonToStartState();
                return;
            }
        }
    }

    //get the proposed program end DateTime
    m_TimeProposedForProgram = cmd.TimeProposed();
    m_ParaffinStartHeatingTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(-cmd.ParaffinMeltCostedTime());
    m_CostedTimeBeforeParaffin = cmd.CostedTimeBeforeParaffin();
    m_iWhichStepHasNoSafeReagent = cmd.WhichStepHasNoSafeReagent();
    m_ParaffinHeatingDuration = cmd.GetSecondsForMeltingParaffin();
    bool bCanotRun = true;
    m_TimeDelta = 0;
    int asapEndTime = GetASAPTime(cmd.TimeProposed(),
                                  cmd.CostedTimeBeforeParaffin(), cmd.ParaffinMeltCostedTime(), m_TimeDelta, bCanotRun);

    //firstly check whether there is any station not existing for some program steps
    for (int i = 0; i < stationList.count(); i++)
    {
        if ("" == stationList.at(i))
        {
            messageDlg.SetIcon(QMessageBox::Warning);
            messageDlg.SetTitle(CommonString::strWarning);
            QString strTemp = m_strNotFoundStation.arg(QString::number(i+1)).arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
            messageDlg.SetText(strTemp);
            messageDlg.SetButtonText(1, CommonString::strOK);
            messageDlg.HideButtons();

            ui->programPanelWidget->EnableStartButton(false);

            if (messageDlg.exec())
            {
                emit ResetFocus(true);
                return;
            }
            return;
        }
        else //then check station's status is empty?
        {
            DataManager::CDashboardStation* pStation = mp_DataConnector->DashboardStationList->GetDashboardStation(stationList.at(i));
            if (!pStation)
            {
                Q_ASSERT(0);
            }

            if ("Empty" == pStation->GetDashboardReagentStatus())
            {
                messageDlg.SetIcon(QMessageBox::Warning);
                messageDlg.SetTitle(CommonString::strWarning);
                QString strTemp = m_strCheckEmptyStation.arg(pStation->GetDashboardStationName()).arg(QString::number(i+1)).arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
                messageDlg.SetText(strTemp);
                messageDlg.SetButtonText(1, CommonString::strOK);
                messageDlg.HideButtons();

                if (messageDlg.exec())
                {
                    emit ResetFocus(true);
                    return;
                }
                return;
            }
        }
    }

    emit ResetFocus(false);

    m_SelectedProgramId = m_NewSelectedProgramId;
    m_StationList.clear();
    m_StationList = stationList;

    m_AsapEndDateTime = m_EndDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(asapEndTime);

    m_bIsFirstStepFixation = IsFixationInFirstStep();
    emit ProgramSelected(m_SelectedProgramId, asapEndTime, m_bIsFirstStepFixation, m_StationList, cmd.GetFirstProgramStepIndex());
    emit ProgramSelected(m_SelectedProgramId, m_StationList);
    emit SendAsapDateTime(asapEndTime, m_bIsFirstStepFixation, false);
    emit UpdateSelectedStationList(m_StationList);
    m_ProgramStageStatus = Undefined;
}

bool CDashboardWidget::IsFixationInFirstStep()
{
    bool bIsFixationInFirstStep = true;
    QString stationID = m_StationList.at(0);
    DataManager::CDashboardStation *p_Station = mp_DataConnector->DashboardStationList->GetDashboardStation(stationID);
    QString reagentID = p_Station->GetDashboardReagentID();
    DataManager::CReagent const *p_Reagent = mp_DataConnector->ReagentList->GetReagent(reagentID);

    DataManager::CReagentGroup const *p_ReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID());
    if ("RG1" != p_ReagentGroup->GetGroupID())
    {
        bIsFixationInFirstStep = false;
    }
    return bIsFixationInFirstStep;
}

void CDashboardWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            CommonString::RetranslateUIString();
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

void CDashboardWidget::RetranslateUI()
{
    m_strCheckSafeReagent = QApplication::translate("Dashboard::CDashboardWidget", "No safe reagent for the Program step \"%1\" of \"%2\". Would you like to continue?", Q_NULLPTR, -1);
    m_strNotFoundStation = QApplication::translate("Dashboard::CDashboardWidget", "Program step \"%1\" of \"%2\" can not find the corresponding reagent station.  A station can be used only once in the program. Please assign a separate station for the reagent of this step.", Q_NULLPTR, -1);
    m_strCheckEmptyStation = QApplication::translate("Dashboard::CDashboardWidget", "You cannot start the program, because Station \"%1\" is set as Empty in Program step \"%2\" of \"%3\".", Q_NULLPTR, -1);
    m_strCannotStartParaffinMelt = QApplication::translate("Dashboard::CDashboardWidget", "Program cannot start because the paraffin is not melted completely and the first program step is not a fixation reagent.", Q_NULLPTR, -1);
    m_strWaitTimeforParaffinMelt = QApplication::translate("Dashboard::CDashboardWidget", "You can start program in %1 hours %2 minutes %3 seconds.", Q_NULLPTR, -1);
    m_strPromptProgramDelay =  QApplication::translate("Dashboard::CDashboardWidget", "Program will be delayed briefly in the first step as the paraffin is not melted completely. Would you like to continue?", Q_NULLPTR, -1);
    m_strInputCassetteBoxTitle = QApplication::translate("Dashboard::CDashboardWidget", "Enter number of Cassettes:", Q_NULLPTR, -1);
    m_strProgramWillComplete = QApplication::translate("Dashboard::CDashboardWidget", "Program \"%1\" has completed! Would you like to drain the retort?", Q_NULLPTR, -1);
    m_strTissueProtectPassed = QApplication::translate("Dashboard::CDashboardWidget", "The specimens have been safeguarded by filling the retort with a safe reagent. Would you like to drain the safe reagent?", Q_NULLPTR, -1);
    m_strTissueProtectPassed_Warning = QApplication::translate("Dashboard::CDashboardWidget", "Tissue safety process has completed with warning. Would you like to drain the retort?", Q_NULLPTR, -1);
    m_strOvenCoverOpen = QApplication::translate("Dashboard::CDashboardWidget", "The oven cover is open. Please close it then click the OK button.", Q_NULLPTR, -1);
    m_strRetortCoverOpen = QApplication::translate("Dashboard::CDashboardWidget", "Retort lid was opened. Please close it and then click OK. \
Wear respirator properly to prevent the potential hazard of reagent vapor. Don't touch the level sensor. It's hot!", Q_NULLPTR, -1);
    m_strItIsPausing = QApplication::translate("Dashboard::CDashboardWidget", "Pausing...\nDon't open the retort lid and unplug any reagent bottles.", Q_NULLPTR, -1);
    m_strWaitRotaryValveHeatingPrompt = QApplication::translate("Dashboard::CDashboardWidget", "Instrument is pre-heating. Wait time may be up to 30 minutes before the instrument is ready to use.", Q_NULLPTR, -1);
    m_strTakeOutSpecimen = QApplication::translate("Dashboard::CDashboardWidget", "Remove specimen and lock the retort. And confirm specimen are removed and retort is empty, then press \"Ok\" button.", Q_NULLPTR, -1);
    m_strRetortContaminated  = QApplication::translate("Dashboard::CDashboardWidget", "The retort is contaminated. Please start the cleaning program.", Q_NULLPTR, -1);
    m_strProgramIsAborted  = QApplication::translate("Dashboard::CDashboardWidget", "Program \"%1\" is aborted!", Q_NULLPTR, -1);
    m_strProgramComplete  = QApplication::translate("Dashboard::CDashboardWidget", "Program \"%1\" has completed!", Q_NULLPTR, -1);
    m_strProgramCompleteSuccessfully  = QApplication::translate("Dashboard::CDashboardWidget", "Program \"%1\" has completed successfully!", Q_NULLPTR, -1);
    m_strProgramCompleteSafeReagent  = QApplication::translate("Dashboard::CDashboardWidget", "The safe reagent has been drained completely. The program \"%1\" was aborted!", Q_NULLPTR, -1);

    m_strRetortNotLock = QApplication::translate("Dashboard::CDashboardWidget", "Please close and lock the retort, then try again!", Q_NULLPTR, -1);
    m_strNotStartRMSOFF = QApplication::translate("Dashboard::CDashboardWidget", "Leica Program can't be operated with RMS OFF.", Q_NULLPTR, -1);
    m_strNotStartExpiredReagent = QApplication::translate("Dashboard::CDashboardWidget", "Reagents needed for this program are expired. Please replace the expired ones with fresh reagents! Program will not start.", Q_NULLPTR, -1);
    m_strStartExpiredReagent =  QApplication::translate("Dashboard::CDashboardWidget", "Do you really want to start the Program with Expired Reagents?", Q_NULLPTR, -1);
    m_strChangeCassetteBoxTitle = QApplication::translate("Dashboard::CDashboardWidget", "Please enter the number of new-added cassettes:", Q_NULLPTR, -1);
    m_strAddCassete = QApplication::translate("Dashboard::CDashboardWidget", "Did you add new cassettes?", Q_NULLPTR, -1);
    m_PowerFailureBoxTitle = QApplication::translate("Dashboard::CDashboardWidget", "Because the wait time is more than five minutes, the instrument will be re-heated and filled with safe reagents.", Q_NULLPTR, -1);
    m_strCoolingDown = QApplication::translate("Dashboard::CDashboardWidget", "Retort temperature is too high. Please allow the retort to cool down before inserting specimens and starting the next program.", Q_NULLPTR, -1);

    m_strDiffTemp = QApplication::translate("Dashboard::CDashboardWidget", "Starting the chosen program is impossible, because its Paraffin temperature is differing from set Paraffin bath temperature (deviation %1).\
Program can be started immediately, if Paraffin temperature in this program is adapted to match the Paraffin bath temperature.\
Program is impossible to start for up to 15 hours if Paraffin bath temperature is increased to equal with Paraffin program temperature!\
Please confirm modifying Paraffin temperature.", Q_NULLPTR, -1);

    m_strReadyStartProgram = QApplication::translate("Dashboard::CDashboardWidget", "Ready to start a new program.", Q_NULLPTR, -1);
    m_checkMachinePrompt = QApplication::translate("Dashboard::CDashboardWidget", "Please check the level of all reagents and clean the retort level sensor, the sealing ring of the retort and the retort filter.\
 Reinstall the retort filter after cleaning.\n Load the specimens into the retort and make sure that the retort is locked.\n Please visually confirm all paraffin are completely melted.", Q_NULLPTR, -1);
    m_strItIsResuming = QApplication::translate("Dashboard::CDashboardWidget", "Resuming...\nDon't open the retort lid and unplug any reagent bottles.", Q_NULLPTR, -1);

}

void CDashboardWidget::OnSelectEndDateTime(const QDateTime& dateTime)
{
    m_EndDateTime = dateTime;
}

void CDashboardWidget::RequestAsapDateTime()
{
    bool bCanotRun = true;
    int paraffinMeltCostedTime = Global::AdjustedTime::Instance().GetCurrentDateTime().secsTo(m_ParaffinStartHeatingTime);
    m_TimeDelta = 0;
    int asapEndTime = GetASAPTime(m_TimeProposedForProgram,
                                  m_CostedTimeBeforeParaffin, -paraffinMeltCostedTime, m_TimeDelta, bCanotRun);

    m_AsapEndDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(asapEndTime);

    emit SendAsapDateTime(asapEndTime, m_bIsFirstStepFixation, true);
}

void CDashboardWidget::OnStationSuckDrain(const MsgClasses::CmdStationSuckDrain & cmd)
{
   

    if (cmd.IsSuck() && cmd.IsStart())
    {
        m_HaveSucked = true;
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CDashboardWidget::OnUserRoleChanged()
{
    if (!m_SelectedProgramId.isEmpty() && m_SelectedProgramId.at(0) == 'C')
    return;

    if (m_IsDrainingWhenPrgrmCompleted) {
        ui->programPanelWidget->EnablePauseButton(false);
        ui->programPanelWidget->EnableStartButton(false);
        return;
    }

    if (Core::CGlobalHelper::GetSystemErrorStatus())
        return;

    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
    {
        if (m_ProgramStatus == ProgramRunning || m_ProgramStatus == Paused)
        {
            if (m_ProgramStatus == ProgramRunning)
            {
                if (m_CurProgramStepIndex < 3)
                {
                    if (m_ProgramStageStatus == Enabled)
                    {
                        ui->programPanelWidget->EnablePauseButton(true);
                    }
                 }
                else// >3rd step
                {
                    ui->programPanelWidget->EnablePauseButton(false);
                }
            }

            //Abort
            if (m_CurProgramStepIndex == 0 || m_ProgramStatus == Paused)
            {
                ui->programPanelWidget->EnableStartButton(true);
            }
            else
            {
                ui->programPanelWidget->EnableStartButton(false);
            }
        }
    }
    else //Supervior or service
    {
        if (m_ProgramStatus == ProgramRunning || m_ProgramStatus == Paused)
        {
            if (m_ProgramStatus == ProgramRunning)
            {
                ui->programPanelWidget->EnablePauseButton(m_ProgramStageStatus == Enabled);
            }
            ui->programPanelWidget->EnableStartButton(true);
        }
    }


}

void CDashboardWidget::OnProcessStateChanged()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
}

bool CDashboardWidget::IsAbortEnabled()
{
    return ui->programPanelWidget->IsAbortEnabled();
}



