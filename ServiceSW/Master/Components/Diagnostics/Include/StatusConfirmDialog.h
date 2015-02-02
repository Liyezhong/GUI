/****************************************************************************/
/*! \file Diagnostics/Include/StatusConfirmDialog.h
 *
 *  \brief Header file for class CStatusConfirmDialog.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-05-21
 *   $Author:  $ Sunny Qu
 *
 *  \b Company:
 *
 *       Leica Biosystems Shanghai.
 *
 *  (C) Copyright 2010 by Leica Biosystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef __STATUSCONFIRMDIALOG_H
#define __STATUSCONFIRMDIALOG_H

#include "MainMenu/Include/DialogFrame.h"
#include "Core/Include/ServiceDefines.h"
#include <QTimer>
#include <QDialog>

namespace Diagnostics {

namespace Ui {
    class CStatusConfirmDialog;
}

/****************************************************************************/
/**
 * \brief This dialog is started, when the user has to wait for the result of
 *        a background process.
 */
/****************************************************************************/
class CStatusConfirmDialog : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    /****************************************************************************/
    /*!
     *  \brief Constructor
     *
     *  \iparam p_Parent = Parent widget
     */
    /****************************************************************************/
    explicit CStatusConfirmDialog(QWidget *p_Parent = 0);

    /****************************************************************************/
    /*!
     *  \brief Destructor
     */
    /****************************************************************************/
    virtual ~CStatusConfirmDialog();

    /****************************************************************************/
    /*!
     *  \brief Sets the text displayed in the wait dialog
     *
     *  \iparam Text = Label text
     */
    /****************************************************************************/
    void SetText(const QString& Text);

    /****************************************************************************/
    /*!
     *  \brief Update label status
     *
     *  \iparam Status = Label test status
     */
    /****************************************************************************/
    void UpdateOvenLabel(const Service::ModuleTestStatus &Status);
    /****************************************************************************/
    /*!
     *  \brief Update retort label status
     *
     *  \iparam Status = Label test status
     */
    /****************************************************************************/
    void UpdateRetortLabel(const Service::ModuleTestStatus &Status);

    /****************************************************************************/
    /*!
     *  \brief Hide retort label status
     */
    /****************************************************************************/
    void HideLabel();

    /****************************************************************************/
    /*!
     *  \brief Update label status
     *
     *  \iparam Status = Label test status name, Value = status value
     *  \iparam Value = display content
     */
    /****************************************************************************/
    void UpdateLabel(const QString& Status, const QString& Value);

    /****************************************************************************/
    /*!
     *  \brief Activates a timeout timer
     *
     *  \iparam Milliseconds = Timeout in milliseconds
     */
    /****************************************************************************/
    void SetTimeout(qint32 MilliSeconds);

    /****************************************************************************/
    /*!
     *  \brief Abort wait dialog
     *  \iparam HideFlag = flag of hide status.
     */
    /****************************************************************************/
    void HideAbort(bool HideFlag=true);

    /****************************************************************************/
    /*!
     *  \brief Overrides the show function of QDialog
     *
     *      If the abort button of this message box is enabled, this method will
     *      immediately show the dialog. If this is not the case, the dialog will
     *      be shown after a time of 500 ms.
     */
    /****************************************************************************/
    void Show();

    /****************************************************************************/
    /*!
     *  \brief Abort Bathlayout generating process
     *  \iparam IsBlgProcessStarted = flag for process started.
     */
    /****************************************************************************/
    void BlgProcessProgress(bool IsBlgProcessStarted);
public slots:
    void done(int Result);

protected:
    bool eventFilter(QObject *p_Object, QEvent *p_Event);

private:
    Ui::CStatusConfirmDialog *mp_Ui;     //!< User interface
    bool m_AbortButtonVisible;  //!< Is the abort button visible?
    bool m_DialogLock;          //!< Prevents closing the dialog
    bool m_DialogHide;          //!< Should the dialog be closed?
    QTimer m_Timer;             //!< Timeout timer
    bool m_IsBlgProcessStarted; //!< Bathlayout process indicator flag
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CStatusConfirmDialog)

private slots:
    void LockDialog();
    void UnlockDialog();
    void AbortWaitDialog();

signals:
    /****************************************************************************/
    /**
     * \brief This signal is emitted when the timer time out happens.
     */
    /****************************************************************************/
    void Timeout();
    /****************************************************************************/
    /**
     * \brief This signal is emitted when the Abort button is pressed.
     * \iparam IsBLGProcessAborted = BLG process aborted/cancelled indicator flag
     *
     */
    /****************************************************************************/
    void AbortBlgProcess(bool IsBLGProcessAborted);

};

} // end namespace Diagnostics

#endif // STATUSCONFIRMDIALOG_H