/****************************************************************************/
/*! \file ParametersWidget.h
 *
 *  \brief ParametersWidget definition.
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

#ifndef SETTINGS_REGIONALSETTINGSWIDGET_H
#define SETTINGS_REGIONALSETTINGSWIDGET_H

#include "Global/Include/Commands/CmdDateAndTime.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/PanelFrame.h"
#include <QTranslator>

namespace MainMenu {
    class CMainWindow;
}

namespace Settings {

namespace Ui {
    class CRegionalSettingsWidget;
}

/****************************************************************************/
/**
 * \brief Widget for regional settings like temperature, date, or time format
 */
/****************************************************************************/
class CRegionalSettingsWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

    friend class CTestSettings;

private:
    Ui::CRegionalSettingsWidget *mp_Ui;             //!< User interface
    DataManager::CHimalayaUserSettings *mp_UserSettings;  //!< Data object
    DataManager::CHimalayaUserSettings m_UserSettingsTemp;  //!< Temporary copy of User Settings
    MainMenu::CMainWindow *mp_MainWindow;           //!< Used to inform Mainwindow settings.
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    Global::DateFormat m_DateFormat;                //!< Date format
    Global::TimeFormat m_TimeFormat;                //!< Time Format
    Global::TemperatureFormat m_TemperatureFormat;  //!< Temperature Format
    QTranslator *translator;       ///<  Definition/Declaration of variable translator
    //Flags
    bool m_ProcessRunning;                  //!< Process running state
    void RetranslateUI();
    void ResetButtons();

public:
    explicit CRegionalSettingsWidget(QWidget *p_Parent = 0);
    ~CRegionalSettingsWidget();
    void SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings);

    /****************************************************************************/
    /*!
     *  \brief Used to set pointer to mainwindow, used to retreive user role and
     *         process state changed.
     *  \param p_MainWindow = the main window
     */
    /****************************************************************************/
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);

protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *p_Event);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnApply
     */
    /****************************************************************************/
    void OnApply();
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
signals:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RegionalSettingsChanged
     *  \param  Settings = the user settings
     */
    /****************************************************************************/
    void RegionalSettingsChanged(DataManager::CUserSettings &Settings);
};

} // end namespace Settings

#endif // SETTINGS_REGIONALSETTINGSWIDGET_H

