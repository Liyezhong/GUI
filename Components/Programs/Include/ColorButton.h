/****************************************************************************/
/*! \file ColorButton.h
 *
 *  \brief Include file for class CColorButton.
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

#ifndef PROGRAMS_COLORBUTTON_H
#define PROGRAMS_COLORBUTTON_H

#include <QPushButton>

namespace Programs {

/****************************************************************************/
/**
 * \brief This is the colored button for the rack color selection dialog.
 */
/****************************************************************************/
class CColorButton : public QPushButton
{
public:
    explicit CColorButton(QWidget *p_Parent = 0);
    void SetColor(QString Color);
};

} // end namespace Programs

#endif // PROGRAMS_COLORBUTTON_H

