/****************************************************************************/
/*! \file AlarmSettingsWidget.cpp
 *
 *  \brief AlarmSettingsWidget implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-20
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
#include "Settings/Include/AlarmSettingsWidget.h"
#include "ui_AlarmSettingsWidget.h"
#include "Core/Include/GlobalHelper.h"

namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CAlarmSettingsWidget::CAlarmSettingsWidget(QWidget *p_Parent) :
    MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CAlarmSettingsWidget),
    mp_DataConnector(NULL)
{
    mp_Ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("Alarm"));

    mp_Information =
            new Settings::CAlarmSettingsDlg(Settings::CAlarmSettingsDlg::Information, this);
    mp_Information->setModal(true);

    mp_Error =
            new Settings::CAlarmSettingsDlg(Settings::CAlarmSettingsDlg::Error, this);
    mp_Error->setModal(true);

    mp_Warning =
            new Settings::CAlarmSettingsDlg(Settings::CAlarmSettingsDlg::Warning, this);
    mp_Warning->setModal(true);

    CONNECTSIGNALSLOT(mp_Ui->informationButton, clicked(), this, OnNoteEdit());
    CONNECTSIGNALSLOT(mp_Ui->warningButton, clicked(), this, OnWarningEdit());
    CONNECTSIGNALSLOT(mp_Ui->errorButton, clicked(), this, OnErrorEdit());
    CONNECTSIGNALSLOT(mp_Error,AlarmSettingsChanged(DataManager::CUserSettings &), this , AlarmSettingsChange(DataManager::CUserSettings &));
    CONNECTSIGNALSLOT(mp_Information, AlarmSettingsChanged(DataManager::CUserSettings &), this , AlarmSettingsChange(DataManager::CUserSettings &));
    CONNECTSIGNALSLOT(mp_Warning, AlarmSettingsChanged(DataManager::CUserSettings &), this , AlarmSettingsChange(DataManager::CUserSettings &));
    CONNECTSIGNALSIGNAL(mp_Error, PlayTestTone(quint8, quint8, Global::AlarmType ), this, PlayTestTone(quint8, quint8, Global::AlarmType ));
    CONNECTSIGNALSIGNAL(mp_Information, PlayTestTone(quint8, quint8, Global::AlarmType ), this, PlayTestTone(quint8, quint8, Global::AlarmType ));
    CONNECTSIGNALSIGNAL(mp_Warning, PlayTestTone(quint8, quint8, Global::AlarmType ), this, PlayTestTone(quint8, quint8, Global::AlarmType ));
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CAlarmSettingsWidget::~CAlarmSettingsWidget()
{
    try {
        delete mp_Error;
        delete mp_Information;
        delete mp_Warning;
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint.
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CAlarmSettingsWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
         //   mp_Ui->retranslateUi(this);
            RetranslateUI();
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CAlarmSettingsWidget::SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings)
{
    m_UserSettings = *p_UserSettings;
    mp_Information->SetUserSettings(&m_UserSettings);
    mp_Error->SetUserSettings(&m_UserSettings);
    mp_Warning->SetUserSettings(&m_UserSettings);
    UpdateLabels();
}

/****************************************************************************/
/*!
 *  \brief Updates the content of this widget
 */
/****************************************************************************/
void CAlarmSettingsWidget::UpdateLabels()
{
    QString TextSound;
    QString TextVolume;
    QString TextPeriodic;

    // information
    TextSound = m_TextSound.arg(m_UserSettings.GetSoundNumberInformation());
    TextVolume = m_TextVolume.arg(m_UserSettings.GetSoundLevelInformation());
    if (m_UserSettings.GetSoundPeriodicInformation())
    {
        int PeriodicTime = m_UserSettings.GetSoundPeriodicTimeInformation();
        QString Minutes = QString("%1").arg(PeriodicTime/60, 2, 10, QChar('0'));
        TextPeriodic = m_TextPeriodicTime.arg(Minutes);
    }
    else
    {
        TextPeriodic = m_TextPeriodicOff;
    }

    mp_Ui->informationSound->setText(TextSound);
    mp_Ui->informationVolume->setText(TextVolume);
    mp_Ui->informationperiodic->setText(TextPeriodic);

    // warning
    TextSound = m_TextSound.arg(m_UserSettings.GetSoundNumberWarning());
    TextVolume = m_TextVolume.arg(m_UserSettings.GetSoundLevelWarning());
    if (m_UserSettings.GetSoundPeriodicWarning())
    {
        int PeriodicTime = m_UserSettings.GetSoundPeriodicTimeWarning();
        QString Minutes = QString("%1").arg(PeriodicTime/60, 2, 10, QChar('0'));
        TextPeriodic = m_TextPeriodicTime.arg(Minutes);
    }
    else
    {
        TextPeriodic = m_TextPeriodicOff;
    }
    mp_Ui->warningSound->setText(TextSound);
    mp_Ui->warningVolume->setText(TextVolume);
    mp_Ui->warningperiodic->setText(TextPeriodic);

    // error
    TextSound = m_TextSound.arg(m_UserSettings.GetSoundNumberError());
    TextVolume = m_TextVolume.arg(m_UserSettings.GetSoundLevelError());
    int PeriodicTime = m_UserSettings.GetSoundPeriodicTimeError();
    QString Minutes = QString("%1").arg(PeriodicTime/60, 2, 10, QChar('0'));
    TextPeriodic = m_TextPeriodicTime.arg(Minutes);

    mp_Ui->errorSound->setText(TextSound);
    mp_Ui->errorVolume->setText(TextVolume);
}
/****************************************************************************/
/*!
 *  \brief Updates the widget content everytime it is displayed
 *
 *  \iparam p_Event = Show event
 */
/****************************************************************************/
void CAlarmSettingsWidget::showEvent(QShowEvent *p_Event)
{
    if((p_Event != NULL) && !p_Event->spontaneous()) {
        UpdateLabels();
        ResetButtons();
    }
}

/****************************************************************************/
/*!
 *  \brief Opens the note alarm editing dialog
 */
/****************************************************************************/
void CAlarmSettingsWidget::OnNoteEdit()
{
    mp_Information->SetDialogType(CAlarmSettingsDlg ::Information);
    mp_Information->show();
    mp_Information->UpdateDisplay();
}
/****************************************************************************/
/*!
 *  \brief Opens the warning alarm editing dialog
 */
/****************************************************************************/
void CAlarmSettingsWidget::OnWarningEdit()
{
    mp_Warning->SetDialogType(CAlarmSettingsDlg :: Warning);
    mp_Warning->show();
    mp_Warning->UpdateDisplay();
}


/****************************************************************************/
/*!
 *  \brief Opens the error alarm editing dialog
 */
/****************************************************************************/
void CAlarmSettingsWidget::OnErrorEdit()
{  
     mp_Error->SetDialogType(CAlarmSettingsDlg ::Error);
     mp_Error->show();
     mp_Error->UpdateDisplay();
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CAlarmSettingsWidget::OnProcessStateChanged()
{
    ResetButtons();
}

void CAlarmSettingsWidget::OnUserRoleChanged()
{
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
    {
        mp_Ui->informationButton->setEnabled(false);
        mp_Ui->warningButton->setEnabled(false);
        mp_Ui->errorButton->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Alarm Setting changes
 *
 *  \param Settings = DataManager::CUserSettings type parameter
 */
/****************************************************************************/
void CAlarmSettingsWidget:: AlarmSettingsChange(DataManager::CUserSettings &us)
{
    emit AlarmSettingsChanged(us);
}
/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CAlarmSettingsWidget::ResetButtons()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    bool bCanEdit = Core::CGlobalHelper::CheckIfCanEdit();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) && (!m_ProcessRunning) && bCanEdit) {
        //Edit Mode
        mp_Ui->informationButton->setEnabled(true);
        mp_Ui->warningButton->setEnabled(true);
        mp_Ui->errorButton->setEnabled(true);

    }
    else {
        mp_Ui->informationButton->setEnabled(false);
        mp_Ui->warningButton->setEnabled(false);
        mp_Ui->errorButton->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CAlarmSettingsWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CAlarmSettingsWidget", "Alarm", Q_NULLPTR, -1));
    mp_Ui->groupBox->setTitle(QApplication::translate("Settings::CAlarmSettingsWidget", "Information", Q_NULLPTR, -1));
    mp_Ui->informationButton->setText(QApplication::translate("Settings::CAlarmSettingsWidget", "Edit", Q_NULLPTR, -1));
    mp_Ui->groupBox_1->setTitle(QApplication::translate("Settings::CAlarmSettingsWidget", "Warning", Q_NULLPTR, -1));
    mp_Ui->warningButton->setText(QApplication::translate("Settings::CAlarmSettingsWidget", "Edit", Q_NULLPTR, -1));
    mp_Ui->groupBox_2->setTitle(QApplication::translate("Settings::CAlarmSettingsWidget", "Error", Q_NULLPTR, -1));
    mp_Ui->errorButton->setText(QApplication::translate("Settings::CAlarmSettingsWidget", "Edit", Q_NULLPTR, -1));
    // texts to be displayed
    m_TextSound = QApplication::translate("Settings::CAlarmSettingsWidget",
                                          "Sound: %1", 0, -1);
    m_TextVolume = QApplication::translate("Settings::CAlarmSettingsWidget",
                                           "Volume: %1", 0, -1);
    m_TextPeriodicOff =
            QApplication::translate("Settings::CAlarmSettingsWidget",
                                    "Periodic: Off", 0, -1);
    m_TextPeriodicTime =
            QApplication::translate("Settings::CAlarmSettingsWidget",
                                    "Periodic time (Min.): %1", 0, -1);
}

void CAlarmSettingsWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());

    mp_Information->SetPtrToMainWindow(mp_MainWindow);
    mp_Error->SetPtrToMainWindow(mp_MainWindow);
    mp_Warning->SetPtrToMainWindow(mp_MainWindow);
}

} // end namespace Settings
