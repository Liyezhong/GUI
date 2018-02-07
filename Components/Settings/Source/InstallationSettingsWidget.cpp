/****************************************************************************/
/*! \file InstallationSettingsWidget.cpp
 *
 *  \brief OvenSettingsWidget implementation.
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
#include "Settings/Include/InstallationSettingsWidget.h"
#include "ui_InstallationSettingsWidget.h"
#include <Global/Include/SystemPaths.h>

namespace Settings {


/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CInstallationSettingsWidget::CInstallationSettingsWidget(QWidget *p_Parent) : MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CInstallationSettingsWidget), mp_KeyBoardWidget(NULL),
    mp_UserSettings(NULL),
    mp_DeviceConfig(NULL),
    mp_SWVersion(NULL),
    mp_MainWindow(NULL), m_ProcessRunning(false),
    m_CurrentUserRole(MainMenu::CMainWindow::Operator)
{
    mp_Ui->setupUi(GetContentFrame());
    mp_Ui->btnEdit->setText(tr("Edit"));
    mp_Ui->btnEdit->hide();
    mp_Ui->driverversion->hide();
    SetPanelTitle(tr("Installation"));
    mp_Ui->serialnumber->setText("0000-0000-0000-0000");
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");
    CONNECTSIGNALSLOT(mp_Ui->btnEdit,clicked(),this,OnEditclicked());
}


/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CInstallationSettingsWidget::~CInstallationSettingsWidget()
{
    try {
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
void CInstallationSettingsWidget::changeEvent(QEvent *p_Event)
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
void CInstallationSettingsWidget::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event)
    ResetButtons();
    RefreshSetting();
}
/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CInstallationSettingsWidget::ResetButtons()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();

    if (!m_ProcessRunning &&
        (MainMenu::CMainWindow::Service == m_CurrentUserRole))
    {
        mp_Ui->btnEdit->setEnabled(true);
    }
    else if (!m_ProcessRunning &&
             MainMenu::CMainWindow::Admin == m_CurrentUserRole)
    {
        mp_Ui->btnEdit->setEnabled(true);
    }
    else
    {
        mp_Ui->btnEdit->setEnabled(false);
    }
}

void CInstallationSettingsWidget::RefreshSetting()
{
    QString name = mp_DeviceConfig->GetValue("DEVICENAME");
    mp_Ui->instrumentname->setText(name);

    QString serial = mp_DeviceConfig->GetValue("SERIALNUMBER");
    mp_Ui->serialnumber->setText(serial);

    QString softVer = m_strSoftVer + mp_SWVersion->GetSWReleaseVersion();
    mp_Ui->softwareversion->setText(softVer);
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CInstallationSettingsWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CInstallationSettingsWidget", "Installation", Q_NULLPTR, -1));
   mp_Ui->btnEdit->setText(QApplication::translate(
                               "Settings::CInstallationSettingsWidget",
                               "Edit", Q_NULLPTR, -1));

   m_strSoftVer = QApplication::translate( "Settings::CInstallationSettingsWidget", "SoftwareVersion: ", Q_NULLPTR, -1);
   mp_Ui->driverversion->setText(QApplication::translate(
                                     "Settings::CInstallationSettingsWidget",
                                     "Driver Version : ",
                                     Q_NULLPTR, -1));
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to Usersetting,
 *
 */
/****************************************************************************/

void CInstallationSettingsWidget::SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings)
{
    mp_UserSettings = p_UserSettings;
}

void CInstallationSettingsWidget::SetDeviceConfig(DataManager::CDeviceConfiguration *p_DeviceConfig)
{
    mp_DeviceConfig = p_DeviceConfig;
}

void CInstallationSettingsWidget::SetSWConfig(DataManager::CSWVersionList *p_SWVersion)
{
    mp_SWVersion = p_SWVersion;

    if (mp_SWVersion != NULL) {
        QString FilenameSWVersion = Global::SystemPaths::Instance().GetSettingsPath() + "/SW_Version.xml";
        (void)mp_SWVersion->Read(FilenameSWVersion);
    }
}


void CInstallationSettingsWidget::SetKeyBoardInstance(KeyBoard::CKeyBoard *p_KeyBoard)
{
    mp_KeyBoardWidget = p_KeyBoard;
}
/****************************************************************************/
/*!
 *  \brief Connects signals and slots of keyboard.
 */
/****************************************************************************/
void CInstallationSettingsWidget::ConnectKeyBoardSignalSlots()
{
    if (mp_KeyBoardWidget) {
        // Connect signals and slots to keyboard.
        CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, OkButtonClicked(QString), this, OnOkClicked(QString));
        CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, EscButtonClicked(), this, OnESCClicked());
    }
}

/****************************************************************************/
/*!
 *  \brief Disconnects signals and slots of keyboard.
 */
/****************************************************************************/
void CInstallationSettingsWidget::DisconnectKeyBoardSignalSlots()
{
    if (mp_KeyBoardWidget) {
        // Disconnect signals and slots connected to keyboard.
        (void) disconnect(mp_KeyBoardWidget, SIGNAL(OkButtonClicked(QString)),
                          this, SLOT(OnOkClicked(QString)));
        (void) disconnect(mp_KeyBoardWidget, SIGNAL(EscButtonClicked()),
                          this, SLOT(OnESCClicked()));
    }

}


/****************************************************************************/
/*!
 *  \brief This slot is called when ESC button on Keyboard is pressed.
 */
/****************************************************************************/
void CInstallationSettingsWidget::OnESCClicked()
{
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief this function is called when keyboard ok clickeds
 *
 *  \param EnteredText = QString type parameter
 */
/****************************************************************************/
void CInstallationSettingsWidget::OnOkClicked(QString EnteredText)
{
    if (!mp_KeyBoardWidget)
        return;

    mp_KeyBoardWidget->hide();
    mp_Ui->instrumentname->setText(EnteredText);
    mp_DeviceConfig->SetValue("DEVICENAME", EnteredText);
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *   \param p_MainWindow = application's main window
 */
/****************************************************************************/
void CInstallationSettingsWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
}

/****************************************************************************/
/*!
 *  \brief This Slot will be called when edit button is clicked.
 */
/****************************************************************************/
void CInstallationSettingsWidget::OnEditclicked()
{
    if (!mp_KeyBoardWidget)
        return;

    mp_KeyBoardWidget->SetKeyBoardDialogTitle(tr("Enter Instrument Name"));

    if (mp_Ui->instrumentname->text().isEmpty())
    {
        mp_KeyBoardWidget->SetLineEditContent(tr("Instrument Name"));
    }
    else
    {
        mp_KeyBoardWidget->SetLineEditContent(mp_Ui->instrumentname->text());
    }

    mp_KeyBoardWidget->SetPasswordMode(false);
    mp_KeyBoardWidget->SetMaxCharLength(32);
    mp_KeyBoardWidget->SetMinCharLength(2);
    mp_KeyBoardWidget->show();

    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();
}

} // end namespace Settings
