#include "Dashboard/Include/ProgramPanelWidget.h"
#include "ui_ProgramPanelWidget.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/MainWindow.h"
#include "Dashboard/Include/DashboardDataTypes.h"
#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "Core/Include/DataConnector.h"
#include "MainMenu/Include/MessageDlg.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "Dashboard/Include/CassetteNumberInputWidget.h"
#include "Dashboard/Include/CommonString.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardStation.h"
#include "HimalayaDataContainer/Helper/Include/Global.h"
#include "Core/Include/GlobalHelper.h"

//lint -e613

namespace Dashboard {
CProgramPanelWidget::CProgramPanelWidget(QWidget *p) :
    CPanelFrame(p),
    ui(new Ui::CProgramPanelWidget),
    m_ProgramNextAction(DataManager::PROGRAM_START),
    m_IsResumeRun(false),
    m_SelectedProgramId(""),
    mp_DataConnector(NULL),
    m_pUserSetting(NULL),
    mp_ProgramList(NULL),
    m_startButtonDisabledAsSysError(false),
    m_pauseButtonDisabledAsSysError(false),
    m_ProgramStartReady(false),
    m_bWaitRotaryValveHeatingPrompt(false)
{
    ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("Programs"));
    CONNECTSIGNALSLOT(this, ResetFocus(bool), ui->favoriteProgramsPanel, OnResetFocus(bool));
    CONNECTSIGNALSLOT(this, AddItemsToFavoritePanel(bool), ui->favoriteProgramsPanel, AddItemsToFavoritePanel(bool));
    CONNECTSIGNALSLOT(this, AddItemsToFavoritePanel(), ui->favoriteProgramsPanel, AddItemsToFavoritePanel());

    CONNECTSIGNALSIGNAL(ui->favoriteProgramsPanel, PrepareSelectedProgramChecking(const QString&), this, PrepareSelectedProgramChecking(const QString&));
    CONNECTSIGNALSLOT(ui->favoriteProgramsPanel, PrepareSelectedProgramChecking(const QString&), this, OnPrepareSelectedProgramChecking(const QString&));

    CONNECTSIGNALSLOT(this, OnSelectEndDateTime(const QDateTime&),
                      ui->programRunningPanel, OnUserSetEndDateTime(const QDateTime&));

    CONNECTSIGNALSLOT(ui->favoriteProgramsPanel, UpdateFavProgram(), this, OnUpdatePanelProgram());

    CONNECTSIGNALSLOT(this, ProgramSelected(QString&, int, bool, QList<QString>&, int),
                      ui->programRunningPanel, ProgramSelected(QString&, int, bool, QList<QString>&, int));

    CONNECTSIGNALSLOT(this, ProgramSelected(QString&, int, bool, QList<QString>&, int),
                      this, OnProgramSelected(QString&, int, bool, QList<QString>&));

    CONNECTSIGNALSLOT(this, UndoProgramSelection(),
                      ui->favoriteProgramsPanel, UndoProgramSelection());

    CONNECTSIGNALSLOT(this, ProgramActionStopped(DataManager::ProgramStatusType_t),
                        ui->programRunningPanel, OnProgramActionStopped(DataManager::ProgramStatusType_t));

    CONNECTSIGNALSLOT(this, ProgramActionStopped(DataManager::ProgramStatusType_t),
                        this, OnProgramActionStopped(DataManager::ProgramStatusType_t));


    CONNECTSIGNALSLOT(this, ProgramActionStarted(DataManager::ProgramActionType_t, int, const QDateTime&, bool),
                      ui->programRunningPanel, OnProgramActionStarted(DataManager::ProgramActionType_t, int, const QDateTime&, bool));

    CONNECTSIGNALSLOT(this, ProgramActionStarted(DataManager::ProgramActionType_t, int, const QDateTime&, bool),
                      this, OnProgramActionStarted(DataManager::ProgramActionType_t, int, const QDateTime&, bool));

    CONNECTSIGNALSLOT(this, CurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor &),
                      ui->programRunningPanel, OnCurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor &));

    m_btnGroup.addButton(ui->startButton, Dashboard::firstButton);
    m_btnGroup.addButton(ui->pauseButton, Dashboard::secondButton);

    CONNECTSIGNALSLOT(&m_btnGroup, buttonClicked(int), this, OnButtonClicked(int));
    ui->startButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    mp_MessageDlg = new MainMenu::CMessageDlg(this);

    CONNECTSIGNALSLOT(ui->programRunningPanel, AbortClicked(int), this, OnButtonClicked(int));

    CONNECTSIGNALSLOT(this, UpdateProgram(DataManager::CProgram&), ui->favoriteProgramsPanel, UpdateProgram(DataManager::CProgram&));
}

CProgramPanelWidget::~CProgramPanelWidget()
{
    try {
            delete ui;
            delete mp_MessageDlg;
        }
        catch (...) {
            // to please Lint.
        }
}

void CProgramPanelWidget::changeEvent(QEvent *p_Event)
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

void CProgramPanelWidget::RetranslateUI()
{
    SetPanelTitle(QApplication::translate("Dashboard::CProgramPanelWidget", "Programs",
                                                                 Q_NULLPTR, -1));
    m_strConfirmation = QApplication::translate("Dashboard::CProgramPanelWidget", "Confirmation Message", Q_NULLPTR, -1);
    m_strAbortProgram = QApplication::translate("Dashboard::CProgramPanelWidget", "Would you like to abort the program?", Q_NULLPTR, -1);

    m_strYes = QApplication::translate("Dashboard::CProgramPanelWidget", "Yes", Q_NULLPTR, -1);
    m_strNo = QApplication::translate("Dashboard::CProgramPanelWidget", "No", Q_NULLPTR, -1);
    m_strOK = QApplication::translate("Dashboard::CProgramPanelWidget", "OK", Q_NULLPTR, -1);
    m_strCancel = QApplication::translate("Dashboard::CProgramPanelWidget", "Cancel", Q_NULLPTR, -1);
    m_strWarning = QApplication::translate("Dashboard::CProgramPanelWidget", "Warning", Q_NULLPTR, -1);
    m_strProgram = QApplication::translate("Dashboard::CProgramPanelWidget", "Program", Q_NULLPTR, -1);
    m_strInformation = QApplication::translate("Dashboard::CProgramPanelWidget", "Information", Q_NULLPTR, -1);
    m_strStart = QApplication::translate("Dashboard::CProgramPanelWidget", "Start", Q_NULLPTR, -1);
    m_strAbort = QApplication::translate("Dashboard::CProgramPanelWidget", "Abort", Q_NULLPTR, -1);
}

void CProgramPanelWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector)
{
    ui->favoriteProgramsPanel->SetPtrToMainWindow(p_MainWindow, p_DataConnector);
    mp_DataConnector = p_DataConnector;
    m_pUserSetting = mp_DataConnector->SettingsInterface->GetUserSettings();
    mp_ProgramList = mp_DataConnector->ProgramList;

    if (!mp_DataConnector)
        return;
    ui->programRunningPanel->SetUserSettings(m_pUserSetting);
    CONNECTSIGNALSLOT(mp_DataConnector, UserSettingsUpdated(), ui->programRunningPanel, OnUserSettingsUpdated());
    CONNECTSIGNALSLOT(p_MainWindow, ProcessStateChanged(), ui->programRunningPanel, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_DataConnector, UpdateProgramEndTime(int), ui->programRunningPanel, UpdateEndDateTime(int));
    CONNECTSIGNALSLOT(mp_DataConnector, UpdateProgramTimerStatus(bool), ui->programRunningPanel, UpdateProgramTimerStatus(bool));
    CONNECTSIGNALSLOT(mp_DataConnector, UpdateProgramTimerStatus(bool), this, UpdateProgramTimerStatus(bool));
}

void CProgramPanelWidget::UpdateProgramTimerStatus(bool enable)
{
    if (enable)
    {
        if (m_startButtonDisabledAsSysError)
        {
            EnableStartButton(true);
            m_startButtonDisabledAsSysError = false;
        }
        if (m_pauseButtonDisabledAsSysError)
        {
            EnablePauseButton(true);
            m_pauseButtonDisabledAsSysError = false;
        }
    }
    else
    {
        if (ui->startButton->isEnabled())
        {
            EnableStartButton(false);
            m_startButtonDisabledAsSysError = true;
        }

        if (ui->pauseButton->isEnabled())
        {
            EnablePauseButton(false);
            m_pauseButtonDisabledAsSysError = true;
        }
    }
}

void CProgramPanelWidget::OnProgramSelected(QString& ProgramId, int asapEndTime, bool bIsFirstStepFixation,
                                            QList<QString>& selectedStationList)
{
    Q_UNUSED(bIsFirstStepFixation);
    m_SelectedProgramId = ProgramId;
    m_EndDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(asapEndTime);

    if (m_ProgramNextAction != DataManager::PROGRAM_START)
    {
        this->ChangeStartButtonToStartState();
    }

    if ((m_bWaitRotaryValveHeatingPrompt || m_ProgramStartReady) && !Core::CGlobalHelper::GetSystemErrorStatus())
        OnProgramStartReadyUpdated();
    m_StationList.clear();
    m_StationList = selectedStationList;
    Core::CGlobalHelper::SetStationList(selectedStationList);
}

bool CProgramPanelWidget::CheckPreConditionsToPauseProgram()
{
    return true;
}

bool CProgramPanelWidget::CheckPreConditionsToAbortProgram()
{
    MainMenu::CMessageDlg ConfirmationMessageDlg;

    ConfirmationMessageDlg.SetTitle(m_strConfirmation);
    ConfirmationMessageDlg.SetText(m_strAbortProgram);
    ConfirmationMessageDlg.SetIcon(QMessageBox::Information);
    ConfirmationMessageDlg.SetButtonText(1, CommonString::strYes);
    ConfirmationMessageDlg.SetButtonText(3, CommonString::strCancel);
    ConfirmationMessageDlg.HideCenterButton();
    return ConfirmationMessageDlg.exec() == (int)QDialog::Accepted;
}

void CProgramPanelWidget::OnButtonClicked(int whichBtn)
{
    ui->favoriteProgramsPanel->SetInFavProgramButtonClicked();

    if ( whichBtn == Dashboard::firstButton ) {
        ui->startButton->setEnabled(false);//protect to click twice in a short time

        switch(m_ProgramNextAction)
        {      
            default:
                break;
            case DataManager::PROGRAM_START:
            {               
                emit CheckPreConditionsToRunProgram();
            }
            break;
            case DataManager::PROGRAM_ABORT:
            {
                if(CheckPreConditionsToAbortProgram()) {
                    ui->pauseButton->setEnabled(false);
                    ui->programRunningPanel->EnableProgramDetailButton(false);
                    mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_ABORT);
                    m_ProgramNextAction = DataManager::PROGRAM_START;

                }
                else {
                    // cancel button pressed.
                    ui->startButton->setEnabled(true);
                }
            }
            break;
        }
    }
    else if (whichBtn == Dashboard::secondButton)//pause
    {
        ui->pauseButton->setEnabled(false);//protect to click twice in a short time
        if(CheckPreConditionsToPauseProgram())
        {
            mp_DataConnector->SendProgramAction(m_SelectedProgramId, DataManager::PROGRAM_PAUSE);
        } else {
            // Take Necessary Action
        }
    }
}

void CProgramPanelWidget::ChangeStartButtonToStopState()
{
    ui->startButton->setText(m_strAbort);
    ui->startButton->update();
    ui->startButton->IsStartStatus(false);
    m_ProgramNextAction = DataManager::PROGRAM_ABORT;
}

void CProgramPanelWidget::ChangeStartButtonToStartState()
{
    ui->startButton->setText(m_strStart);
    ui->startButton->update();
    ui->startButton->IsStartStatus(true);
    m_ProgramNextAction = DataManager::PROGRAM_START;
}

void CProgramPanelWidget::EnableStartButton(bool bEnable, bool bProgramNotStart)
{
    if (bEnable && (m_SelectedProgramId.at(0) == 'C') && !bProgramNotStart)
    {
        return;
    }

    ui->startButton->setEnabled(bEnable);

    if (!bEnable && !(ui->startButton->IsStartStatus()))
    {
        ui->programRunningPanel->UpdateAbortButtonStatus();
    }
}

void CProgramPanelWidget::ResumePauseRunningStatus(bool bEnable)
{
    ui->programRunningPanel->UpdateProgramTimerStatus(bEnable);
}

void CProgramPanelWidget::EnablePauseButton(bool bEnable)
{
    ui->pauseButton->setEnabled(bEnable);
}

void CProgramPanelWidget::IsResumeRun(bool bSet)
{
    m_IsResumeRun = bSet;
}

bool CProgramPanelWidget::IsResumeRun()
{
    return m_IsResumeRun;
}

bool CProgramPanelWidget::IsAbortEnabled()
{
    return (this->ui->startButton->isEnabled() && !(ui->startButton->IsStartStatus()));
}

void CProgramPanelWidget::ProgramReadyPrompt()
{
    this->ui->startButton->setEnabled(true);
}

void CProgramPanelWidget::OnProgramStartReadyUpdated()
{
    if ((m_ProgramStartReady || m_bWaitRotaryValveHeatingPrompt) && !Core::CGlobalHelper::GetSystemErrorStatus())
	{
    	if (!m_SelectedProgramId.isEmpty())
        {
            ProgramReadyPrompt();
        }
	}
}

void CProgramPanelWidget::OnWaitRotaryValveHeatingPrompt()
{
    if (m_bWaitRotaryValveHeatingPrompt && !Core::CGlobalHelper::GetSystemErrorStatus())
    {
        if (!m_SelectedProgramId.isEmpty())
            ProgramReadyPrompt();
    }
}

void CProgramPanelWidget::OnProgramActionStarted(DataManager::ProgramActionType_t ProgramActionType,
                                                     int remainingTimeTotal, const QDateTime& startDateTime, bool IsResume)
{
    Q_UNUSED(remainingTimeTotal);
    Q_UNUSED(startDateTime);
    if (DataManager::PROGRAM_START== ProgramActionType)
    {
        if (m_SelectedProgramId.at(0) == 'C')
        {
            QDateTime endDatetime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(remainingTimeTotal);
            ui->programRunningPanel->OnUserSetEndDateTime(endDatetime);
        }
        ui->stackedWidget->setCurrentIndex(1);
        QString strIconName = ":/HimalayaImages/Icons/Program/"+ mp_ProgramList->GetProgram(m_SelectedProgramId)->GetIcon() + ".png";
        ui->programRunningPanel->SetPanelIcon(strIconName);
        ui->programRunningPanel->SetPanelTitle(QString("%1").arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME));
        if (!IsResume)
            ui->programRunningPanel->EnableProgramDetailButton(false);
    }
}

void CProgramPanelWidget::SwitchToProgramRunningStatus(const MsgClasses::CmdRecoveryFromPowerFailure& cmd)
{
    QString selectedProgramId = cmd.GetProgramID();
    ui->stackedWidget->setCurrentIndex(1);
    const DataManager::CProgram* pProgram = mp_ProgramList->GetProgram(selectedProgramId);
    if (!pProgram)
        return;
    QString strIconName = ":/HimalayaImages/Icons/Program/"+ pProgram->GetIcon() + ".png";
    ui->programRunningPanel->SetPanelIcon(strIconName);

    QString selectedProgramName = pProgram->GetName();
    ui->programRunningPanel->SetPanelTitle(selectedProgramName);
    ui->programRunningPanel->SetSelectedProgramId(selectedProgramId);
    ui->programRunningPanel->EnableProgramDetailButton(true);
    QString strReagentID = pProgram->GetProgramStep(cmd.GetStepIndex())->GetReagentID();
    const DataManager::CReagent *p_Reagent = mp_DataConnector->ReagentList->GetReagent(strReagentID);
    ui->programRunningPanel->OnCurrentProgramStepInforUpdated(p_Reagent->GetReagentName(), 0,
                                                              cmd.GetRemainingTime(), cmd.GetStepIndex());
}

void CProgramPanelWidget::ProgramStartReady(bool bSet)
{
    m_ProgramStartReady = bSet;
    if (m_ProgramStartReady)
    {
        m_bWaitRotaryValveHeatingPrompt = false;
    }
}

void CProgramPanelWidget::WaitRotaryValveHeatingPrompt(bool bSet)
{
    m_bWaitRotaryValveHeatingPrompt = bSet;
    if (m_bWaitRotaryValveHeatingPrompt)
    {
        m_ProgramStartReady = false;
    }
}

void CProgramPanelWidget::OnPreTestDone()
{
    ui->programRunningPanel->EnableProgramDetailButton(true);
}

void CProgramPanelWidget::OnProgramActionStopped(DataManager::ProgramStatusType_t ProgramStatusType)
{
    if (DataManager::PROGRAM_STATUS_COMPLETED == ProgramStatusType)
    {
        SwitchToFavoritePanel();
    }
}

void CProgramPanelWidget::SwitchToFavoritePanel()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->favoriteProgramsPanel->ResetInFavProgramButtonClicked();
}

void CProgramPanelWidget::OnUpdatePanelProgram()
{
    if (!m_IsResumeRun)
        EnableStartButton(false);
}

void CProgramPanelWidget::OnPrepareSelectedProgramChecking(const QString&)
{
    ui->startButton->setEnabled(false);
}

void CProgramPanelWidget::ResetInFavProgramButtonClicked()
{
    ui->favoriteProgramsPanel->ResetInFavProgramButtonClicked();
}

}
