/****************************************************************************/
/*! \file DateTimeWidget.cpp
 *
 *  \brief DateTimeWidget implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-21
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
#include "Settings/Include/DateTimeWidget.h"
#include <QApplication>

namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CDateTimeWidget::CDateTimeWidget(QWidget *p_Parent) :
    MainMenu::CPanelFrame(p_Parent),
    mp_UserSettings(NULL),
    mp_MainWindow(NULL)
{
    mp_DateTime = new MainMenu::CDateTime();
    SetPanelTitle(tr("Date/Time"));
    SetContent(mp_DateTime->layout());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDateTimeWidget::~CDateTimeWidget()
{
    try {
        delete mp_DateTime;
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

void CDateTimeWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            RetranslateUI();
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief  Returns the content of this panel
 *
 *  \return A pointer to the content
 */
/****************************************************************************/
MainMenu::CDateTime *CDateTimeWidget::GetContent()
{
    return mp_DateTime;
}

/****************************************************************************/
/*!
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CDateTimeWidget::SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings)
{
    mp_UserSettings = p_UserSettings;
}

/****************************************************************************/
/*!
 *  \brief Updates the widget content everytime it is displayed
 *
 *  \iparam p_Event = Show event
 */
/****************************************************************************/
void CDateTimeWidget::showEvent(QShowEvent *p_Event) {
    if((mp_DateTime != NULL) && (mp_UserSettings != NULL) && (p_Event != NULL) && !p_Event->spontaneous()) {
        // widget is quite before to be shown (again) Refresh contents.
        mp_DateTime->RefreshDateTime(mp_UserSettings->GetTimeFormat());
        mp_DateTime->ResetButtons();
    }
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CDateTimeWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CDateTimeWidget", "Date/Time", Q_NULLPTR, -1));
}

void CDateTimeWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
    if (mp_DateTime)
    {
        mp_DateTime->SetPtrToMainWindow(mp_MainWindow);
    }
}

} // end namespace Settings
