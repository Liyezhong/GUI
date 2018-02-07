/****************************************************************************/
/*! \file IconTitlePanelFrame.h
 *
 *  \brief IconTitlePanelFrame definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-11-12
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

#ifndef DASHBOARD_ICONTITLEPANELFRAME_H
#define DASHBOARD_ICONTITLEPANELFRAME_H

#include <QWidget>

namespace Dashboard {

namespace Ui {
    class CIconTitlePanelFrame;
}

/****************************************************************************/
/**
 * \brief This is the basic background widget for all GUI panels.
 */
/****************************************************************************/
class CIconTitlePanelFrame : public QWidget
{
    Q_OBJECT

public:
    explicit CIconTitlePanelFrame(QWidget *p_Parent = 0);
    virtual ~CIconTitlePanelFrame();

    void SetPanelTitle(QString Title);
    void SetPanelTitle(QString TitleLeftCorner, QString TitleRigtCorner);
    void SetTitleCenter();
    void SetContent(QLayout *p_Content);
    QWidget *GetContentFrame();
    
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetPanelIcon
     *
     *  \param icon =  QString type parameter
     *
     *  \return from SetPanelIcon
     */
    /****************************************************************************/
    void SetPanelIcon(const QString& icon);

private:
    void paintEvent(QPaintEvent *);
    Ui::CIconTitlePanelFrame *mp_FrameUi;    //!< User interface
    bool m_IsDialog;    //!< Indicates if the frame belongs to a dialog

protected:
    void changeEvent(QEvent *p_Event);
};

} // end namespace Dashboard

#endif // DASHBOARD_ICONTITLEPANELFRAME_H

