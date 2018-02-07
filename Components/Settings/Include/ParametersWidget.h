/****************************************************************************/
/*! \file ParametersWidget.h
 *
 *  \brief ParametersWidget definition.
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

#ifndef SETTINGS_PARAMETERSWIDGET_H
#define SETTINGS_PARAMETERSWIDGET_H

#include "MainMenu/Include/PanelFrame.h"


namespace Settings {

namespace Ui {
    class CParametersWidget;
}

/****************************************************************************/
/**
 * \brief Main selection panel for the settings screen
 */
/****************************************************************************/
class CParametersWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

    friend class CTestSettings;

private:
    Ui::CParametersWidget *mp_Ui;   //!< User interface
    void RetranslateUI();
    bool m_bFirstShow;
public:
    explicit CParametersWidget(QWidget *p_Parent = NULL);
    virtual ~CParametersWidget();

protected:
    void changeEvent(QEvent *p_Event);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function showEvent
     *
     *  \param p_Event = QShowEvent type parameter
     *
     *  \return from showEvent
     */
    /****************************************************************************/
    void showEvent(QShowEvent *p_Event);

signals:
    /****************************************************************************/
    /*!
     *  \brief Changes the selected settings widget
     *
     *  \iparam CurrentRow = Number of the widget
     */
    /****************************************************************************/
    void CurrentRowChanged(int CurrentRow);
};

} // end namespace Settings

#endif // SETTINGS_PARAMETERSWIDGET_H

