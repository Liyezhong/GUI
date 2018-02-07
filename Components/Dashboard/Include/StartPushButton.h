/****************************************************************************/
/*! \file StartPushButton.h
 *
 *  \brief Header file for class CStartPushButton.
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

#ifndef DASHBOARD_STARTPUSHBUTTON_H
#define DASHBOARD_STARTPUSHBUTTON_H

#include <QPushButton>

namespace Dashboard {
  

/****************************************************************************/
/**
 * \brief This is a customized Start/Abort button for Himalaya Project.
 */
/****************************************************************************/
class CStartPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit CStartPushButton(QWidget *p_Parent = 0);
    virtual ~ CStartPushButton();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function setText
     *
     *  \param text =  QString type parameter
     *
     *  \return from setText
     */
    /****************************************************************************/
    void setText(const QString &text);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsStartStatus
     *  \param bSet =  bool type parameter
     *  \return from IsStartStatus
     */
    /****************************************************************************/
    void IsStartStatus(bool bSet);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsStartStatus
     *  \return from IsStartStatus
     */
    /****************************************************************************/
    inline bool IsStartStatus() { return m_IsStartStatus; }
private:
    void paintEvent(QPaintEvent *);
    QString m_Text;       ///<  Definition/Declaration of variable m_Text
    bool m_IsStartStatus;       ///<  Definition/Declaration of variable m_IsStartStatus
};

} // end namespace Dashboard

#endif // DASHBOARD_STARTPUSHBUTTON_H

