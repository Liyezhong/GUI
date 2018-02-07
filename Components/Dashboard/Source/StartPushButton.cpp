/****************************************************************************/
/*! \file StartPushButton.cpp
 *
 *  \brief Implementation of file for class CStartPushButton.
 *
 *  \b Description:
 *          This is a customized Start/Abort button for Himalaya Project.
 *
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-03-12
 *   $Author:  $ A.Yang
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2014 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "Dashboard/Include/StartPushButton.h"
#include <QPainter>
#include <QPaintEvent>
#include "Core/Include/GlobalHelper.h"

namespace Dashboard {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CStartPushButton::CStartPushButton(QWidget *p_Parent) :
    QPushButton(p_Parent),
    m_IsStartStatus(true)
{
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CStartPushButton::~CStartPushButton()
{
}

void CStartPushButton::setText(const QString &txt)
{
    m_Text = txt;
}

void CStartPushButton::IsStartStatus(bool bSet)
{
    m_IsStartStatus = bSet;
}

/****************************************************************************/
/*!
 *  \brief Draw the pause button 
 */
/****************************************************************************/
void CStartPushButton::paintEvent(QPaintEvent * e)
{
    QPushButton::paintEvent(e);

    QPainter painter(this);
    QRect r = e->rect();
    int XCenter = r.width() / 2;
    int top = 13;
    int left = 15;
    int width = 73;
    //draw the caption at the left side
    const QFont& ft = painter.font();
    QFontMetrics fm(ft);
    int stringWidth = fm.width(m_Text);

    QString tempString(m_Text);
    if (stringWidth > width)
    {
        width = 80;
        tempString = Core::CGlobalHelper::TrimText(fm, m_Text, width, true);
    }

    QRect rectText(left, 3, width, 26);
    painter.drawText(rectText, Qt::AlignRight|Qt::AlignHCenter|Qt::AlignBottom, tempString);

    //draw the virtical line
    int lineLeft = XCenter + 33;
    painter.drawLine(lineLeft, top, lineLeft, r.bottom() - 4);

    //draw the solid circle point at the right side
    QRectF rectangle(lineLeft + 8, top + 2, 10.0, 10.0);
    QPainterPath path;
    path.addEllipse(rectangle);

    QColor color(Qt::white);
    if (!this->isEnabled())
    {
        color = Qt::lightGray;
    }
    else if (m_IsStartStatus)
    {
        color.setRgb(162, 164, 164);
    }
    else if (!m_IsStartStatus)
        color.setRgb(255, 1, 1);

    QBrush brush(color);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillPath(path, brush);
}

} // end namespace Dashboard
