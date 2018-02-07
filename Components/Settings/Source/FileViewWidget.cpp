/****************************************************************************/
/*! \file FileViewWidget.cpp
 *
 *  \brief FileViewWidget definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-12-16, 2012-11-07
 *   $Author:  $ N.Kamath, Raju
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

#include "Settings/Include/FileViewWidget.h"
#include <QApplication>
#include <QEvent>

namespace Settings {
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CFileViewWidget::CFileViewWidget(QWidget *p_Parent) : MainMenu::CPanelFrame(p_Parent)
{
    mp_FileView = new MainMenu::CFileView(this);
    SetContent(mp_FileView->layout());
    // connect the slots
    CONNECTSIGNALSLOT(this, DayRunLogFileContent(const QDataStream &),
                      mp_FileView, DayRunLogFileContent(const QDataStream &));
    CONNECTSIGNALSLOT(this, DayRunLogFileNames(const QStringList &),
                      mp_FileView, DayRunLogFileNames(const QStringList &));
    // connect the signals
    CONNECTSIGNALSIGNAL(mp_FileView, GetFileContent(const QString &),
                        this, SelectedLogFile(const QString &));
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CFileViewWidget::~CFileViewWidget()
{
    try {
        delete mp_FileView;
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
void CFileViewWidget::changeEvent(QEvent *p_Event)
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
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CFileViewWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CFileViewWidget", "Event View", Q_NULLPTR, -1));
}

/****************************************************************************/
/**
 * \brief Set the panel size of the Eventviewr window
 *
 * \iparam Width = width of the panel
 * \iparam Height = Height of the panel
 */
/****************************************************************************/
void CFileViewWidget::SetPanelSize(const qint32 &Width, const qint32 &Height)
{
    mp_FileView->SetEventViewerPanelSize(Width, Height);
}

/****************************************************************************/
/**
 * \brief Disable the "Open" button in the widget
 */
/****************************************************************************/
void CFileViewWidget::DisableButton()
{
    mp_FileView->DisableButton();
}

}//End of namespace Settings
