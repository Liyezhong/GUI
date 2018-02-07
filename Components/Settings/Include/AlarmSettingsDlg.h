/****************************************************************************/
/*! \file AlarmSettingsDlg.h
 *
 *  \brief AlarmSettingsDlg definition.
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

#ifndef SETTINGS_ALARMSETTINGSDLG_H
#define SETTINGS_ALARMSETTINGSDLG_H

#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/SliderControl.h"
#include <QButtonGroup>

namespace Settings {

namespace Ui {
    class CAlarmSettingsDlg;
}

/****************************************************************************/
/**
 * \brief Dialog for changing the alarm settings
 */
/****************************************************************************/
class CAlarmSettingsDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT

    friend class CTestSettings;
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of enum AlarmDialogType
     */
    /****************************************************************************/
    typedef enum {
        Information = 0,
        Warning = 1,
        Error
    } AlarmDialogType;

    explicit CAlarmSettingsDlg(AlarmDialogType DialogType, QWidget *p_Parent = 0);

    CAlarmSettingsDlg();


    ~CAlarmSettingsDlg();
    void UpdateDisplay(void);
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    Global::AlarmType m_Type;       ///<  Definition/Declaration of variable m_Type

    /****************************************************************************/
    /*!
     *  \brief Sets pointer to UserSettings
     *
     *  \iparam p_UserSettings
     */
    /****************************************************************************/
    void SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings) { mp_UserSettings = p_UserSettings;}
    /****************************************************************************/
    /*!
     *  \brief Used by Alarm Settings widget to set the dialog type -warning/ error
     *
     *  \iparam DialogType = True - error, False - warning
     */
    /****************************************************************************/
    void SetDialogType(AlarmDialogType DialogType) { m_AlarmScreen = DialogType; }

private:
    Ui::CAlarmSettingsDlg *mp_Ui;   //!< User interface
    MainMenu::CScrollWheel *mp_VolumeScrollWheel; //!< Volume scroll wheel
    MainMenu::CScrollWheel *mp_SoundScrollWheel; //!< Volume scroll wheel
    MainMenu::CScrollWheel *mp_MinWheel;        //!< Minut scroll wheel
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role

    QButtonGroup m_ButtonGroup; //!< Groups radio buttons for sound selection
    DataManager::CHimalayaUserSettings *mp_UserSettings;    //!< Data object
    DataManager::CHimalayaUserSettings m_UserSettingsTemp;  //!< Temporary copy of User Settings
    //Flags
    bool m_ProcessRunning;  //!< Process running state
    AlarmDialogType m_AlarmScreen;  //!< Tells if the screen shown is error alarm setting.
    void RetranslateUI();
    void ResetButtons();

protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *p_Event);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnPeriodicChanged
     *  \param  Position = user's selection in UI.
     */
    /****************************************************************************/
    void OnPeriodicChanged(MainMenu::CSliderControl::Position_t Position);
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
     *  \brief  Definition/Declaration of OnApply
     */
    /****************************************************************************/
    void OnApply();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnPlayTone
     */
    /****************************************************************************/
    void OnPlayTone();

signals:
    /****************************************************************************/
    /*!
     *  \brief  signal for alarm setting changing
     *  \param  Settings = the user setting
     */
    /****************************************************************************/
    void AlarmSettingsChanged(DataManager::CUserSettings &Settings);
    /****************************************************************************/
    /*!
     *  \brief  play test tone
     *  \param  volume = volume value
     *  \param  sound = which sound number
     *  \param  alarmType = alarm type
     */
    /****************************************************************************/
    void PlayTestTone(quint8 volume, quint8 sound, Global::AlarmType alarmType);
};

} // end namespace Settings

#endif // SETTINGS_ALARMSETTINGSDLG_H

