/****************************************************************************/
/*! \file DlgConfirmationText.h
 *
 *  \brief Declaration of confirmation dialog with status.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-05-28
 *   $Author:  $ R.Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef MAINMENU_DLGCONFIRMATIONTEXT_H
#define MAINMENU_DLGCONFIRMATIONTEXT_H

#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>

#include "ServiceWidget/Include/DlgConfirmationBase.h"

namespace MainMenu {

class CDlgConfirmationText : public CDlgConfirmationBase
{
    Q_OBJECT

public:
    explicit CDlgConfirmationText(QWidget *parent = 0);
    ~CDlgConfirmationText(void);

public Q_SLOTS:
    void SetText(const QString &Text);

private:
    QLabel      *mp_Text;
};

} // namespace MainMenu

#endif // MAINMENU_DlgConfirmationText_H
