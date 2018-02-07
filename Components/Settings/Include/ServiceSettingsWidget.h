/****************************************************************************/
/*! \file ServiceSettingsWidget.h
 *
 *  \brief ServiceSettingsWidget definition.
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

#ifndef SETTINGS_SERVICESETTINGSWIDGET_H
#define SETTINGS_SERVICESETTINGSWIDGET_H

#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace Settings {

namespace Ui {
    class CServiceSettingsWidget;
}

/****************************************************************************/
/**
 * \brief This widget contains a button to start the service software
 */
/****************************************************************************/
class CServiceSettingsWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

    friend class CTestSettings;

public:
    explicit CServiceSettingsWidget(QWidget *p_Parent = 0);
    ~CServiceSettingsWidget();
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetUserSettings
     *
     *  \param p_UserSettings = DataManager::CHimalayaUserSettings type parameter
     *
     *  \return from SetUserSettings
     */
    /****************************************************************************/
    void SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings);
private:
    Ui::CServiceSettingsWidget *mp_Ui;  //!< User interface
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    //Flags
    bool m_ProcessRunning;                  //!< Process running state
    //UI related
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    DataManager::CHimalayaUserSettings *mp_UserSettings;  //!< Data object
    DataManager::CHimalayaUserSettings m_UserSettingsTemp;  //!< Temporary copy of User Settings

    void RetranslateUI();
    void ResetButtons();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetFormattedCurrentDateString
     *
     *  \return from GetFormattedCurrentDateString
     */
    /****************************************************************************/
    QString GetFormattedCurrentDateString();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetFormattedDateString
     *
     *  \param dateTime =  QDateTime type parameter
     *
     *  \return from GetFormattedDateString
     */
    /****************************************************************************/
    QString GetFormattedDateString(const QDateTime& dateTime);
    QString m_strShutdownConfirm12Hrs;       ///<  Definition/Declaration of variable m_strShutdownConfirm12Hrs

protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *p_Event);

private slots:
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
     *  \brief  Definition/Declaration of OnResetOperationDays
     */
    /****************************************************************************/
    void OnResetOperationDays();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnResetCarbonFilter
     */
    /****************************************************************************/
    void OnResetCarbonFilter();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnSaveSetting
     */
    /****************************************************************************/
    void OnSaveSetting();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnPrepareShutdown
     */
    /****************************************************************************/
    void OnPrepareShutdown();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnStartServiceApp
     */
    /****************************************************************************/
    void OnStartServiceApp();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of MaintainanceTimecountStart
     */
    /****************************************************************************/
    void MaintainanceTimecountStart();
signals:
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal ServiceSettingsChanged, when change in service page
     *  emit this signal.
     *
     *  \param  Settings = the user setting
     */
    /****************************************************************************/
    void ServiceSettingsChanged(DataManager::CUserSettings &Settings);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ResetOperationDays
     *  \param  resetOperationHoursType =  the type for reset opeartion time
     */
    /****************************************************************************/
    void ResetOperationDays(DataManager::ResetOperationHoursType_t resetOperationHoursType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal AppQuitSystemPrepareShutdown
     *  \param  quitAppShutdownActionType = quit application or prepare shutdown
     */
    /****************************************************************************/
    void AppQuitSystemPrepareShutdown(DataManager::QuitAppShutdownActionType_t quitAppShutdownActionType);
};

} // end namespace Settings

#endif // SETTINGS_SERVICESETTINGSWIDGET_H

