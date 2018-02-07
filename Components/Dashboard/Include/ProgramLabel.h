/****************************************************************************/
/*! \file ProgramLabel.h
 *
 *  \brief Header file for class CProgramLabel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-07-16
 *   $Author:  $ Jeff Chen
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
//lint -esym(1511, QLabel::setText)

#ifndef DASHBOARD_PROGRAMLABEL_H
#define DASHBOARD_PROGRAMLABEL_H

#include <QLabel>

namespace Dashboard {
/****************************************************************************/
/**
 * \brief This is a customized Label for glow program name.
 */
/****************************************************************************/
class CProgramLabel : public QLabel
{
    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of construction CProgramLabel
     *
     *  \param p_Parent =  Parent widget
     */
    /****************************************************************************/
    explicit CProgramLabel(QWidget *p_Parent = 0);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of destruction CProgramLabel
     *
     */
    /****************************************************************************/
    virtual ~CProgramLabel();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function setText
     *
     *  \param text =  text to show in this label
     *  \param highlight =  highlight or not
     *
     */
    /****************************************************************************/
    void setText(const QString &text, bool highlight = false);

public slots:


protected:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function paintEvent
     *
     *  \param event =  Paint event
     */
    /****************************************************************************/
    void paintEvent(QPaintEvent * event);
    QString m_Content;       ///<  Definition/Declaration of variable m_Text
    QPixmap m_PixmapContent; ///<  text pixmap
};

} // end namespace Dashboard

#endif // DASHBOARD_PROGRAMLABEL_H

