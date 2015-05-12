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
    m_TimeProposedForProgram(0),
    m_CostedTimeBeforeParaffin(0),
    m_ParaffinHeatingDuration(0),
    m_ProgramStartReady(false),
    m_IsWaitingCleaningProgram(false),
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
    m_bWaitRotaryValveHeatingPrompt(false)

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

    CONNECTSIGNALSLOT(mp_wdgtDateTime, OnSelectDateTime(const QDateTime &), this, OnSelectEndDateTime(const QDateTime &));
    CONNECTSIGNALSIGNAL(mp_wdgtDateTime, OnSelectDateTime(const QDateTime &), ui->programPanelWidget, OnSelectEndDateTime(const QDateTime &));

    CONNECTSIGNALSLOT(mp_wdgtDateTime, RequestAsapDateTime(), this, RequestAsapDateTime());
    CONNECTSIGNALSLOT(this, SendAsapDateTime(int, bool), mp_wdgtDateTime, OnGetASAPDateTime(int, bool));

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
                      ui->programPanelWidget, OnProgramStartReadyUpdated());

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramStartReady(),
                      this, OnProgramStartReadyUpdated());

    CONNECTSIGNALSLOT(mp_DataConnector, RetortLockStatusChanged(const MsgClasses::CmdLockStatus &),
                      this, OnRetortLockStatusChanged(const MsgClasses::CmdLockStatus&));


    CONNECTSIGNALSIGNAL(this, ProgramSelected(QString&, QList<QString>&),
                       ui->containerPanelWidget, ProgramSelected(QString&, QList<QString>&));
    mp_MessageDlg = new MainMenu::CMessageDlg(this);

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

    CONNECTSIGNALSLOT(mp_DataConnector, TissueProtectPassed(),
                      this, OnTissueProtectPassed());

    CONNECTSIGNALSLOT(mp_DataConnector, OvenCoverOpen(),
                      this, OnOvenCoverOpen());

    CONNECTSIGNALSLOT(mp_DataConnector, RetortCoverOpen(),
                      this, OnRetortCoverOpen());

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramAborted(bool),
                      this, OnProgramAborted(bool));

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramBeginAbort(),
                              this, OnProgramBeginAbort());

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramCompleted(bool, bool),
                              this, OnProgramCompleted(bool, bool));
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

    CONNECTSIGNALSLOT(mp_DataConnector, ShowPauseMsgDialog(),
                      this, OnShowPauseMsgDialog());

    CONNECTSIGNALSLOT(mp_DataConnector, DismissPauseMsgDialog(),
                      this, OnDismissPauseMsgDialog());

    CONNECTSIGNALSLOT(mp_DataConnector, WaitRotaryValveHeatingPrompt(),
                      this, OnWaitRotaryValveHeatingPrompt());

    CONNECTSIGNALSLOT(mp_DataConnector, WaitRotaryValveHeatingPrompt(),
                      ui->programPanelWidget, OnWaitRotaryValveHeatingPrompt());

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
}

CDashboardWidget::~CDashboardWidget()
{
    try {
            delete ui;
            delete mp_MessageDlg;
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
        //enable the "OK"
        if (m_IsWaitingCleaningProgram && mp_MessageDlg->isVisible())
        {
            mp_MessageDlg->EnableButton(1, true);
        }

        if ((m_CurProgramStepIndex < 3) && (m_ProgramStatus == Paused) && (m_pUserSetting->GetModeRMSProcessing() == Global::RMS_CASSETTES) && (m_SelectedProgramId.at(0) != 'C'))
        {
            mp_MessageDlg->SetIcon(QMessageBox::Information);
            mp_MessageDlg->SetTitle(CommonString::strInforMsg);
            mp_MessageDlg->SetText(m_strAddCassete);
            mp_MessageDlg->SetButtonText(1, CommonString::strYes);
            mp_MessageDlg->SetButtonText(3, CommonString::strNo);
            mp_MessageDlg->HideCenterButton();
            if (mp_MessageDlg->exec())
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
}

void CDashboardWidget::CancelProgramWillCompletePrompt()
{
    mp_MessageDlg->reject();
}

void CDashboardWidget::OnProgramWillComplete()
{
    mp_MessageDlg->SetIcon(QMessageBox::Information);
    mp_MessageDlg->SetTitle(CommonString::strConfirmMsg);
    QString strTemp(m_strProgramWillComplete);
    strTemp = strTemp.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
    mp_MessageDlg->SetText(strTemp);
    mp_MessageDlg->SetButtonText(1, CommonString::strOK);
    mp_MessageDlg->HideButtons();

    emit ProgramActionStopped(DataManager::PROGRAM_STATUS_PAUSED);//pause ProgressBar and EndTime countdown

    if (mp_MessageDlg->exec())
    {
        m_IsDrainingWhenPrgrmCompleted = true;

        //Resume EndTime countdown
        emit ProgramActionStarted(DataManager::PROGRAM_START, 0, Global::AdjustedTime::Instance().GetCurrentDateTime(), true);

        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_DRAIN);
        //disable pause and abort
        ui->programPanelWidget->EnableStartButton(false);
        ui->programPanelWidget->EnablePauseButton(false);
        return;
    }
}

void CDashboardWidget::CancelTissueProtectPassedPrompt()
{
    if (mp_TissueProtectPassedMsgDlg)
        mp_TissueProtectPassedMsgDlg->reject();
}

void CDashboardWidget::OnTissueProtectPassed()
{
    mp_TissueProtectPassedMsgDlg = new MainMenu::CMessageDlg(this);
    mp_TissueProtectPassedMsgDlg->SetIcon(QMessageBox::Information);
    mp_TissueProtectPassedMsgDlg->SetTitle(CommonString::strConfirmMsg);
    QString strTemp(m_strTissueProtectPassed);
    mp_TissueProtectPassedMsgDlg->SetText(strTemp);
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
    MainMenu::CMessageDlg* pMessageDlg = new MainMenu::CMessageDlg(this);
    pMessageDlg->SetIcon(QMessageBox::Information);
    pMessageDlg->SetTitle(CommonString::strConfirmMsg);
    pMessageDlg->SetText(m_strOvenCoverOpen);
    pMessageDlg->SetButtonText(1, CommonString::strOK);
    pMessageDlg->HideButtons();
    if (pMessageDlg->exec())
    {
        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_OVEN_COVER_OPEN);
        delete pMessageDlg;
        return;
    }
    delete pMessageDlg;
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
    MainMenu::CMessageDlg* pMessageDlg = new MainMenu::CMessageDlg(this);
    pMessageDlg->SetIcon(QMessageBox::Critical);
    pMessageDlg->SetTitle(CommonString::strConfirmMsg);
    QString strTemp(m_PowerFailureBoxTitle);
    pMessageDlg->SetText(strTemp);
    pMessageDlg->SetButtonText(1, CommonString::strOK);
    pMessageDlg->HideButtons();

    if (pMessageDlg->exec())
    {
        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_POWER_FAILURE_MSG);
        delete pMessageDlg;
        return;
    }
    delete pMessageDlg;
}


void CDashboardWidget::OnShowPauseMsgDialog()
{
    mp_PausingMsgDlg = new MainMenu::CMessageDlg(this);
    mp_PausingMsgDlg->SetIcon(QMessageBox::Information);
    mp_PausingMsgDlg->SetText(m_strItIsPausing);
    mp_PausingMsgDlg->HideAllButtons();
    (void)mp_PausingMsgDlg->exec();
    delete mp_PausingMsgDlg;
    mp_PausingMsgDlg = NULL;
}

void CDashboardWidget::OnDismissPauseMsgDialog()
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
}

void CDashboardWidget::OnDismissRotaryValveHeatingPrompt()
{
    m_bWaitRotaryValveHeatingPrompt = false;
    ui->programPanelWidget->WaitRotaryValveHeatingPrompt(false);
}

void CDashboardWidget::OnCoolingDown()
{
    MainMenu::CMessageDlg* pMessageDlg = new MainMenu::CMessageDlg(this);
    pMessageDlg->SetIcon(QMessageBox::Information);
    pMessageDlg->SetTitle(CommonString::strConfirmMsg);
    pMessageDlg->SetText(m_strCoolingDown);
    pMessageDlg->SetButtonText(1, CommonString::strOK);
    pMessageDlg->HideButtons();
    if (pMessageDlg->exec())
    {
        delete pMessageDlg;
        return;
    }
    delete pMessageDlg;
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
        m_IsWaitingCleaningProgram = true;
        //only show Cleaning program in the favorite panel
        emit AddItemsToFavoritePanel(true);

        ui->programPanelWidget->ChangeStartButtonToStartState();
        ui->programPanelWidget->EnableStartButton(false);
        ui->programPanelWidget->EnablePauseButton(false);
        //show all Stations and pipes
        m_StationList.clear();
        QString programID("");
        emit ProgramSelected(programID, m_StationList);
        //switch to the dashboard page
        mp_MainWindow->SetTabWidgetIndex();
        emit SwitchToFavoritePanel();
     }
     else
     {
             if (m_ProgramStatus == Completed ||
            m_ProgramStatus == Aborted || m_ProgramStatus == CompletedAsSafeReagent)
            {
                mp_MessageDlg->SetIcon(QMessageBox::Information);
                mp_MessageDlg->SetTitle(CommonString::strInforMsg);
                QString strTemp;
                if (m_ProgramStatus == Completed)
                {
                    strTemp = m_strProgramComplete.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
                }
                else if (m_ProgramStatus == Aborted)
                {
                    strTemp = m_strProgramIsAborted.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
                }

                if (m_ProgramStatus != CompletedAsSafeReagent)
                {
                    mp_MessageDlg->SetText(strTemp);
                    mp_MessageDlg->SetButtonText(1, CommonString::strOK);
                    mp_MessageDlg->HideButtons();
                    (void)mp_MessageDlg->exec();
                }
                m_ProgramStatus = Undefined_ProgramStatus;
                Core::CGlobalHelper::SetProgramPaused(false);
            }

            mp_MessageDlg->SetIcon(QMessageBox::Information);
            mp_MessageDlg->SetTitle(CommonString::strConfirmMsg);
            mp_MessageDlg->SetText(m_strTakeOutSpecimen);
            mp_MessageDlg->SetButtonText(1, CommonString::strOK);
            mp_MessageDlg->HideButtons();
            if (mp_MessageDlg->exec())
            {
                mp_DataConnector->SendTakeOutSpecimenFinishedCMD();
                //represent the retort as contaminated status
                ui->containerPanelWidget->UpdateRetortStatus(DataManager::CONTAINER_STATUS_CONTAMINATED, lastReagentGroupID, "");

                mp_MessageDlg->SetText(m_strRetortContaminated);
                mp_MessageDlg->SetButtonText(1, CommonString::strOK);
                mp_MessageDlg->HideButtons();
                //mp_MessageDlg->EnableButton(1, false);//when lock is locked, "OK" will be enable
                mp_MessageDlg->EnableButton(1, true);//6.6 for test

                m_IsWaitingCleaningProgram = true;
                if (mp_MessageDlg->exec())
                {
                    //only show Cleaning program in the favorite panel
                    emit AddItemsToFavoritePanel(true);
                }

                ui->programPanelWidget->ChangeStartButtonToStartState();
                ui->programPanelWidget->EnableStartButton(false);
                ui->programPanelWidget->EnablePauseButton(false);
                //show all Stations and pipes
                m_StationList.clear();
                QString programID("");
                emit ProgramSelected(programID, m_StationList);
                //switch to the dashboard page
                mp_MainWindow->SetTabWidgetIndex();
                emit SwitchToFavoritePanel();
            }
     }
}

void CDashboardWidget::SetCassetteNumber()
{
    if (mp_DataConnector)
    {
        //input cassette number
        CCassetteNumberInputWidget *pCassetteInput = new Dashboard::CCassetteNumberInputWidget(NULL, mp_MainWindow);
        pCassetteInput->setWindowFlags(Qt::CustomizeWindowHint);
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
    m_ProgramStatus = Aborted;
    Core::CGlobalHelper::SetProgramPaused(false);
    if (!IsRetortContaminated)
    {
        mp_MessageDlg->SetIcon(QMessageBox::Information);
        mp_MessageDlg->SetTitle(CommonString::strInforMsg);
        QString strTemp = m_strProgramIsAborted.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
        mp_MessageDlg->SetText(strTemp);
        mp_MessageDlg->SetButtonText(1, CommonString::strOK);
        mp_MessageDlg->HideButtons();
        if (mp_MessageDlg->exec())
        {
            mp_MessageDlg->SetIcon(QMessageBox::Information);
            mp_MessageDlg->SetTitle(CommonString::strConfirmMsg);
            mp_MessageDlg->SetText(m_strTakeOutSpecimen);
            mp_MessageDlg->SetButtonText(1, CommonString::strOK);
            mp_MessageDlg->HideButtons();
            if (mp_MessageDlg->exec())
            {
                mp_DataConnector->SendTakeOutSpecimenFinishedCMD();
                ui->programPanelWidget->ChangeStartButtonToStartState();
                ui->programPanelWidget->EnableStartButton(true);
                ui->programPanelWidget->EnablePauseButton(false);
                //switch to the dashboard page
                mp_MainWindow->SetTabWidgetIndex();
                emit SwitchToFavoritePanel();
                OnUnselectProgram();
            }
        }
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
}

void CDashboardWidget::OnProgramCompleted(bool isDueToSafeReagent, bool IsRetortContaminated)
{
    ui->programPanelWidget->IsResumeRun(false);
    m_CurProgramStepIndex = -1;
    m_IsDrainingWhenPrgrmCompleted = false;
    if ((!m_SelectedProgramId.isEmpty() && m_SelectedProgramId.at(0) == 'C') || !IsRetortContaminated)
    {
        mp_MessageDlg->SetIcon(QMessageBox::Information);
        mp_MessageDlg->SetTitle(CommonString::strInforMsg);
        QString strTemp;
        strTemp = m_strProgramComplete.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
        mp_MessageDlg->SetText(strTemp);
        mp_MessageDlg->SetButtonText(1, CommonString::strOK);
        mp_MessageDlg->HideButtons();
        if (mp_MessageDlg->exec())
        {
            if (!IsRetortContaminated && (m_SelectedProgramId.at(0) != 'C'))
            {
                mp_MessageDlg->SetIcon(QMessageBox::Information);
                mp_MessageDlg->SetTitle(CommonString::strConfirmMsg);
                mp_MessageDlg->SetText(m_strTakeOutSpecimen);
                mp_MessageDlg->SetButtonText(1, CommonString::strOK);
                mp_MessageDlg->HideButtons();
                mp_MessageDlg->exec();
                mp_DataConnector->SendTakeOutSpecimenFinishedCMD();
            }
            ui->programPanelWidget->EnablePauseButton(false);
            emit AddItemsToFavoritePanel();
            ui->programPanelWidget->ChangeStartButtonToStartState();

            ui->programPanelWidget->EnableStartButton(false);

            m_StationList.clear();
            QString programID("");
            emit ProgramSelected(programID, m_StationList);
        }
    }

    emit ProgramActionStopped(DataManager::PROGRAM_STATUS_COMPLETED);
    if (isDueToSafeReagent)
        m_ProgramStatus = CompletedAsSafeReagent;
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
            ui->programPanelWidget->EnablePauseButton(true);//enable pause button
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

bool CDashboardWidget::IsOKPreConditionsToRunProgram()
{

    if ("" == m_SelectedProgramId)
        return false;

    if (m_bWaitRotaryValveHeatingPrompt)
    {
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->SetTitle(CommonString::strWarning);
        mp_MessageDlg->SetText(m_strWaitRotaryValveHeatingPrompt);
        mp_MessageDlg->SetButtonText(1, CommonString::strOK);
        mp_MessageDlg->HideButtons();
        if (mp_MessageDlg->exec())
        return false;
    }

    if (!m_bRetortLocked){
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->SetTitle(CommonString::strWarning);
        mp_MessageDlg->SetText(m_strRetortNotLock);
        mp_MessageDlg->SetButtonText(1, CommonString::strOK);
        mp_MessageDlg->HideButtons();
        if (mp_MessageDlg->exec())
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
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->SetTitle(CommonString::strWarning);
        mp_MessageDlg->SetText(m_strNotStartRMSOFF);
        mp_MessageDlg->SetButtonText(1, CommonString::strOK);
        mp_MessageDlg->HideButtons();
        if (mp_MessageDlg->exec())
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
            break;
    }

    if ( reagentStatus == DataManager::REAGENT_STATUS_EXPIRED ) {
        MainMenu::CMainWindow::UserRole_t userRole = MainMenu::CMainWindow::GetCurrentUserRole();
        if (userRole == MainMenu::CMainWindow::Operator)
        {
            mp_MessageDlg->SetIcon(QMessageBox::Warning);
            mp_MessageDlg->SetTitle(CommonString::strWarning);
            mp_MessageDlg->SetText(m_strNotStartExpiredReagent);
            mp_MessageDlg->SetButtonText(1, CommonString::strOK);
            mp_MessageDlg->HideButtons();
            if (mp_MessageDlg->exec())
                return false;
        }
        else if(userRole == MainMenu::CMainWindow::Admin ||
            userRole == MainMenu::CMainWindow::Service)
        {
            MainMenu::CMessageDlg messageDlg(this);
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
                    mp_MessageDlg->SetIcon(QMessageBox::Warning);
                    mp_MessageDlg->SetTitle(CommonString::strInforMsg);
                    mp_MessageDlg->SetText(m_strDiffTemp);
                    mp_MessageDlg->SetButtonText(1, CommonString::strOK);
                    mp_MessageDlg->HideButtons();
                    if (mp_MessageDlg->exec())
                        return false;
                 }
            }
        }
    }

    //Check safe reagent
    if ((m_SelectedProgramId.at(0) != 'C') && (m_iWhichStepHasNoSafeReagent  != -1))
    {
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->SetTitle(CommonString::strWarning);
        QString strTemp = m_strCheckSafeReagent.arg(QString::number(m_iWhichStepHasNoSafeReagent +1)).arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
        mp_MessageDlg->SetText(strTemp);
        mp_MessageDlg->SetButtonText(1, CommonString::strYes);
        mp_MessageDlg->SetButtonText(3, CommonString::strNo);
        mp_MessageDlg->HideCenterButton();
        if (!mp_MessageDlg->exec())
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
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->SetTitle(CommonString::strWarning);
        QTime time(0,0,0);
        time = time.addSecs(timeDelta);
        QString waitTime = m_strWaitTimeforParaffinMelt.arg(QString::number(time.hour())).arg(QString::number(time.minute())).arg(QString::number(time.second()));
        mp_MessageDlg->SetText(m_strCannotStartParaffinMelt + '\n' + waitTime);
        mp_MessageDlg->SetButtonText(1, CommonString::strOK);
        mp_MessageDlg->HideButtons();
        if (mp_MessageDlg->exec())
        {
            return false;
        }
    }
    else if (m_bIsFirstStepFixation && bCanNotRun)
    {
        mp_MessageDlg->SetIcon(QMessageBox::Information);
        mp_MessageDlg->SetTitle(CommonString::strConfirmMsg);
        mp_MessageDlg->SetText(m_strPromptProgramDelay);
        mp_MessageDlg->SetButtonText(1, CommonString::strYes);
        mp_MessageDlg->SetButtonText(3, CommonString::strNo);
        mp_MessageDlg->HideCenterButton();
        if (!mp_MessageDlg->exec())
        {
            return false;
        }
    }

    if ( mp_DataConnector)
    {
        //input cassette number
        if (m_SelectedProgramId.at(0) != 'C' && Global::RMS_CASSETTES == mp_DataConnector->SettingsInterface->GetUserSettings()->GetModeRMSProcessing())
        {
            CCassetteNumberInputWidget *pCassetteInput = new Dashboard::CCassetteNumberInputWidget(NULL, mp_MainWindow);

            pCassetteInput->ShowLabel(false);
            pCassetteInput->setWindowFlags(Qt::CustomizeWindowHint);
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
            mp_MessageDlg->SetIcon(QMessageBox::Warning);
            mp_MessageDlg->SetTitle(CommonString::strWarning);
            mp_MessageDlg->SetText(m_strRetortNotLock);
            mp_MessageDlg->SetButtonText(1, CommonString::strOK);
            mp_MessageDlg->HideButtons();
            if (mp_MessageDlg->exec())
            {
                ui->programPanelWidget->EnableStartButton(true);
                return;
            }
        }


        mp_DataConnector->SendProgramAction("", DataManager::PROGRAM_START, 0, 0);
        ui->programPanelWidget->ChangeStartButtonToStopState();
        return;
    }

    if (IsOKPreConditionsToRunProgram())
    {
        int EndTimeDelta = m_AsapEndDateTime.secsTo(m_EndDateTime);
        int delayTime = EndTimeDelta + m_TimeDelta;
        int runDuration = Global::AdjustedTime::Instance().GetCurrentDateTime().secsTo(m_EndDateTime);
        mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_START, delayTime, runDuration);
        ui->programPanelWidget->ChangeStartButtonToStopState();
    }
    else
    {
        ui->programPanelWidget->EnableStartButton(true);
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
    const QList<QString>& stationList = cmd.StationList();
    if (stationList.count() == 0)
    {
        if (m_NewSelectedProgramId.at(0) == 'C')
        {
            mp_MessageDlg->SetIcon(QMessageBox::Warning);
            mp_MessageDlg->SetTitle(CommonString::strWarning);
            mp_MessageDlg->SetText("CleaningProgram does not need to be run as the reagent compatibility");
            mp_MessageDlg->SetButtonText(1, CommonString::strOK);
            mp_MessageDlg->HideButtons();
            if (mp_MessageDlg->exec())
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
            mp_MessageDlg->SetIcon(QMessageBox::Warning);
            mp_MessageDlg->SetTitle(CommonString::strWarning);
            QString strTemp = m_strNotFoundStation.arg(QString::number(i+1)).arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
            mp_MessageDlg->SetText(strTemp);
            mp_MessageDlg->SetButtonText(1, CommonString::strOK);
            mp_MessageDlg->HideButtons();

            ui->programPanelWidget->EnableStartButton(false);

            if (mp_MessageDlg->exec())
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
                mp_MessageDlg->SetIcon(QMessageBox::Warning);
                mp_MessageDlg->SetTitle(CommonString::strWarning);
                QString strTemp = m_strCheckEmptyStation.arg(pStation->GetDashboardStationName()).arg(QString::number(i+1)).arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME);
                mp_MessageDlg->SetText(strTemp);
                mp_MessageDlg->SetButtonText(1, CommonString::strOK);
                mp_MessageDlg->HideButtons();

                if (mp_MessageDlg->exec())
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
    emit SendAsapDateTime(asapEndTime, m_bIsFirstStepFixation);
    emit UpdateSelectedStationList(m_StationList);
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
    m_strCheckSafeReagent = QApplication::translate("Dashboard::CDashboardWidget", "No safe reagent is found for Program step \"%1\" of \"%2\",if errors happen.Would you like to continue?", 0, QApplication::UnicodeUTF8);
    m_strNotFoundStation = QApplication::translate("Dashboard::CDashboardWidget", "Program step \"%1\" of \"%2\" can not find the corresponding reagent station. One station can be used only once in the program. Please set a station for the reagent of this step.", 0, QApplication::UnicodeUTF8);
    m_strCheckEmptyStation = QApplication::translate("Dashboard::CDashboardWidget", "You cannot start the program, because the Station \"%1\" status is set as Empty in Program step \"%2\" of \"%3\".", 0, QApplication::UnicodeUTF8);
    m_strCannotStartParaffinMelt = QApplication::translate("Dashboard::CDashboardWidget", "Program cannot start because the paraffin is not melted completely and the first program step is not fixation reagent.", 0, QApplication::UnicodeUTF8);
    m_strWaitTimeforParaffinMelt = QApplication::translate("Dashboard::CDashboardWidget", "You can start in %1 hours %2 minutes %3 seconds.", 0, QApplication::UnicodeUTF8);
    m_strPromptProgramDelay =  QApplication::translate("Dashboard::CDashboardWidget", "Program will be delayed for some minutes in the first step as the paraffin is not melted completely. Would you like to continue?", 0, QApplication::UnicodeUTF8);
    m_strInputCassetteBoxTitle = QApplication::translate("Dashboard::CDashboardWidget", "Please enter cassette number:", 0, QApplication::UnicodeUTF8);
    m_strProgramWillComplete = QApplication::translate("Dashboard::CDashboardWidget", "Program \"%1\" has completed the last step! Would you like to drain the retort?", 0, QApplication::UnicodeUTF8);
    m_strTissueProtectPassed = QApplication::translate("Dashboard::CDashboardWidget", "Tissue protecting process is done successfully. Would you like to drain the retort?", 0, QApplication::UnicodeUTF8);
    m_strOvenCoverOpen = QApplication::translate("Dashboard::CDashboardWidget", "The oven cover is open. Please close it then click OK button.", 0, QApplication::UnicodeUTF8);
    m_strRetortCoverOpen = QApplication::translate("Dashboard::CDashboardWidget", "Retort lid was opened, please close it and then click OK.", 0, QApplication::UnicodeUTF8);
    m_strWaitingForFillingCompleted = QApplication::translate("Dashboard::CDashboardWidget", "Please wait for filling to be completed.", 0, QApplication::UnicodeUTF8);
    m_strItIsPausing = QApplication::translate("Dashboard::CDashboardWidget", "Pausing...", 0, QApplication::UnicodeUTF8);
    m_strWaitRotaryValveHeatingPrompt = QApplication::translate("Dashboard::CDashboardWidget", "Instrument is heating. Please wait for about 30 minutes.", 0, QApplication::UnicodeUTF8);
    m_strTakeOutSpecimen = QApplication::translate("Dashboard::CDashboardWidget", "Please take out the specimen!", 0, QApplication::UnicodeUTF8);
    m_strRetortContaminated  = QApplication::translate("Dashboard::CDashboardWidget", "The retort is contaminated. Please lock the retort and select Cleaning Program to run!", 0, QApplication::UnicodeUTF8);
    m_strProgramIsAborted  = QApplication::translate("Dashboard::CDashboardWidget", "Program \"%1\" is aborted!", 0, QApplication::UnicodeUTF8);
    m_strProgramComplete  = QApplication::translate("Dashboard::CDashboardWidget", "Program \"%1\" is completed successfully!", 0, QApplication::UnicodeUTF8);
    m_strRetortNotLock = QApplication::translate("Dashboard::CDashboardWidget", "Please close and lock the retort, then try again!", 0, QApplication::UnicodeUTF8);
    m_strNotStartRMSOFF = QApplication::translate("Dashboard::CDashboardWidget", "Leica Program can't be operated with RMS OFF.", 0, QApplication::UnicodeUTF8);
    m_strNotStartExpiredReagent = QApplication::translate("Dashboard::CDashboardWidget", "Reagents needed for this program are expired. Please replace the expired ones with fresh reagents! You can't run this program.", 0, QApplication::UnicodeUTF8);
    m_strStartExpiredReagent =  QApplication::translate("Dashboard::CDashboardWidget", "Would you like to start the Program with Expired Reagents?", 0, QApplication::UnicodeUTF8);
    m_strChangeCassetteBoxTitle = QApplication::translate("Dashboard::CDashboardWidget", "Please enter the new-added cassette number:", 0, QApplication::UnicodeUTF8);
    m_strAddCassete = QApplication::translate("Dashboard::CDashboardWidget", "Did you add new cassettes?", 0, QApplication::UnicodeUTF8);
    m_PowerFailureBoxTitle = QApplication::translate("Dashboard::CDashboardWidget", "Because the waiting time is more than five minutes, the instrument will be re-heated and filled with safety reagents.", 0, QApplication::UnicodeUTF8);
    m_strCoolingDown = QApplication::translate("Dashboard::CDashboardWidget", "Retort temperature is too high, please wait for a few minutes to cool it down before next program.", 0, QApplication::UnicodeUTF8);
    QString strTemp = QString("Starting the chosen program is impossible, because its Paraffin temperature is differing from set Paraffin bath temperature (deviation 2°C).") +
    QString("Program can be started immediately, if Paraffin temperature in this program is adapted to match the Paraffin bath temperature.") +
    QString("Program is impossible to start for up to 15 hours if Paraffin bath temperature is increased to equal with Paraffin program temperature! ") +
    QString("Please confirm modifying Paraffin temperature.");
    QByteArray ba = strTemp.toLatin1();
    char* ch =ba.data();
    m_strDiffTemp = QApplication::translate("Dashboard::CDashboardWidget", ch,
                                            0, QApplication::UnicodeUTF8);
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

    emit SendAsapDateTime(asapEndTime, m_bIsFirstStepFixation);
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

    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
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

            //Abort
            if (m_CurProgramStepIndex == 0)
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
        if (m_ProgramStatus == ProgramRunning)
        {
            ui->programPanelWidget->EnablePauseButton(m_ProgramStageStatus == Enabled);
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



