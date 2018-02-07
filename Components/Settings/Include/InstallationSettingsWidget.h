/****************************************************************************/
/*! \file InstallationSettingsWidget.h
 *
 *  \brief OvenSettingsWidget definition.
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

#ifndef SETTINGS_INSTALLATIONSETTINGSWIDGET_H
#define SETTINGS_INSTALLATIONSETTINGSWIDGET_H

#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfiguration.h"
#include "DataManager/Containers/SWVersions/Include/SWVersionList.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/MainWindow.h"


namespace Settings {

namespace Ui {
    class CInstallationSettingsWidget;
}

/****************************************************************************/
/**
 * \brief This class displays all settings related to the oven of Himalaya
 */
/****************************************************************************/
class CInstallationSettingsWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

    friend class CTestSettings;
private:
    Ui::CInstallationSettingsWidget *mp_Ui;                 //!< User interface
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;         //!< Reference to Keyboard widget
    DataManager::CHimalayaUserSettings *mp_UserSettings;    //!< Data object
    DataManager::CDeviceConfiguration *mp_DeviceConfig;    //!< Device configuration object
    DataManager::CSWVersionList *mp_SWVersion;          //!< Software version object
    DataManager::CHimalayaUserSettings m_UserSettingsTemp;  //!< Temporary copy of User Settings
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    //Flags
    bool m_ProcessRunning;                          //!< Process running state
    //UI related
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    void ConnectKeyBoardSignalSlots();
    void DisconnectKeyBoardSignalSlots();
    QString m_strSoftVer;       //!< Software version string
public:
    explicit CInstallationSettingsWidget(QWidget *p_Parent = NULL);
    ~CInstallationSettingsWidget();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of SetUserSettings
     *
     *  \param p_UserSettings = current user's setting
     *
     */
    /****************************************************************************/
    void SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of SetDeviceConfig
     *
     *  \param p_DeviceConfig = the device config
     *
     */
    /****************************************************************************/
    void SetDeviceConfig(DataManager::CDeviceConfiguration *p_DeviceConfig);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of SetSWConfig, set version information
     *
     *  \param p_SWVersion = the software version
     *
     */
    /****************************************************************************/
    void SetSWConfig(DataManager::CSWVersionList *p_SWVersion);
    /****************************************************************************/
    /*!
     *  \brief Used to set pointer to mainwindow, used to retreive user role and
     *         process state changed.
     *   \param p_MainWindow = application's main window
     */
    /****************************************************************************/
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    /****************************************************************************/
    /*!
     *  \brief Sets KeyBoard object instance
     *  \param p_KeyBoard = the current keyboard instance
     */
    /****************************************************************************/
    void SetKeyBoardInstance(KeyBoard::CKeyBoard *p_KeyBoard);


private:
    void RetranslateUI();
    void ResetButtons();
    void RefreshSetting();


protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *p_Event);

 private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnEditclicked
     */
    /****************************************************************************/
    void OnEditclicked();
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

};

} // end namespace Settings

#endif // SETTINGS_OVENSETTINGSWIDGET_H

