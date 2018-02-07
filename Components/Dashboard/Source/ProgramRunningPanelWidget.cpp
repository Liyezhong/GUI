#include "Dashboard/Include/ProgramRunningPanelWidget.h"
#include "ui_ProgramRunningPanelWidget.h"
#include <QDateTime>
#include <QTimer>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdCurrentProgramStepInfor.h>
#include "Global/Include/Utils.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "MainMenu/Include/MainWindow.h"
#include "Dashboard/Include/DashboardProgramStatusWidget.h"
#include "Core/Include/Startup.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardStation.h"
#include "Dashboard/Include/DashboardWidget.h"
#include "Core/Include/GlobalHelper.h"

namespace Dashboard
{
CProgramRunningPanelWidget::CProgramRunningPanelWidget(QWidget *p):
    CIconTitlePanelFrame(p),
    ui(new Ui::CProgramRunningPanelWidget),
    m_remainingTimeTotal(0),
    m_curRemainingTimeTotal(0),
    m_strAborted(tr("Aborted.")),
    m_strCompleted(tr("Completed!")),
    mp_UserSettings(NULL),
    m_DateTimeStr(""),
    m_selectedProgramId(""),
    m_isAborting(false),
    m_firstProgramStepIndex(-1),
    m_pProgramStatusWidget(NULL)
{
    qDebug()<<"CProgramRunningPanelWidget::CProgramRunningPanelWidget  m_remainingTimeTotal = "<<m_remainingTimeTotal;

    ui->setupUi(GetContentFrame());
    mp_ProgressTimer = new QTimer(this);
    mp_ProgressTimer->setInterval(1000);
    CONNECTSIGNALSLOT(mp_ProgressTimer, timeout(), this, UpdateProgress());
    CONNECTSIGNALSLOT(ui->btnProgramDetail, clicked(), this, OnProgramDetail());
}

CProgramRunningPanelWidget::~CProgramRunningPanelWidget()
{
    try {
            delete ui;
            delete mp_ProgressTimer;
        }
        catch (...) {
            // to please Lint.
        }
}

void CProgramRunningPanelWidget::OnProgramActionStopped(DataManager::ProgramStatusType_t ProgramStatusType)
{
    qDebug()<<"CProgramRunningPanelWidget::OnProgramActionStopped  ProgramStatusType="<<ProgramStatusType;

    mp_ProgressTimer->stop();//the progress bar and Time countdown will stop
    if (DataManager::PROGRAM_STATUS_PAUSED == ProgramStatusType)
    {
       return;
    }
    else if (DataManager::PROGRAM_STATUS_ABORTED == ProgramStatusType)
    {
        ui->lblReagentName->setText(m_strAborted);
        ui->lblRemainTime->setText("00:00:00");
        m_isAborting = false;
    }
    else if (DataManager::PROGRAM_STATUS_COMPLETED == ProgramStatusType)
    {
        ui->lblReagentName->setText(m_strCompleted);
        ui->lblRemainTime->setText("00:00");
    }
    ui->stepTimeLabel->setVisible(false);
    ui->lblStepTime->setVisible(false);
}

void CProgramRunningPanelWidget::OnProgramActionStarted(DataManager::ProgramActionType_t ProgramActionType,
                                                     int remainingTimeTotal, const QDateTime& startDateTime, bool IsResume)
{
    qDebug()<<"CProgramRunningPanelWidget::OnProgramActionStarted  reamainigTimeTotal="<<remainingTimeTotal<<"  IsResume="<<IsResume;

   Q_UNUSED(startDateTime);

   if (!IsResume)
   {
       m_remainingTimeTotal = remainingTimeTotal;       
       m_curRemainingTimeTotal = remainingTimeTotal;
   }

   qDebug()<<"CProgramRunningPanelWidget::OnProgramActionStarted  m_remainingTimeTotal = "<<m_remainingTimeTotal;

   mp_ProgressTimer->start();
   ui->stepTimeLabel->setVisible(true);
   ui->lblStepTime->setVisible(true);
   ui->lblReagentName->setVisible(true);
   if (DataManager::PROGRAM_ABORT == ProgramActionType)
   {
        ui->lblReagentName->setText(m_strAborting);//only show the first label

        ui->lblStepTime->setVisible(false);
        ui->stepTimeLabel->setVisible(false);
        m_isAborting = true;
        m_curRemainingTimeTotal = m_remainingTimeTotal;
        ui->lblRemainTime->setText(Core::CGlobalHelper::TimeToString(m_curRemainingTimeTotal, true));
   }
   else if (DataManager::PROGRAM_START == ProgramActionType)
   {
       m_isAborting = false;
   }
}

void CProgramRunningPanelWidget::OnCurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor & cmd)
{
    qDebug()<<"CProgramRunningPanelWidget::OnCurrentProgramStepInforUpdated  cmd="<<cmd.StepName();

    ui->lblReagentName->setText(cmd.StepName());
    QString timeStr = Core::CGlobalHelper::TimeToString(cmd.CurRemainingTime(), true);
    ui->lblStepTime->setText(timeStr);
    m_CurStepRemainingTime = m_CurRemainingTime = cmd.CurRemainingTime();
    m_CurProgramStepIndex = cmd.CurProgramStepIndex();
}

void CProgramRunningPanelWidget::OnCurrentProgramStepInforUpdated(const QString& stepName,
                                                                  int stepRemainingTime,
                                                                  int totalRemainingTime,
                                                                  int stepIndex)
{

    ui->lblReagentName->setText(stepName);
    QString timeStr = Core::CGlobalHelper::TimeToString(stepRemainingTime, true);
    ui->lblStepTime->setText(timeStr);
    m_CurStepRemainingTime = m_CurRemainingTime = stepRemainingTime;
    m_CurProgramStepIndex = stepIndex;

    m_curRemainingTimeTotal = m_remainingTimeTotal = totalRemainingTime;
    ui->lblRemainTime->setText(Core::CGlobalHelper::TimeToString(m_curRemainingTimeTotal, false));

    m_ProgramEndDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(m_curRemainingTimeTotal);
    UpdateDateTime();
}


void CProgramRunningPanelWidget::UpdateProgress()
{
    if (!m_isAborting)
    {
        // update the remaining time for single step
        if (m_CurRemainingTime>0) {
            ui->lblStepTime->setText(Core::CGlobalHelper::TimeToString(--m_CurRemainingTime, true));
        }

        // update the total remaining time        
        if (m_curRemainingTimeTotal > 0) {
            ui->lblRemainTime->setText(Core::CGlobalHelper::TimeToString(--m_curRemainingTimeTotal, false));
        }
    }
    else
    {
        // to avoid negative number
        if (m_curRemainingTimeTotal>0) {
            ui->lblRemainTime->setText(Core::CGlobalHelper::TimeToString(m_curRemainingTimeTotal--, true));
        }
    }
}

void CProgramRunningPanelWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

void CProgramRunningPanelWidget::RetranslateUI()
{
    m_strAborting  = QApplication::translate("Dashboard::CProgramRunningPanelWidget", "Aborting...", Q_NULLPTR, -1);
    m_strAborted   = QApplication::translate("Dashboard::CProgramRunningPanelWidget", "Aborted.", Q_NULLPTR, -1);
    m_strCompleted = QApplication::translate("Dashboard::CProgramRunningPanelWidget", "Completed!", Q_NULLPTR, -1);
    m_strStation   = QApplication::translate("Dashboard::CProgramRunningPanelWidget", "Station", Q_NULLPTR, -1);
    m_strTemp      = QApplication::translate("Dashboard::CProgramRunningPanelWidget", "Temp.", Q_NULLPTR, -1);
}

void CProgramRunningPanelWidget::SetUserSettings(DataManager::CUserSettings *pUserSettings)
{
    mp_UserSettings = pUserSettings;
}

void CProgramRunningPanelWidget::OnUserSettingsUpdated()
{
   if (mp_UserSettings)
   {
        m_CurDateFormat = mp_UserSettings->GetDateFormat();
        m_CurTimeFormat = mp_UserSettings->GetTimeFormat();
        UpdateDateTime();
   }
}

void CProgramRunningPanelWidget::UpdateEndDateTime(int endTimeDiff)
{
    m_ProgramEndDateTime = m_ProgramEndDateTime.addSecs(endTimeDiff);
    UpdateDateTime();
}

void CProgramRunningPanelWidget::OnUserSetEndDateTime(const QDateTime& dateTime)
{
    m_ProgramEndDateTime = dateTime;
    UpdateDateTime();
}

void CProgramRunningPanelWidget::UpdateDateTime()
{
    if (!this->isVisible())
        return;

    QString DateStr;
    QString TimeStr;

    switch(m_CurDateFormat) {
        case Global::DATE_INTERNATIONAL:
        {
            DateStr = QString("%1").arg(m_ProgramEndDateTime.date().toString("dd.MM.yyyy"));
        }
        break;
        case Global::DATE_ISO:
        {
            DateStr = QString("%1").arg(m_ProgramEndDateTime.date().toString("yyyy-MM-dd"));
        }
        break;
        case Global::DATE_US:
        {
            DateStr = QString("%1").arg(m_ProgramEndDateTime.date().toString("MM/dd/yyyy"));
        }
        break;
        case Global::DATE_UNDEFINED:
        default:
        {
            qDebug() << " Date format is Invalid";
        }
    }

    switch(m_CurTimeFormat) {
        case Global::TIME_12:
        {
            TimeStr = QString("%1").arg(m_ProgramEndDateTime.time().toString("hh:mm ap"));
        }
            break;
        case Global::TIME_24:
        {
            TimeStr = QString("%1").arg(m_ProgramEndDateTime.time().toString("hh:mm"));
        }
            break;
        case Global::TIME_UNDEFINED:
        default:
        {
            qDebug() << " Time format is Invalid";
        }
    }

    QString DateTimeStr;
    DateTimeStr.append(TimeStr);
    DateTimeStr.append("\n");
    DateTimeStr.append(DateStr);
    m_DateTimeStr = "";
    m_DateTimeStr.append(TimeStr);

    QDateTime CurDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    uint TotalSecs = m_ProgramEndDateTime.toTime_t() - CurDateTime.toTime_t();
    if (TotalSecs >= 24 * 3600) {
        m_DateTimeStr.append(" ");
        m_DateTimeStr.append(DateStr);
    }
    ui->lblEndtime->setText(DateTimeStr);
}
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function showEvent
 *
 *  \param p_Event = QShowEvent type parameter
 *
 *  \return from showEvent
 */
/****************************************************************************/
void CProgramRunningPanelWidget::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);
    UpdateDateTime();
}
void CProgramRunningPanelWidget::ProgramSelected(QString& programId, int asapEndTime,
                                                 bool bIsFirstStepFixation,
                                                 QList<QString>& selectedStationList, int firstProgramStepIndex)
{
    m_selectedProgramId = programId;
    m_selectedStationList = selectedStationList;
    m_firstProgramStepIndex = firstProgramStepIndex;
    Q_UNUSED(bIsFirstStepFixation);
    m_ProgramEndDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(asapEndTime);
    UpdateDateTime();
}

int CProgramRunningPanelWidget::GetStepRemainingTime()
{
   return m_CurRemainingTime;
}

int CProgramRunningPanelWidget::GetProgramRemainingTime()
{
   return m_curRemainingTimeTotal;
}

const QString CProgramRunningPanelWidget::GetEndDateTime()
{
   return m_DateTimeStr;
}

void CProgramRunningPanelWidget::EnableProgramDetailButton(bool bEnable)
{
    ui->btnProgramDetail->setEnabled(bEnable);
}

void CProgramRunningPanelWidget::SetSelectedProgramId(const QString& programID)
{
    m_selectedProgramId = programID;
}

void CProgramRunningPanelWidget::OnProcessStateChanged()
{
   bool bProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
   if (!bProcessRunning)
    mp_ProgressTimer->stop();//the progress bar and Time countdown will stop
}

void CProgramRunningPanelWidget::UpdateProgramTimerStatus(bool enable)
{
    if (enable)
    {
        mp_ProgressTimer->start();
    }
    else
    {
        if (!m_isAborting)
            mp_ProgressTimer->stop();
    }
}

void CProgramRunningPanelWidget::GetStationNameList(QList<QString>& stationNameList)
{
    Core::CStartup* pStartup = Core::CStartup::instance();
    const DataManager::CProgram* pProgram = pStartup->DataConnector()->ProgramList ->GetProgram(m_selectedProgramId);
    if (pProgram)
    {
        for (int i = 0; i < m_selectedStationList.count(); i++)
        {
            DataManager::CDashboardStation* pStation = pStartup->DataConnector()->DashboardStationList->GetDashboardStation(m_selectedStationList.at(i));
            if (!pStation)
            {
                Q_ASSERT(0);
            }
            stationNameList.append(pStation->GetDashboardStationName());
        }
    }
}

void CProgramRunningPanelWidget::OnProgramDetail()
{
    if (m_selectedProgramId.isEmpty())
        return;

    m_pProgramStatusWidget = new Dashboard::CDashboardProgramStatusWidget(this);

    Core::CStartup* pStartup = Core::CStartup::instance();
    MainMenu::CMainWindow * pMainWin = pStartup->MainWindow();
    //for position the window of ProgramStatusWidget
    m_pProgramStatusWidget->SetMainWindow(pMainWin);

    const DataManager::CProgram* pProgram = pStartup->DataConnector()->ProgramList ->GetProgram(m_selectedProgramId);
    Q_ASSERT(pProgram);

    if (!pProgram)
        return;

    QList<QString> stationNameList;
    for (int i = 0; i < m_firstProgramStepIndex; i++)
    {
        stationNameList.append("");
    }
    GetStationNameList(stationNameList);


    bool bAboutEnable = pStartup->Dashboard()->IsAbortEnabled();
    m_pProgramStatusWidget->InitDialog(const_cast<DataManager::CProgram*>(pProgram), pStartup->DataConnector(),
                                       stationNameList, m_CurProgramStepIndex,
                                        m_CurRemainingTime,
                                       m_curRemainingTimeTotal,
                                       m_DateTimeStr, bAboutEnable,
                                       m_strStation,  m_strTemp);

    (void)QObject::connect(m_pProgramStatusWidget, SIGNAL(AbortClicked(int)), this, SIGNAL(AbortClicked(int)));
    (void)m_pProgramStatusWidget->exec();
    (void)QObject::disconnect(m_pProgramStatusWidget, SIGNAL(AbortClicked(int)), this, SIGNAL(AbortClicked(int)));
    delete m_pProgramStatusWidget;
    m_pProgramStatusWidget = NULL;
}

void CProgramRunningPanelWidget::UpdateAbortButtonStatus()
{
    if (m_pProgramStatusWidget)
    {
        m_pProgramStatusWidget->DisableAbortButton();
    }
}

}
