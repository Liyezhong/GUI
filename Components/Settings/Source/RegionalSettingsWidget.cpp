/****************************************************************************/
/*! \file RegionalSettingsWidget.cpp
 *
 *  \brief RegionalSettingsWidget implementation.
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "MainMenu/Include/SliderControl.h"
#include "MainMenu/Include/MainWindow.h"
#include "Settings/Include/RegionalSettingsWidget.h"
#include "ui_RegionalSettingsWidget.h"
#include "Core/Include/GlobalHelper.h"

namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CRegionalSettingsWidget::CRegionalSettingsWidget(QWidget *p_Parent) :
    MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CRegionalSettingsWidget),
    mp_UserSettings(NULL)
{
    mp_Ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("Regional Settings"));
    translator= new QTranslator(this);
    CONNECTSIGNALSLOT(mp_Ui->btnApply, clicked(), this, OnApply());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CRegionalSettingsWidget::~CRegionalSettingsWidget()
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
void CRegionalSettingsWidget::changeEvent(QEvent *p_Event)
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

/****************************************************************************/
/*!
 *  \brief Updates the widget content everytime it is displayed
 *
 *  \iparam p_Event = Show event
 */
/****************************************************************************/
void CRegionalSettingsWidget::showEvent(QShowEvent *p_Event)
{
    if((mp_UserSettings != NULL) && (p_Event != NULL) && !p_Event->spontaneous()) {
        if(mp_UserSettings->GetTemperatureFormat() == Global::TEMP_FORMAT_CELSIUS) {
            mp_Ui->temperatureSlider->SetPosition(MainMenu::CSliderControl::PosLeft);
        }
        else {
            mp_Ui->temperatureSlider->SetPosition(MainMenu::CSliderControl::PosRight);
        }

        if(mp_UserSettings->GetTimeFormat() == Global::TIME_24) {
            mp_Ui->timeSlider->SetPosition(MainMenu::CSliderControl::PosLeft);
        }
        else {
            mp_Ui->timeSlider->SetPosition(MainMenu::CSliderControl::PosRight);
        }

        if(mp_UserSettings->GetDateFormat() == Global::DATE_INTERNATIONAL) {
            mp_Ui->radioInternational->setChecked(true);
        }
        else if (mp_UserSettings->GetDateFormat() == Global::DATE_ISO) {
            mp_Ui->radioIso->setChecked(true);
        }
        else {
            mp_Ui->radioUs->setChecked(true);
        }
        ResetButtons();
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CRegionalSettingsWidget::SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings)
{
    mp_UserSettings = p_UserSettings;
}

/****************************************************************************/
/*!
 *  \brief Called when the apply button is clicked
 */
/****************************************************************************/
void CRegionalSettingsWidget::OnApply()
{
    if (mp_UserSettings)
    {
        m_UserSettingsTemp = *mp_UserSettings;
    }

    if(mp_Ui->radioInternational->isChecked()) {
        m_UserSettingsTemp.SetDateFormat(Global::DATE_INTERNATIONAL);
    }
    else if (mp_Ui->radioIso->isChecked()){
        m_UserSettingsTemp.SetDateFormat(Global::DATE_ISO);
    }
    else if (mp_Ui->radioUs->isChecked()) {
        m_UserSettingsTemp.SetDateFormat(Global::DATE_US);
    }

    if (mp_Ui->timeSlider->GetPosition() == MainMenu::CSliderControl::PosLeft) {
        m_UserSettingsTemp.SetTimeFormat(Global::TIME_24);
    }
    else {
        m_UserSettingsTemp.SetTimeFormat(Global::TIME_12);
    }

    if (mp_Ui->temperatureSlider->GetPosition() == MainMenu::CSliderControl::PosLeft) {
        m_UserSettingsTemp.SetTemperatureFormat(Global::TEMP_FORMAT_CELSIUS);
    }
    else {
        m_UserSettingsTemp.SetTemperatureFormat(Global::TEMP_FORMAT_FAHRENHEIT);
    }

    if (m_UserSettingsTemp == *mp_UserSettings )
        return ;

    emit RegionalSettingsChanged(m_UserSettingsTemp);
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CRegionalSettingsWidget::OnProcessStateChanged()
{
    ResetButtons();
}

void CRegionalSettingsWidget::OnUserRoleChanged()
{
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
    {
        mp_Ui->btnApply->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CRegionalSettingsWidget::ResetButtons()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) && (!m_ProcessRunning)) {
        if (Core::CGlobalHelper::CheckIfCanEdit() == false) {
            mp_Ui->btnApply->setEnabled(false);
        }
        else {
            //Edit Mode
            mp_Ui->btnApply->setEnabled(true);
        }
    }
    else {
        mp_Ui->btnApply->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CRegionalSettingsWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CRegionalSettingsWidget", "Regional Settings", Q_NULLPTR, -1));
}

void CRegionalSettingsWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
}

} // end namespace Settings
