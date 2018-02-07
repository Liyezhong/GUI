/****************************************************************************/
/*! \file PausePushButton.cpp
 *
 *  \brief Implementation of file for class CPausePushButton.
 *
 *  \b Description:
 *          This is a customized push button for Himalaya Project.
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

#include "Dashboard/Include/PausePushButton.h"
#include <QPainter>
#include <QPaintEvent>

namespace Dashboard {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CPausePushButton::CPausePushButton(QWidget *p_Parent) : QPushButton(p_Parent)
{
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CPausePushButton::~CPausePushButton()
{
}

/****************************************************************************/
/*!
 *  \brief Draw the pause button 
 */
/****************************************************************************/
void CPausePushButton::paintEvent(QPaintEvent * e)
{
    QPushButton::paintEvent(e);

    QPainter painter(this);
    QRect r = e->rect();
    int XCenter = r.width() / 2;
    QColor color(Qt::white);
    if (!this->isEnabled())
    {
        color = Qt::lightGray;
    }
    QBrush brush(color);
    int widthVirticalLine = 3;
    int top = 13;
    int heighVirticalLine = r.height() - top * 2;

    painter.fillRect(QRect(XCenter - widthVirticalLine, top, widthVirticalLine, heighVirticalLine), brush);
    painter.fillRect(QRect(XCenter + widthVirticalLine, top, widthVirticalLine, heighVirticalLine), brush);
}

} // end namespace Dashboard
