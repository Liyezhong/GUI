/****************************************************************************/
/*! \file NetworkSettingsWidget.cpp
 *
 *  \brief NetworkSettingsWidget implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-26
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

#include "Settings/Include/NetworkSettingsWidget.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "Dashboard/Include/CommonString.h"
#include "ui_NetworkSettingsWidget.h"
#include "Core/Include/GlobalHelper.h"

namespace Settings {
//const QString REGEXP_NUMERIC_VALIDATOR = "^[0-9]*$"; //!< Reg expression for the validator
const QString REGEXP_NUMERIC_VALIDATOR = "^[0-9]{1,5}$"; //!< Reg expression for the validator
const QString IPADDRESS_INPUT_MASK_    = "000.000.000.000; "; //!< Mask for the Ip address

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/

CNetworkSettingsWidget::CNetworkSettingsWidget(QWidget *p_Parent) :
    MainMenu::CPanelFrame(p_Parent), mp_Ui(new Ui::CNetworkSettingsWidget),
    mp_MainWindow(NULL),m_ProcessRunning(false),
    m_CurrentUserRole(MainMenu::CMainWindow::Operator),
    mp_KeyBoardWidget(NULL),
    m_ButtonType(USERNAME_BTN_CLICKED),
    m_Password(""),
    m_strEnterProxyName(tr("Enter Proxy User Name")),
    m_strEnterProxyPassword(tr("Enter Proxy Password")),
    m_strEnterProxyIP(tr("Enter Proxy IP Address")),
    m_strEnterProxyPort(tr("Enter Proxy Port")),
    m_strErrIP(tr("IP address is not correct")),
    m_strErrPort(tr("Network port is not correct."))
{
    mp_Ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("Network"));
    mp_Ui->checkBox_RemoteCare->setChecked(true);

    mp_MessageDlg = NULL;

    CONNECTSIGNALSLOT(mp_Ui->checkBox_DirectConnection, stateChanged(int),
                      this, OnDirectConnectionStateChanged(int));
    CONNECTSIGNALSLOT(mp_Ui->proxyUserNameButton, clicked(), this, OnProxyUserName());
    CONNECTSIGNALSLOT(mp_Ui->proxyPasswordButton, clicked(), this, OnProxyPassword());
    CONNECTSIGNALSLOT(mp_Ui->proxyIpAddressButton, clicked(), this, OnProxyIPAddress());
    CONNECTSIGNALSLOT(mp_Ui->proxyPortButton, clicked(), this, OnProxyPort());
    CONNECTSIGNALSLOT(mp_Ui->saveButton, clicked(), this, OnSave());   
}

void CNetworkSettingsWidget::showInformation(QString &msg)
{
    mp_MessageDlg = new MainMenu::CMessageDlg;
    mp_MessageDlg->HideAllButtons();
    mp_MessageDlg->SetIcon(QMessageBox::Information);
    mp_MessageDlg->SetTitle(CommonString::strInforMsg);
    mp_MessageDlg->SetButtonText(2, CommonString::strOK);
    mp_MessageDlg->SetText(msg);
    (void)mp_MessageDlg->exec();
    delete mp_MessageDlg;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CNetworkSettingsWidget::~CNetworkSettingsWidget()
{
    delete mp_Ui;
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CNetworkSettingsWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        //Commented below line because it calls retranslateUi of ui_NetworkSettingsWidget.h
        // which sets the buttons(UserNameButton, Password, IPAddress and IP Port) text to default values.
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
 *  \iparam p_Event = Show event
 */
/****************************************************************************/
void CNetworkSettingsWidget::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnProcessStateChanged()
{
    ResetButtons();
}

void CNetworkSettingsWidget::OnUserRoleChanged()
{
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
    {
        mp_Ui->saveButton->setEnabled(false);
    }
}
/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CNetworkSettingsWidget::ResetButtons()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    bool bCanEdit = Core::CGlobalHelper::CheckIfCanEdit();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning) && bCanEdit) {
        //Edit Mode
        mp_Ui->saveButton->setEnabled(true);
    }
    else {
        mp_Ui->saveButton->setEnabled(false);
    }

    if (m_UserSettings.GetRemoteCare() == Global::ONOFFSTATE_ON) {
        mp_Ui->checkBox_RemoteCare->setChecked(true);
    }
    else {
        mp_Ui->checkBox_RemoteCare->setChecked(false);
    }
    if (m_UserSettings.GetDirectConnection() == Global::ONOFFSTATE_ON) {
        mp_Ui->checkBox_DirectConnection->setChecked(true);
    }
    else {
        mp_Ui->checkBox_DirectConnection->setChecked(false);
    }
    mp_Ui->proxyUserNameButton->setText(m_UserSettings.GetProxyUserName());
    m_Password = m_UserSettings.GetProxyPassword();
    int PasswordLength = m_UserSettings.GetProxyPassword().length();
    QString PasswordString;
    for (int i = 0; i < PasswordLength; i++) {
        PasswordString  = PasswordString + "*";
    }
    mp_Ui->proxyUserNameButton->setText(m_UserSettings.GetProxyUserName());
    mp_Ui->proxyPasswordButton->setText(PasswordString);
    mp_Ui->proxyIpAddressButton->setText(m_UserSettings.GetProxyIPAddress());
    mp_Ui->proxyPortButton->setText(QString::number(m_UserSettings.GetProxyIPPort()));
}

bool CNetworkSettingsWidget::validator(QString &input, SettingType_t type)
{
    switch (type)
    {
        case IP_ADDRESS:
        {
            // range: 000.000.000.001 to 255.255.255.255
            QStringList list = input.split('.');
            if (list.size() != 4) {
                return false;
            }

            bool ret = (list[3].compare("001") >= 0 && list[3].compare("255") <= 0);
            if (!ret) {
                return false;
            }

            QString val;
            for (int i = 0; i < 3; ++ i) {
                val = list[i];
                if (val.compare("000") >= 0 && val.compare("255") <= 0) {
                    continue;
                }
                else {
                    return false;
                }
            }
            return true;
        }

        case NETWORK_PORT:
        {
            // 0001 to 65535
            quint32 port = input.toUInt();
            qDebug() << "input port: " << port;
            if (port >= 1 && port <= 65535) {
                return true;
            }
            else {
                return false;
            }
        }
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CNetworkSettingsWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CNetworkSettingsWidget", "Network", Q_NULLPTR, -1));
    mp_Ui->checkBox_RemoteCare->setText(QApplication::translate("Settings::CNetworkSettingsWidget", "Enable RemoteCare Events", Q_NULLPTR, -1));
    mp_Ui->checkBox_DirectConnection->setText(QApplication::translate("Settings::CNetworkSettingsWidget", "Direct Connection", Q_NULLPTR, -1));
    mp_Ui->proxyUserNameLabel->setText(QApplication::translate("Settings::CNetworkSettingsWidget", "Proxy User Name", Q_NULLPTR, -1));
    mp_Ui->proxyPasswordLabel->setText(QApplication::translate("Settings::CNetworkSettingsWidget", "Proxy Password", Q_NULLPTR, -1));
    mp_Ui->proxyIpAddressLabel->setText(QApplication::translate("Settings::CNetworkSettingsWidget", "Proxy IP Address", Q_NULLPTR, -1));
    mp_Ui->proxyPortLabel->setText(QApplication::translate("Settings::CNetworkSettingsWidget", "Proxy Port", Q_NULLPTR, -1));
    mp_Ui->saveButton->setText(QApplication::translate("Settings::CNetworkSettingsWidget", "Save", Q_NULLPTR, -1));
    m_strEnterProxyName = QApplication::translate("Settings::CNetworkSettingsWidget", "Enter Proxy User Name", Q_NULLPTR, -1);
    m_strEnterProxyPassword = QApplication::translate("Settings::CNetworkSettingsWidget", "Enter Proxy Password", Q_NULLPTR, -1);
    m_strEnterProxyIP = QApplication::translate("Settings::CNetworkSettingsWidget", "Enter Proxy IP Address", Q_NULLPTR, -1);
    m_strEnterProxyPort = QApplication::translate("Settings::CNetworkSettingsWidget", "Enter Proxy Port", Q_NULLPTR, -1);
    m_strErrIP = QApplication::translate("Settings::CNetworkSettingsWidget", "IP address is not correct", Q_NULLPTR, -1);
    m_strErrPort = QApplication::translate("Settings::CNetworkSettingsWidget", "Network port is not correct.", Q_NULLPTR, -1);
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *  \param p_MainWindow = the main window
 */
/****************************************************************************/
void CNetworkSettingsWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
}

/****************************************************************************/
/*!
 *  \brief This slot is called when DirectConnection checkBox state is changed.
 *
 *  \param State = int type parameter
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnDirectConnectionStateChanged(int State)
{
    if (State == (int)Qt::Checked) {
        mp_Ui->proxyUserNameButton->setDisabled(true);
        mp_Ui->proxyPasswordButton->setDisabled(true);
        mp_Ui->proxyIpAddressButton->setDisabled(true);
        mp_Ui->proxyPortButton->setDisabled(true);
    }
    else {
        mp_Ui->proxyUserNameButton->setDisabled(false);
        mp_Ui->proxyPasswordButton->setDisabled(false);
        mp_Ui->proxyIpAddressButton->setDisabled(false);
        mp_Ui->proxyPortButton->setDisabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to Enter Proxy UserName
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnProxyUserName()
{
    if (!mp_KeyBoardWidget)
        return;

    mp_KeyBoardWidget->SetLineEditInputMask("");
    mp_KeyBoardWidget->GetLineEditPtr()->clear();

    m_ButtonType = USERNAME_BTN_CLICKED;
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(m_strEnterProxyName);
    mp_KeyBoardWidget->SetPasswordMode(false);
    //! \note Why is the replace done before setting text ?
    //! Unfortunately, QAbstractPushButton's uses "&" for shortcut key. Hence entering "&&"
    //! would result in "&" only. Hence the below hack!
    mp_KeyBoardWidget->SetLineEditContent(mp_Ui->proxyUserNameButton->text().replace("&&", "&"));
    mp_KeyBoardWidget->SetMaxCharLength(16);
    mp_KeyBoardWidget->SetMinCharLength(1);
    mp_KeyBoardWidget->show();
    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to Enter Proxy Password
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnProxyPassword()
{
    if (!mp_KeyBoardWidget)
        return;

    mp_KeyBoardWidget->SetLineEditInputMask("");
    mp_KeyBoardWidget->GetLineEditPtr()->clear();


    m_ButtonType = PASSWORD_BTN_CLICKED;
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(m_strEnterProxyPassword);
    mp_KeyBoardWidget->SetPasswordMode(true);
    mp_KeyBoardWidget->SetMaxCharLength(16);
    mp_KeyBoardWidget->SetMinCharLength(4);
    mp_KeyBoardWidget->DisplayNumericKeyBoard();
    mp_KeyBoardWidget->show();
    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to Enter Proxy IP Address
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnProxyIPAddress()
{
    if (!mp_KeyBoardWidget)
        return;
    mp_KeyBoardWidget->SetLineEditInputMask("");
    mp_KeyBoardWidget->GetLineEditPtr()->clear();


    m_ButtonType = IP_ADDRESS_BTN_CLICKED;
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(m_strEnterProxyIP);
    mp_KeyBoardWidget->SetPasswordMode(false);
    mp_KeyBoardWidget->SetLineEditContent(mp_Ui->proxyIpAddressButton->text());
    mp_KeyBoardWidget->SetMaxCharLength(32);
    mp_KeyBoardWidget->SetMinCharLength(2);
    // enable the input mask so that user can easily fill the ip address
    mp_KeyBoardWidget->SetLineEditInputMask(IPADDRESS_INPUT_MASK_);
    mp_KeyBoardWidget->DisplayNumericKeyBoard();
    mp_KeyBoardWidget->show();
    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to Enter Proxy Port number
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnProxyPort()
{
    if (!mp_KeyBoardWidget)
        return;
    mp_KeyBoardWidget->SetLineEditInputMask("");
    mp_KeyBoardWidget->GetLineEditPtr()->clear();


    m_ButtonType = PORT_BTN_CLICKED;
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(m_strEnterProxyPort);
    mp_KeyBoardWidget->SetPasswordMode(false);
    mp_KeyBoardWidget->SetLineEditContent(mp_Ui->proxyPortButton->text());
    mp_KeyBoardWidget->SetMaxCharLength(5);
    mp_KeyBoardWidget->SetMinCharLength(2);
    // ^ and $ is used for any character. * is used to enter multiple characters
    // [0-9] is used to allow user to enter only 0 to 9 digits
    mp_KeyBoardWidget->SetLineEditValidatorExpression(REGEXP_NUMERIC_VALIDATOR);
    mp_KeyBoardWidget->DisplayNumericKeyBoard();
    mp_KeyBoardWidget->SetLineEditSelected();
    mp_KeyBoardWidget->show();
    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Sets KeyBoard object instance
 *  \param p_KeyBoard = pointer to keyboard instance
 */
/****************************************************************************/
void CNetworkSettingsWidget::SetKeyBoardInstance(KeyBoard::CKeyBoard *p_KeyBoard)
{
    mp_KeyBoardWidget = p_KeyBoard;
}

/****************************************************************************/
/*!
 *  \brief Connects signals and slots of keyboard.
 */
/****************************************************************************/
void CNetworkSettingsWidget::ConnectKeyBoardSignalSlots()
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
void CNetworkSettingsWidget::DisconnectKeyBoardSignalSlots()
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
void CNetworkSettingsWidget::OnESCClicked()
{
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
}


/****************************************************************************/
/*!
 *  \brief This slot is called when Ok button on KeyBoard is pressed.
 *
 *  \param EnteredText = QString type parameter
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnOkClicked(QString EnteredText)
{
    if (!mp_KeyBoardWidget)
        return;
    QString PasswordString;
    QString EnteredString;
    EnteredString  = mp_KeyBoardWidget->GetLineEditString();
    int Length = EnteredText.length();

    switch(m_ButtonType) {

    case USERNAME_BTN_CLICKED:
        mp_Ui->proxyUserNameButton->setText(EnteredText);
        break;

    case PASSWORD_BTN_CLICKED:
        m_Password = EnteredText;
        for(int i = 0; i < Length; i++) {
            qDebug()<<"i = "<< i;
            PasswordString  = PasswordString + "*";
        }
        mp_Ui->proxyPasswordButton->setText(PasswordString);
        break;

    case IP_ADDRESS_BTN_CLICKED:
        if (!validator(EnteredString, IP_ADDRESS)) {
            showInformation(m_strErrIP);
            break;
        }
        mp_Ui->proxyIpAddressButton->setText(EnteredText);
        break;

    case PORT_BTN_CLICKED:
        if (!validator(EnteredString, NETWORK_PORT)) {
            showInformation(m_strErrPort);
            break;
        }
        mp_Ui->proxyPortButton->setText(EnteredText);
        break;
    default:
        break;
    }
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CNetworkSettingsWidget::SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings)
{
    m_UserSettings = *p_UserSettings;
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Save button is clicked.
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnSave()
{
    if (mp_Ui->checkBox_RemoteCare->isChecked() == true) {
        m_UserSettings.SetRemoteCare(Global::ONOFFSTATE_ON);
    }
    else {
         m_UserSettings.SetRemoteCare(Global::ONOFFSTATE_OFF);
    }

    if (mp_Ui->checkBox_DirectConnection->isChecked() == true) {
        m_UserSettings.SetDirectConnection(Global::ONOFFSTATE_ON);
    }
    else {
        m_UserSettings.SetDirectConnection(Global::ONOFFSTATE_OFF);
    }
    m_UserSettings.SetProxyUserName(mp_Ui->proxyUserNameButton->text());
    m_UserSettings.SetProxyPassword(m_Password);
    m_UserSettings.SetProxyIPAddress(mp_Ui->proxyIpAddressButton->text());
    m_UserSettings.SetProxyIPPort(mp_Ui->proxyPortButton->text().toInt());
    emit SettingsChanged(m_UserSettings);
}

} // end namespace Settings
