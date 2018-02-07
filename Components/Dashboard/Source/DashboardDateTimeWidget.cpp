/****************************************************************************/
/*! \file DashboardDateTimeWidget.cpp
 *
 *  \brief DashboardDateTimeWidget implementation.
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

#include "Dashboard/Include/DashboardDateTimeWidget.h"
#include <Global/Include/AdjustedTime.h>
#include "Application/Include/LeicaStyle.h"
#include "ui_DashboardDateTimeWidget.h"
#include "Dashboard/Include/FavoriteProgramsPanelWidget.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"


namespace Dashboard {

const int ONE_WEEK_TIME_OFFSET_VALUE = (7 * 24 * 60 * 60);       ///<  Definition/Declaration of variable ONE_WEEK_TIME_OFFSET_VALUE

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 *  \param pMainWindow =  QMainWindow type parameter
 */
/****************************************************************************/
CDashboardDateTimeWidget::CDashboardDateTimeWidget(QWidget *p_Parent, QMainWindow *pMainWindow)
    : CDialogFrame(p_Parent, pMainWindow),
    mp_Ui(new Ui::CDashboardDateTimeWidget),
    mp_UserSetting(NULL),
    m_IsClickedOK(false),
    m_IsASAPMode(false)
{

    mp_Ui->setupUi(GetContentFrame());


    mp_DayWheel = new MainMenu::CScrollWheel();
    mp_MonthWheel = new MainMenu::CScrollWheel();
    mp_YearWheel = new MainMenu::CScrollWheel();
    mp_HourWheel = new MainMenu::CScrollWheel();
    mp_MinWheel = new MainMenu::CScrollWheel();

    mp_MessageDlg = new MainMenu::CMessageDlg(this);

    // Day
    for (int i = 1; i <= 31; i++) {
        mp_DayWheel->AddItem(QString("%1").arg(i, 2, 10, QChar('0')), i);
    }

    // Month
    for (int i = 1; i <= 12; i++) {
        mp_MonthWheel->AddItem(QString("%1").arg(i, 2, 10, QChar('0')), i);
    }

    // Year
    for (int i = 2000; i <= 2099; i++) {
        mp_YearWheel->AddItem(QString::number(i).right(QString::number(i).length() - 2), i);
    }

    // Minute
    for (int i = 0; i < 60; i++) {
        mp_MinWheel->AddItem(QString("%1").arg(i, 2, 10, QChar('0')), i);
    }

    mp_Ui->scrollPanelDate->Init(3);
    mp_Ui->scrollPanelDate->SetTitle(tr("Date"));
    mp_Ui->scrollPanelDate->AddScrollWheel(mp_DayWheel, 0);
    mp_Ui->scrollPanelDate->SetSubtitle(tr("Day"), 0);
    mp_Ui->scrollPanelDate->AddSeparator(MainMenu::CWheelPanel::FULLSTOP, 0);
    mp_Ui->scrollPanelDate->AddScrollWheel(mp_MonthWheel, 1);
    mp_Ui->scrollPanelDate->SetSubtitle(tr("Month"), 1);
    mp_Ui->scrollPanelDate->AddSeparator(MainMenu::CWheelPanel::FULLSTOP, 1);
    mp_Ui->scrollPanelDate->AddScrollWheel(mp_YearWheel, 2);
    mp_Ui->scrollPanelDate->SetSubtitle(tr("Year"), 2);

    mp_Ui->scrollPanelTime->Init(2);
    mp_Ui->scrollPanelTime->SetTitle(tr("Time"));
    mp_Ui->scrollPanelTime->AddScrollWheel(mp_HourWheel, 0);
    mp_Ui->scrollPanelTime->SetSubtitle(tr("Hour"), 0);
    mp_Ui->scrollPanelTime->AddSeparator(MainMenu::CWheelPanel::COLON, 0);
    mp_Ui->scrollPanelTime->AddScrollWheel(mp_MinWheel, 1);
    mp_Ui->scrollPanelTime->SetSubtitle(tr("Minute"), 1);



    CONNECTSIGNALSLOT(mp_Ui->btnCancel, clicked(), this, OnCancel());
    CONNECTSIGNALSLOT(mp_Ui->btnASAP, clicked(), this, OnRequestASAPDateTime());
    CONNECTSIGNALSLOT(mp_Ui->btnOK, clicked(), this, OnOK());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDashboardDateTimeWidget::~CDashboardDateTimeWidget()
{
    try {
        delete mp_DayWheel;
        delete mp_MonthWheel;
        delete mp_YearWheel;
        delete mp_HourWheel;
        delete mp_MinWheel;
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint.
    }
}

void CDashboardDateTimeWidget ::UpdateProgramName()
{
     QString str(m_strEndTimeForProgram + " \"%1\"");
     SetDialogTitle(str.arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME));
}

void CDashboardDateTimeWidget::SetASAPDateTime(const QDateTime& DateTime)
{
    m_ASAPDateTime = DateTime;
}

void CDashboardDateTimeWidget::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{
    mp_UserSetting = p_UserSettings;
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CDashboardDateTimeWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

void CDashboardDateTimeWidget::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);
    if (mp_UserSetting) {
        RefreshDateTime(mp_UserSetting->GetTimeFormat());
    }
    mp_Ui->btnASAP->setChecked(false);
    m_IsASAPMode = false;
    mp_Ui->btnOK->setText(tr("OK"));
    DisabledWheel(false);
}

/****************************************************************************/
/*!
 *  \brief Sets the date and time displayed by the scroll wheels
 *
 *  \iparam TimeFormat = 12 or 24 hour format
 */
/****************************************************************************/
void CDashboardDateTimeWidget::RefreshDateTime(Global::TimeFormat TimeFormat)
{
    // Hour
    mp_HourWheel->ClearItems();
    for (int i = 0; i < 24; i++) {
        if (TimeFormat == Global::TIME_24) {
            mp_HourWheel->AddItem(QString("%1").arg(i, 2, 10, QChar('0')), i);
        }
        else {
            QPixmap Pixmap = (i < 12) ?
                            QPixmap(QString(":/%1/Digits/Digit_AM.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())) :
                            QPixmap(QString(":/%1/Digits/Digit_PM.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())) ;
            qint32 Hour = (i % 12 == 0) ? 12 : i % 12;
            mp_HourWheel->AddItem(QString("%1").arg(Hour, 2, 10, QChar('0')), i, Pixmap);
        }
    }

    mp_DayWheel->SetCurrentData(m_ASAPDateTime.date().day());
    mp_MonthWheel->SetCurrentData(m_ASAPDateTime.date().month());
    mp_YearWheel->SetCurrentData(m_ASAPDateTime.date().year());

    mp_HourWheel->SetCurrentData(m_ASAPDateTime.time().hour());
    mp_MinWheel->SetCurrentData(m_ASAPDateTime.time().minute());

    // make it UTC
    m_selDateTime.setTimeSpec(Qt::UTC);

    m_selDateTime.setDate(QDate(mp_YearWheel->GetCurrentData().toInt(),
                             mp_MonthWheel->GetCurrentData().toInt(),
                             mp_DayWheel->GetCurrentData().toInt()));

    m_selDateTime.setTime(QTime(mp_HourWheel->GetCurrentData().toInt(), mp_MinWheel->GetCurrentData().toInt()));

    Global::DateFormat DateFmt = mp_UserSetting->GetDateFormat();
    QString Date, Time;
    if (DateFmt == Global::DATE_INTERNATIONAL) {
        Date = m_ASAPDateTime.date().toString("dd.MM.yyyy");
    }
    else if (DateFmt == Global::DATE_ISO) {
        Date = m_ASAPDateTime.date().toString(Qt::ISODate);
    }
    else {
        Date = m_ASAPDateTime.date().toString("MM/dd/yyyy");
    }

    if (TimeFormat == Global::TIME_24) {
        Time = m_ASAPDateTime.time().toString("hh:mm");
    }
    else {
        Time = m_ASAPDateTime.time().toString("hh:mm a");
    }

    mp_Ui->lblDateTimeDisplay->setText(Date + " " + Time);
}

/****************************************************************************/
/*!
 *  \brief Reads data from the scroll wheels
 */
/****************************************************************************/
void CDashboardDateTimeWidget::OnOK()
{
    m_selDateTime.setTimeSpec(Qt::UTC);

    m_selDateTime.setDate(QDate(mp_YearWheel->GetCurrentData().toInt(),
                             mp_MonthWheel->GetCurrentData().toInt(),
                             mp_DayWheel->GetCurrentData().toInt()));

    m_selDateTime.setTime(QTime(mp_HourWheel->GetCurrentData().toInt(), mp_MinWheel->GetCurrentData().toInt()));

    if (!m_selDateTime.isValid())
    {
        mp_MessageDlg->SetTitle(QApplication::translate("MainMenu::CDateTime", "Information",
                                                     Q_NULLPTR, -1));
        mp_MessageDlg->SetIcon(QMessageBox::Information);
        mp_MessageDlg->SetButtonText(1, QApplication::translate("MainMenu::CDateTime", "Ok",
                                                               Q_NULLPTR, -1));
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(QApplication::translate("MainMenu::CDateTime", "The selected date is invalid. Please select another one.",
                                                         Q_NULLPTR, -1));
        (void) mp_MessageDlg->exec();
        return;
    }

    m_IsClickedOK = true;
    emit RequestAsapDateTime();
}


/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CDashboardDateTimeWidget::RetranslateUI()
{
    mp_Ui->scrollPanelDate->SetTitle(QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Date", Q_NULLPTR, -1));
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Day", Q_NULLPTR, -1),0);
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Month", Q_NULLPTR, -1),1);
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Year", Q_NULLPTR, -1),2);


    mp_Ui->scrollPanelTime->SetTitle(QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Time", Q_NULLPTR, -1));
    mp_Ui->scrollPanelTime->SetSubtitle(QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Hour", Q_NULLPTR, -1),0);
    mp_Ui->scrollPanelTime->SetSubtitle(QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Minute", Q_NULLPTR, -1),1);

    mp_MessageDlg->SetTitle(QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Warning", Q_NULLPTR, -1));
    m_strEarlierEndTime = QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Program End Date/Time cannot be earlier than the ASAP End Date/Time.", Q_NULLPTR, -1);
    m_strLaterEndTime = QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Program End Date/Time cannot be later than one week.", Q_NULLPTR, -1);
    m_strCannotDelay = QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Program End Date/Time cannot be later than the ASAP End Date/Time as the first program step is not a fixation reagent.", Q_NULLPTR, -1);
    m_strRun = QApplication::translate("Dashboard::CDashboardDateTimeWidget", "Run", Q_NULLPTR, -1);
    mp_MessageDlg->SetButtonText(1, QApplication::translate("Dashboard::CDashboardDateTimeWidget", "OK", Q_NULLPTR, -1));
    m_strEndTimeForProgram = QApplication::translate("Dashboard::CDashboardDateTimeWidget", "End Time of program", Q_NULLPTR, -1);
}


void CDashboardDateTimeWidget::OnCancel()
{
    reject();
}

void CDashboardDateTimeWidget::OnRequestASAPDateTime()
{
    mp_Ui->btnOK->setEnabled(false);
    m_IsASAPMode = mp_Ui->btnASAP->isChecked();
    emit RequestAsapDateTime();
}

void CDashboardDateTimeWidget::OnGetASAPDateTime(int asapDateTime, bool bIsFirstStepFixation, bool bNeedUpdateUI)
{
    m_ASAPDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(asapDateTime);
    if (m_IsClickedOK)
    {
        if (m_IsASAPMode)
        {
            emit OnSelectDateTime(m_ASAPDateTime);
             m_IsClickedOK = false;
             m_IsASAPMode = false;
            accept();
            return;
        }
        // make it UTC
        m_selDateTime.setTimeSpec(Qt::UTC);

        m_selDateTime.setDate(QDate(mp_YearWheel->GetCurrentData().toInt(),
                                 mp_MonthWheel->GetCurrentData().toInt(),
                                 mp_DayWheel->GetCurrentData().toInt()));

        m_selDateTime.setTime(QTime(mp_HourWheel->GetCurrentData().toInt(), mp_MinWheel->GetCurrentData().toInt()));


        // get adjusted time
        QDateTime CurTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
        int secsDifference = CurTime.secsTo(m_selDateTime);

        QString strEndTime("");
        int allowDiff = 0;
        bool bTooLater = false;
        if (bIsFirstStepFixation)
        {
           allowDiff = ONE_WEEK_TIME_OFFSET_VALUE;
           if (secsDifference > allowDiff)
           {
               bTooLater = true;
               strEndTime = m_strLaterEndTime;
           }
        }
        else
        {
            allowDiff = m_ASAPDateTime.secsTo(m_selDateTime);
            if (allowDiff > 0)
            {
                bTooLater = true;
                strEndTime = m_strCannotDelay;
            }
        }

        if(!bTooLater)
        {
            QTime selTime = m_selDateTime.time();
            QTime asapTime = m_ASAPDateTime.time();

            int selHour = selTime.hour();
            int selMin= selTime.minute();

            int asapHour = asapTime.hour();
            int asapMin= asapTime.minute();
            QDateTime tempSelDatetime, tempAsapDatetime;
            tempSelDatetime.setDate(m_selDateTime.date());
            (void)selTime.setHMS(selHour, selMin, 0);
            tempSelDatetime.setTime(selTime);

            tempAsapDatetime.setDate(m_ASAPDateTime.date());
            (void)asapTime.setHMS(asapHour, asapMin, 0);
            tempAsapDatetime.setTime(asapTime);

            if(tempSelDatetime < tempAsapDatetime)  {
                strEndTime = m_strEarlierEndTime;
            }
        }

        if ("" != strEndTime)
        {
            mp_MessageDlg->SetIcon(QMessageBox::Warning);
            mp_MessageDlg->SetTitle(tr("Warning"));
            mp_MessageDlg->SetText(strEndTime);
            mp_MessageDlg->SetButtonText(1, tr("OK"));
            mp_MessageDlg->HideButtons();    // Hiding First Two Buttons in the Message Dialog
            (void)mp_MessageDlg->exec();
        }
        else{
            m_ASAPDateTime = m_selDateTime;
            emit OnSelectDateTime(m_selDateTime);
            accept();
        }
        m_IsClickedOK = false;
    }
    else
    {
        if (m_IsASAPMode)
        {
            if (mp_UserSetting) {
                RefreshDateTime(mp_UserSetting->GetTimeFormat());
            }
            DisabledWheel(true);
            mp_Ui->lblDateTimeDisplay->setText("ASAP");
            mp_Ui->btnOK->setText(m_strRun);
            mp_Ui->btnOK->setEnabled(true);
        }
        else
        {
            if (bNeedUpdateUI)
            {
                DisabledWheel(false);
                if (mp_UserSetting) {
                    RefreshDateTime(mp_UserSetting->GetTimeFormat());
                }
                mp_Ui->btnOK->setText(tr("OK"));
                mp_Ui->btnOK->setEnabled(true);
            }
        }
    }
}

void CDashboardDateTimeWidget::DisabledWheel(bool enable)
{
    mp_Ui->scrollPanelDate->SetDisabled(enable);
    mp_Ui->scrollPanelTime->SetDisabled(enable);
}
} // end namespace Dashboard
