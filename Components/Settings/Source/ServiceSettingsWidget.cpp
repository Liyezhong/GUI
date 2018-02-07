/****************************************************************************/
/*! \file ServiceSettingsWidget.cpp
 *
 *  \brief ServiceSettingsWidget implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-29
 *   $Author:  $ M.Scherer
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


#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "Settings/Include/ServiceSettingsWidget.h"
#include "ui_ServiceSettingsWidget.h"
#include "MainMenu/Include/MessageDlg.h"
#include "Dashboard/Include/CommonString.h"
#include "Core/Include/GlobalHelper.h"

namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CServiceSettingsWidget::CServiceSettingsWidget(QWidget *p_Parent) :  MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CServiceSettingsWidget),
    mp_MainWindow(NULL),
    m_ProcessRunning(false),
    m_CurrentUserRole(MainMenu::CMainWindow::Operator),
    mp_UserSettings(NULL)
{
    mp_Ui->setupUi(GetContentFrame());
    CONNECTSIGNALSLOT(mp_Ui->btnResetOperationHour, clicked(), this, OnResetOperationDays());
    CONNECTSIGNALSLOT(mp_Ui->btnResetCarbonFilter, clicked(), this, OnResetCarbonFilter());
    CONNECTSIGNALSLOT(mp_Ui->btnSave, clicked(), this, OnSaveSetting());
    CONNECTSIGNALSLOT(mp_Ui->btnShutdown, clicked(), this, OnPrepareShutdown());
    CONNECTSIGNALSLOT(mp_Ui->btnStartServiceApp, clicked(), this, OnStartServiceApp());

    if (mp_UserSettings)
        m_UserSettingsTemp = *mp_UserSettings;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CServiceSettingsWidget::~CServiceSettingsWidget()
{
    delete mp_Ui;
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CServiceSettingsWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            RetranslateUI();
            break;
        default:
            break;
    }
}

QString CServiceSettingsWidget::GetFormattedCurrentDateString()
{
    if (!mp_UserSettings)
        return "";

    Global::DateFormat curDateFormat = mp_UserSettings->GetDateFormat();
    QDateTime curDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();

    QString dateStr;
    switch(curDateFormat) {
        case Global::DATE_INTERNATIONAL:
        {
            dateStr = QString("%1").arg(curDateTime.date().toString("dd.MM.yyyy"));
        }
        break;
        case Global::DATE_ISO:
        {
            dateStr = QString("%1").arg(curDateTime.date().toString("yyyy-MM-dd"));
        }
        break;
        case Global::DATE_US:
        {
            dateStr = QString("%1").arg(curDateTime.date().toString("MM/dd/yyyy"));
        }
        break;
        case Global::DATE_UNDEFINED:
        default:
        {
            qDebug() << " Date format is Invalid";
            dateStr = "";
        }
    }
    return dateStr;
}

QString CServiceSettingsWidget::GetFormattedDateString(const QDateTime& dateTime)
{
    if (!mp_UserSettings)
        return "";

    Global::DateFormat curDateFormat = mp_UserSettings->GetDateFormat();
    QString dateStr;
    switch(curDateFormat) {
        case Global::DATE_INTERNATIONAL:
        {
            dateStr = QString("%1").arg(dateTime.date().toString("dd.MM.yyyy"));
        }
        break;
        case Global::DATE_ISO:
        {
            dateStr = QString("%1").arg(dateTime.date().toString("yyyy-MM-dd"));
        }
        break;
        case Global::DATE_US:
        {
            dateStr = QString("%1").arg(dateTime.date().toString("MM/dd/yyyy"));
        }
        break;
        case Global::DATE_UNDEFINED:
        default:
        {
            qDebug() << " Date format is Invalid";
            dateStr = "";
        }
    }
    return dateStr;
}

/****************************************************************************/
/*!
 *  \brief Updates the widget content everytime it is displayed
 *
 *  \iparam p_Event = Show event
 */
/****************************************************************************/
void CServiceSettingsWidget::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);
    ResetButtons();

    QString dateStr = GetFormattedCurrentDateString();
    if ("" == mp_UserSettings->GetOperationLastResetDate())
    {
        mp_Ui->labelResetOperationDate->setText(dateStr);
    }
    else
    {
        QString str = mp_UserSettings->GetOperationLastResetDate();
        QDateTime lastResetDate = QDateTime::fromString(str);
        mp_Ui->labelResetOperationDate->setText(GetFormattedDateString(lastResetDate));
        m_UserSettingsTemp.SetOperationLastResetDate(str);
    }

    if ("" == mp_UserSettings->GetActiveCarbonLastResetDate())
    {
        mp_Ui->labelResetCarbonFilterDate->setText(dateStr);
    }
    else
    {
        QString str = mp_UserSettings->GetActiveCarbonLastResetDate();
        QDateTime lastResetDate = QDateTime::fromString(str);
        mp_Ui->labelResetCarbonFilterDate->setText(GetFormattedDateString(lastResetDate));
        m_UserSettingsTemp.SetActiveCarbonLastResetDate(str);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CServiceSettingsWidget::OnUserRoleChanged()
{
    ResetButtons();
}

void CServiceSettingsWidget::OnResetOperationDays()
{
    m_UserSettingsTemp.SetOperationHours(0);
    QString strDate = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
    m_UserSettingsTemp.SetOperationLastResetDate(strDate);

    QString dateStr = GetFormattedCurrentDateString();
    mp_Ui->labelResetOperationDate->setText(dateStr);
    emit ResetOperationDays(DataManager::RESETOPERATIONHOURS_WHOLEMACHINEOPERATION);
}

void CServiceSettingsWidget::OnResetCarbonFilter()
{
    QString strDate = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
    m_UserSettingsTemp.SetActiveCarbonLastResetDate(strDate);
    QString dateStr = GetFormattedCurrentDateString();
    mp_Ui->labelResetCarbonFilterDate->setText(dateStr);
    emit ResetOperationDays(DataManager::RESETOPERATIONHOURS_ACTIVECARBONFILTER);
}

void CServiceSettingsWidget::MaintainanceTimecountStart()
{
    QString strCurDate = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
    mp_UserSettings->SetActiveCarbonLastResetDate(strCurDate);
    mp_UserSettings->SetOperationLastResetDate(strCurDate);


    m_UserSettingsTemp.SetActiveCarbonLastResetDate(strCurDate);
    m_UserSettingsTemp.SetOperationLastResetDate(strCurDate);

    QString dateStr = GetFormattedCurrentDateString();
    mp_Ui->labelResetCarbonFilterDate->setText(dateStr);
    mp_Ui->labelResetOperationDate->setText(dateStr);
}

void CServiceSettingsWidget::OnSaveSetting()
{
    emit ServiceSettingsChanged(m_UserSettingsTemp);
}

void CServiceSettingsWidget::OnPrepareShutdown()
{
    MainMenu::CMessageDlg ConfirmationMessageDlg;
    ConfirmationMessageDlg.SetTitle(CommonString::strInforMsg);
    int paraffinMeltingPoint = mp_UserSettings->GetTemperatureParaffinBath();
    if (paraffinMeltingPoint <= 64)
    {
        ConfirmationMessageDlg.SetText(m_strShutdownConfirm12Hrs.arg("12"));
    }
    else
        ConfirmationMessageDlg.SetText(m_strShutdownConfirm12Hrs.arg("15"));

    ConfirmationMessageDlg.SetIcon(QMessageBox::Warning);
    ConfirmationMessageDlg.SetButtonText(1, CommonString::strYes);//right
    ConfirmationMessageDlg.SetButtonText(3, CommonString::strCancel);//left
    ConfirmationMessageDlg.HideCenterButton();
    if (ConfirmationMessageDlg.exec())
    {
        //send command to scheduler to shut Down
        emit AppQuitSystemPrepareShutdown(DataManager::QUITAPPSHUTDOWNACTIONTYPE_PREPARESHUTDOWN);
    }
}

void CServiceSettingsWidget::OnStartServiceApp()
{
    // checkout udisk
    // mount u
    // check usb-key file
    // check content
    emit AppQuitSystemPrepareShutdown(DataManager::QUITAPPSHUTDOWNACTIONTYPE_QUITAPP);
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CServiceSettingsWidget::OnProcessStateChanged()
{
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CServiceSettingsWidget::ResetButtons()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if (((m_CurrentUserRole == MainMenu::CMainWindow::Admin) ||
         (m_CurrentUserRole == MainMenu::CMainWindow::Service)) &&
         (!m_ProcessRunning) && Core::CGlobalHelper::CheckIfCanEdit() == true)
    {
        //Edit Mode
        mp_Ui->btnResetCarbonFilter->setEnabled(true);
        mp_Ui->btnShutdown->setEnabled(true);
        if (m_CurrentUserRole == MainMenu::CMainWindow::Service)
        {
            mp_Ui->btnStartServiceApp->setEnabled(true);
            mp_Ui->btnResetOperationHour->setEnabled(true);
        }
        else
        {
            mp_Ui->btnStartServiceApp->setEnabled(false);
            mp_Ui->btnResetOperationHour->setEnabled(false);
        }
        mp_Ui->btnSave->setEnabled(true);
    }
    else {
        mp_Ui->btnResetOperationHour->setEnabled(false);
        mp_Ui->btnResetCarbonFilter->setEnabled(false);
        mp_Ui->btnShutdown->setEnabled(false);
        mp_Ui->btnStartServiceApp->setEnabled(false);
        mp_Ui->btnSave->setEnabled(false);
    }

    bool isError = Core::CGlobalHelper::GetSystemErrorStatus();
    bool isBusy  = Core::CGlobalHelper::GetSystemBusyStatus();

    if (isError && !isBusy && (m_CurrentUserRole == MainMenu::CMainWindow::Service))
    {
        mp_Ui->btnStartServiceApp->setEnabled(true);
    }

    if (!m_ProcessRunning && !isError && !isBusy && ((m_CurrentUserRole == MainMenu::CMainWindow::Service) ||
                                (m_CurrentUserRole == MainMenu::CMainWindow::Admin)))
    {
        mp_Ui->btnShutdown->setEnabled(true);
    }
    else
        mp_Ui->btnShutdown->setEnabled(false);
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CServiceSettingsWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CServiceSettingsWidget", "Maintenance", Q_NULLPTR, -1));
   m_strShutdownConfirm12Hrs = QApplication::translate("Settings::CServiceSettingsWidget",
                        "If the instrument is shut down now, you may have to wait %1 hours to ensure the paraffin is melted before running the next program. Do you really want to shut down your instrument?",
                                           Q_NULLPTR, -1);
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *  \param p_MainWindow = the main window
 */
/****************************************************************************/
void CServiceSettingsWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
}

void CServiceSettingsWidget::SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings)
{
    m_UserSettingsTemp = *p_UserSettings;
    mp_UserSettings = p_UserSettings;
}

} // end namespace Settings
