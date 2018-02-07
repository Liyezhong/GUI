/****************************************************************************/
/*! \file ParametersWidget.cpp
 *
 *  \brief ParametersWidget implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-29
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
#include "Settings/Include/ParametersWidget.h"
#include "ui_ParametersWidget.h"

namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CParametersWidget::CParametersWidget(QWidget *p_Parent) : MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CParametersWidget),
    m_bFirstShow(true)
{
    mp_Ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("Parameter"));

    QPalette Palette = mp_Ui->parameterList->palette();
    Palette.setColor(QPalette::Base, Qt::transparent);
    Palette.setColor(QPalette::Highlight, Qt::transparent);
    Palette.setColor(QPalette::HighlightedText, Qt::black);
    mp_Ui->parameterList->setPalette(Palette);
    mp_Ui->parameterList->setCurrentRow(0);
    CONNECTSIGNALSIGNAL(mp_Ui->parameterList, currentRowChanged(int), this, CurrentRowChanged(int));
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CParametersWidget::~CParametersWidget()
{
    delete mp_Ui;
}

void CParametersWidget::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);
    if (m_bFirstShow)
    {
        mp_Ui->retranslateUi(this);
        m_bFirstShow = false;
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CParametersWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            if (this->isVisible()) {
                mp_Ui->retranslateUi(this);
            }
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
void CParametersWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CParametersWidget", "Parameter", Q_NULLPTR, -1));
}

} // end namespace Settings
