/****************************************************************************/
/*! \file ColorButton.cpp
 *
 *  \brief Implementation file for class CColorButton.
 *
 *  \b Description:
 *         This class implements assigning of color to the Color button in
 *         Edit program widget which is unique to the selected program.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-16
 *   $Author:  $ M.Scherer, C.Adaragunchi1234
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

#include "Programs/Include/ColorButton.h"

namespace Programs {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CColorButton::CColorButton(QWidget *p_Parent) : QPushButton(p_Parent)
{
}

/****************************************************************************/
/*!
 *  \brief Sets the button color
 *
 *  \iparam Color = Color string as it is used in the data model
 */
/****************************************************************************/
void CColorButton::SetColor(QString Color)
{
    QPalette Palette = palette();

    if (Color == "grey" || Color == "lightblue" || Color == "green" || Color == "pink" ||
            Color == "white" || Color == "yellow") {
        Palette.setColor(QPalette::ButtonText, Qt::black);
    }
    else {
        Palette.setColor(QPalette::ButtonText, Qt::white);
    }
    Palette.setColor(QPalette::Button, QColor(Color));
    setPalette(Palette);
}

} // end namespace Programs
