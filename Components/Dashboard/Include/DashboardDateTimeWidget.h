/****************************************************************************/
/*! \file DashboardDateTimeWidget.h
 *
 *  \brief DashboardDateTimeWidget definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 27-Mar-2013
 *   $Author:  $ Santhosha Kumar Raju
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DASHBOARD_DATETIMEWIDGET_H
#define DASHBOARD_DATETIMEWIDGET_H


#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/DateTime.h"
#include "Application/Include/LeicaStyle.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/WheelPanel.h"
#include "MainMenu/Include/MessageDlg.h"


namespace DataManager
{
    class CUserSettings;
}

namespace Dashboard {

namespace Ui {
    class CDashboardDateTimeWidget;
}


/****************************************************************************/
/**
 * \brief Frame for the date and time widget
 */
/****************************************************************************/
class CDashboardDateTimeWidget : public MainMenu::CDialogFrame
{
    Q_OBJECT

public:
    explicit CDashboardDateTimeWidget(QWidget *p_Parent = 0, QMainWindow *pMainWindow = 0);
    virtual ~CDashboardDateTimeWidget();
    void RefreshDateTime(Global::TimeFormat TimeFormat = Global::TIME_24);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetASAPDateTime
     *
     *  \param DateTime =  QDateTime type parameter
     *
     *  \return from SetASAPDateTime
     */
    /****************************************************************************/
    void SetASAPDateTime(const QDateTime& DateTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetUserSettings
     *
     *  \param p_UserSettings = DataManager::CUserSettings type parameter
     *
     *  \return from SetUserSettings
     */
    /****************************************************************************/
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);

private:
    void RetranslateUI();
    void DisabledWheel(bool enable);

    Ui::CDashboardDateTimeWidget *mp_Ui;            //!< User interface
    MainMenu::CScrollWheel *mp_DayWheel;    //!< Day scroll wheel
    MainMenu::CScrollWheel *mp_MonthWheel;  //!< Month scroll wheel
    MainMenu::CScrollWheel *mp_YearWheel;   //!< Year scroll wheel
    MainMenu::CScrollWheel *mp_HourWheel;   //!< Hour scroll wheel
    MainMenu::CScrollWheel *mp_MinWheel;    //!< Minute scroll wheel
    MainMenu::CMessageDlg   *mp_MessageDlg;                      //!< Message Dialogue

    DataManager::CUserSettings *mp_UserSetting;       ///<  Definition/Declaration of variable mp_UserSetting

    //Flags
    bool m_ProcessRunning;                  //!< Process running state
    QDateTime m_selDateTime;                //!< Selected Date and Time
    QDateTime m_ASAPDateTime;
    QDateTime m_lastSetDateTime;
    QString m_strEndTimeForProgram;
    QString m_strEarlierEndTime;
    QString m_strLaterEndTime;
    QString m_strCannotDelay;
    QString m_strRun;
    bool m_IsClickedOK;
    bool m_IsASAPMode;

protected:
    void changeEvent(QEvent *p_Event);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function showEvent
     *
     *  \param p_Event = QShowEvent type parameter
     *
     *  \return from showEvent
     */
    /****************************************************************************/
    void showEvent(QShowEvent *p_Event);

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnOK
     */
    /****************************************************************************/
    void OnOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnCancel
     */
    /****************************************************************************/
    void OnCancel();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateProgramName
     */
    /****************************************************************************/
    void UpdateProgramName();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnRequestASAPDateTime
     */
    /****************************************************************************/
    void OnRequestASAPDateTime();
    /****************************************************************************/
    /*!
     *  \brief  processing slot when Get ASAP DateTime
     *  \param asapDateTime = as soon as possible datetime
     *  \param bIsFirstStepFixation = whether the first step is fixation reagent or not
     *  \param bNeedUpdateUI = whether update GUI or not
     */
    /****************************************************************************/
    void OnGetASAPDateTime(int asapDateTime, bool bIsFirstStepFixation, bool bNeedUpdateUI);

signals:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal OnSelectDateTime
     *  \param  selDateTime = the selected datetime
     */
    /****************************************************************************/
    void OnSelectDateTime(const QDateTime& selDateTime);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RequestAsapDateTime
     *
     */
    /****************************************************************************/
    void RequestAsapDateTime();

};

} // end namespace Dashboard

#endif // DASHBOARD_DATETIMEWIDGET_H

