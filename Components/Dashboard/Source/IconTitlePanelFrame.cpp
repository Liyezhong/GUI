/****************************************************************************/
/*! \file IconTitlePanelFrame.cpp
 *
 *  \brief IconTitlePanelFrame implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-15-12
 *   $Author:  $ Abe Yang
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

#include "Dashboard/Include/IconTitlePanelFrame.h"
#include "Application/Include/LeicaStyle.h"
#include "ui_IconTitlePanelFrame.h"
#include <QDebug>
#include <QPainter>

namespace Dashboard {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CIconTitlePanelFrame::CIconTitlePanelFrame(QWidget *p_Parent) : QWidget(p_Parent), mp_FrameUi(new Ui::CIconTitlePanelFrame)
{
    mp_FrameUi->setupUi(this);
    m_IsDialog = false;
    QPalette Palette = mp_FrameUi->panelTitle->palette();
    Palette.setBrush(QPalette::WindowText, QColor(Qt::white));
    mp_FrameUi->panelIcon->setPalette(Palette);
    mp_FrameUi->panelTitle->setPalette(Palette);
    mp_FrameUi->panelTitleAdditional->setPalette(Palette);
    mp_FrameUi->panelTitleAdditional->hide();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CIconTitlePanelFrame::~CIconTitlePanelFrame()
{
    try {
        delete mp_FrameUi;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CIconTitlePanelFrame::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_FrameUi->retranslateUi(this);
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the title of the panel
 *
 *  \iparam Title = Title of the panel
 */
/****************************************************************************/
void CIconTitlePanelFrame::SetPanelTitle(QString Title)
{
    mp_FrameUi->panelTitle->setText(tr("%1").arg(Title));
    //mp_FrameUi->panelTitleAdditional->hide();
    //mp_FrameUi->horizontalSpacer->changeSize(0, 0);
    //mp_FrameUi->horizontalSpacer->invalidate();
}

/****************************************************************************/
/*!
 *  \brief Sets two titles, one at left corner and other at right corner
 *
 *  \iparam TitleLeft  = Title at left corner
 *  \iparam TitleRight = Title at right corner
 */
/****************************************************************************/
void CIconTitlePanelFrame::SetPanelTitle(QString TitleLeft, QString TitleRight)
{
    mp_FrameUi->panelTitle->setText(tr("%1").arg(TitleLeft));
    mp_FrameUi->panelTitleAdditional->setText(tr("%1").arg(TitleRight));
    mp_FrameUi->panelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mp_FrameUi->panelTitleAdditional->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
    mp_FrameUi->panelTitleAdditional->show();
}

void CIconTitlePanelFrame::SetPanelIcon(const QString& icon)
{
    QPixmap pixmap(icon);
    QPixmap img = pixmap.copy(8, 8, 54, 45);
    mp_FrameUi->panelIcon->setPixmap(img.scaledToHeight(25, Qt::SmoothTransformation));
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the panel
 *
 *  \iparam pContent = Content layout of the panel
 */
/****************************************************************************/
void CIconTitlePanelFrame::SetContent(QLayout *pContent)
{
    //lint -esym(429, pContent)
    mp_FrameUi->panelWidget->setLayout(pContent);
}


/****************************************************************************/
/*!
 *  \brief Returns the content frame of the panel
 *
 *  \return Content frame of the panel
 */
/****************************************************************************/
QWidget *CIconTitlePanelFrame::GetContentFrame()
{
    return mp_FrameUi->panelWidget;
}

/****************************************************************************/
/*!
 *  \brief Centers the text in the title bar
 */
/****************************************************************************/
void CIconTitlePanelFrame::SetTitleCenter()
{
    mp_FrameUi->panelTitle->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    m_IsDialog = true;
}

/****************************************************************************/
/*!
 *  \brief Draws the background image of the panel
 */
/****************************************************************************/
void CIconTitlePanelFrame::paintEvent(QPaintEvent *)
{
    QPainter Painter(this);
    QPixmap Target(size());
    QPixmap Source;

    Target.fill(Qt::transparent);

    if (m_IsDialog == false) {
        Source = QPixmap(QString(":/%1/Panel.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
    }
    else {
        Source = QPixmap(QString(":/%1/Popup/Popup.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
    }
    Application::CLeicaStyle::BorderPixmap(&Target, &Source, 18, 32, 20, 21);
    Painter.drawPixmap(0, 0, Target);
}

} // end namespace Dashboard
