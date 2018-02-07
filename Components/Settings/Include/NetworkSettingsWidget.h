/****************************************************************************/
/*! \file NetworkSettingsWidget.h
 *
 *  \brief NetworkSettingsWidget definition.
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

#ifndef SETTINGS_NETWORKSETTINGSWIDGET_H
#define SETTINGS_NETWORKSETTINGSWIDGET_H

#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/MessageDlg.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"

namespace Settings {

namespace Ui {
    class CNetworkSettingsWidget;
}

/****************************************************************************/
/**
 * \brief Enum for Button Type -UserName, Password, IP Address or Port.
 */
/****************************************************************************/
typedef enum {
    USERNAME_BTN_CLICKED = 1,
    PASSWORD_BTN_CLICKED,
    IP_ADDRESS_BTN_CLICKED,
    PORT_BTN_CLICKED
} ButtonType_t;

/****************************************************************************/
/**
 * \brief Enum for network setting.
 */
/****************************************************************************/
typedef enum {
    IP_ADDRESS = 1, //!< ip address
    NETWORK_PORT    //!< network port
} SettingType_t;


/****************************************************************************/
/**
 * \brief This class contains the proxy settings for remote care
 */
/****************************************************************************/
class CNetworkSettingsWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

    friend class CTestSettings;
public:
    explicit CNetworkSettingsWidget(QWidget *p_Parent = 0);
    ~CNetworkSettingsWidget();
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    void SetKeyBoardInstance(KeyBoard::CKeyBoard *p_KeyBoard);

private:
    Ui::CNetworkSettingsWidget *mp_Ui;              //!< User interface    
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    //Flags
    bool m_ProcessRunning;                          //!< Process running state
    //UI related
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;         //!< Reference to Keyboard widget
    ButtonType_t m_ButtonType;                      //!< Button Type
    DataManager::CHimalayaUserSettings m_UserSettings;      //!< UserSettings object
    QString m_Password;                             //!< String to hold Password
    QString m_strEnterProxyName;       ///<  Definition/Declaration of variable m_strEnterProxyName
    QString m_strEnterProxyPassword;       ///<  Definition/Declaration of variable m_strEnterProxyPassword
    QString m_strEnterProxyIP;       ///<  Definition/Declaration of variable m_strEnterProxyIP
    QString m_strEnterProxyPort;       ///<  Definition/Declaration of variable m_strEnterProxyPort

    QString m_strErrIP; ///<  Definition/Declaration of variable m_strErrIP
    QString m_strErrPort; ///<  Definition/Declaration of variable m_strErrPort
    MainMenu::CMessageDlg *mp_MessageDlg; ///<  Definition/Declaration of variable mp_MessageDlg

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of InputValidator
     *  \iparam input = content need to be checked
     *  \iparam type = setting/content type
     */
    /****************************************************************************/
    bool validator(QString &input, SettingType_t type);

    void RetranslateUI();
    void ResetButtons();
    void ConnectKeyBoardSignalSlots();
    void DisconnectKeyBoardSignalSlots();
protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *p_Event);

public:
     void SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings);
     /****************************************************************************/
     /*!
      *  \brief  show the specified message
      *  \param msg = the message to be shown
      */
     /****************************************************************************/
     void showInformation(QString &msg);
private slots:    
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnOkClicked
      */
     /****************************************************************************/
     void OnOkClicked(QString EnteredText);
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnESCClicked
      */
     /****************************************************************************/
     void OnESCClicked();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnProcessStateChanged
      */
     /****************************************************************************/
     void OnProcessStateChanged();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnUserRoleChanged
      */
     /****************************************************************************/
     void OnUserRoleChanged();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnDirectConnectionStateChanged
      */
     /****************************************************************************/
     void OnDirectConnectionStateChanged(int State);
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnProxyUserName
      */
     /****************************************************************************/
     void OnProxyUserName();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnProxyPassword
      */
     /****************************************************************************/
     void OnProxyPassword();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnProxyIPAddress
      */
     /****************************************************************************/
     void OnProxyIPAddress();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnProxyPort
      */
     /****************************************************************************/
     void OnProxyPort();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnSave
      */
     /****************************************************************************/
     void OnSave();

signals:
     /****************************************************************************/
     /*!
      *  \brief  emit network settings changing
      *  \param  Settings = the user setting
      */
     /****************************************************************************/
     void SettingsChanged(DataManager::CUserSettings &Settings);
};

} // end namespace Settings

#endif // SETTINGS_NETWORKSETTINGSWIDGET_H

