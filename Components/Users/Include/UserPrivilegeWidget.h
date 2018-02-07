/****************************************************************************/
/*! \file UserPrivilegeWidget.h
 *
 *  \brief UserPrivilegeWidget definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-28, 2012-10-30
 *   $Author:  $ M.Scherer, N.Kamath, Raju
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

#ifndef USERS_USERPRIVILEGEWIDGET_H
#define USERS_USERPRIVILEGEWIDGET_H

#include "MainMenu/Include/PanelFrame.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "MainMenu/Include/MainWindow.h"
#include <QRegExpValidator>

class QTimer;

namespace Users {

namespace Ui {
    class CUserPrivilegeWidget;
}

/****************************************************************************/
/**
 * \brief Main widget for user configuration and switching
 */
/****************************************************************************/
class CUserPrivilegeWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT
    friend class CTestUsers;

private:
    Ui::CUserPrivilegeWidget *mp_Ui;                //!< User interface
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;         //!< Reference to Keyboard widget
    MainMenu::CMainWindow *mp_MainWindow;           //!< pointer to main window
    bool m_StdUserBtnClicked;                       //!< True if Std user btn was clicked
    bool m_ServiceUserBtnClicked;                   //!< True if Service user btn was Clicked
    bool m_SupervisorBtnClicked;                    //!< True if Supervisor user btn was clicked
    bool m_ChangePasswdBtnClicked;                  //!< True if Change password btn was clicked
    QString m_PwdType;                              //!< Type of the password is stored i.e. Old, New and Confirm
    MainMenu::CMainWindow::UserRole_t m_UserLevel;  //!< Current user Level
    void RetranslateUI();
    QTimer* m_Timer, *m_ScrollTimer, *m_PressTimer, *m_IntervalPressedTimer;
    int m_yScroll;
    bool m_bIsRightTopPressed, m_bIsRightBottomPressed;
    bool m_bValidRightTopPressed, m_bValidIntervalPressed;
    void ConnectKeyBoardSignalSlots();
    void DisconnectKeyBoardSignalSlots();
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CUserPrivilegeWidget)

public:
    explicit CUserPrivilegeWidget(QWidget *p_Parent = 0,
                                  KeyBoard::CKeyBoard *p_KeyBoardWidget = NULL);
    ~CUserPrivilegeWidget();

protected:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of changeEvent
     *  \param p_Event = all events
     */
    /****************************************************************************/
    void changeEvent(QEvent *p_Event);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of mousePressEvent
     *  \param p_Event = Mouse Event
     */
    /****************************************************************************/
    void mousePressEvent (QMouseEvent * p_Event);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of mouseReleaseEvent
     *  \param p_Event = Mouse Event
     */
    /****************************************************************************/
    void mouseReleaseEvent (QMouseEvent* p_Event);
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnBtnServiceClicked
     */
    /****************************************************************************/
    void OnBtnServiceClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnBtnAdministratorClicked
     */
    /****************************************************************************/
    void OnBtnAdministratorClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnBtnUserClicked
     */
    /****************************************************************************/
    void OnBtnUserClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnBtnChangePasswordClicked
     */
    /****************************************************************************/
    void OnBtnChangePasswordClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnOkClicked
     */
    /****************************************************************************/
    void OnOkClicked(QString EnteredString);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnESCClicked
     */
    /****************************************************************************/
    void OnESCClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of AppIdleForLongTime
     */
    /****************************************************************************/
	void AppIdleForLongTime();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of ScrollScreen
     */
    /****************************************************************************/
    void ScrollScreen();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnTimeOutPress()
     */
    /****************************************************************************/
    void OnTimeOutPress();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnTimeOutIntervalPressedTimer()
     */
    /****************************************************************************/
    void OnTimeOutIntervalPressedTimer();

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UserAuthenticated
     */
    /****************************************************************************/
    void UserAuthenticated(const qint32 &AuthenticatedLevel);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot ChangeInAdminPassword
     */
    /****************************************************************************/
    void ChangeInAdminPassword(const QString &PasswordType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnInteractStart
     */
    /****************************************************************************/
    void OnInteractStart();
signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when the langauge is changed.
     *
     */
    /****************************************************************************/
    void UserLevelClicked(QDataStream &);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when Change Password button is clicked.
     *
     */
    /****************************************************************************/
    void ChangePasswordClicked(QDataStream &);

};

} // end namespace Users

#endif // USERPRIVILEGEWIDGET_H

