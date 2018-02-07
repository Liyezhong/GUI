/****************************************************************************/
/*! \file UserPrivilegeWidget.cpp
 *
 *  \brief UserPrivilegeWidget implementation.
 *         Implements change in the user level and password change for the
 *         Supervisor user
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Global/Include/EventObject.h"
#include "Users/Include/UserPrivilegeWidget.h"
#include "ui_UserPrivilegeWidget.h"
#include <QDebug>

namespace Users {

const QString REGEXP_NUMERIC_VALIDATOR = "^[0-9]{1,4}$"; //!< Reg expression for the validator
const qint32 BUTTON_FIXED_WIDTH = 70; ///< Button widget fixed width
const qint32 BUTTON_FIXED_HEIGHT = 70; ///< Button widget fixed height
const qint32 MAX_CHAR_LENGTH = 16; ///< Maximum character length
const qint32 MIN_CHAR_LENGTH = 4; ///< Minimum character length

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 *  \iparam p_KeyBoardWidget = Keyboard widget
 */
/****************************************************************************/
CUserPrivilegeWidget::CUserPrivilegeWidget(QWidget *p_Parent,
                                           KeyBoard::CKeyBoard *p_KeyBoardWidget) :
    MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CUserPrivilegeWidget),
    m_bIsRightTopPressed(false),
    m_bIsRightBottomPressed(false),
    m_bValidRightTopPressed(false),
    m_bValidIntervalPressed(false)
{
    mp_Ui->setupUi(GetContentFrame());
    mp_MainWindow = static_cast<MainMenu::CMainWindow *>(p_Parent);
    m_UserLevel = MainMenu::CMainWindow::Operator;
    m_StdUserBtnClicked = false;
    m_ServiceUserBtnClicked = false;
    m_SupervisorBtnClicked = false;
    m_ChangePasswdBtnClicked = false;
    m_PwdType = "Old";

    mp_Ui->btnAdministrator->setFixedHeight(BUTTON_FIXED_HEIGHT);
    mp_Ui->btnAdministrator->setFixedWidth(BUTTON_FIXED_WIDTH);
    mp_Ui->btnUser->setFixedHeight(BUTTON_FIXED_HEIGHT);
    mp_Ui->btnUser->setFixedWidth(BUTTON_FIXED_WIDTH);
    mp_Ui->btnService->setFixedHeight(BUTTON_FIXED_HEIGHT);
    mp_Ui->btnService->setFixedWidth(BUTTON_FIXED_WIDTH);

    mp_Ui->btnAdministrator->setIcon(QIcon(":/Large/Icons/User_Status_PushButton/Icon_User_Admin.png"));
    mp_Ui->btnUser->setIcon(QIcon(":/Large/Icons/User_Status_PushButton/Icon_User_Operator.png"));
    mp_Ui->btnService->setIcon(QIcon(":/Large/Icons/User_Status_PushButton/Icon_User_Service.png"));

    CONNECTSIGNALSLOTGUI(mp_Ui->btnAdministrator, clicked(), this, OnBtnAdministratorClicked());
    CONNECTSIGNALSLOTGUI(mp_Ui->btnChangePassword, clicked(), this, OnBtnChangePasswordClicked());
    CONNECTSIGNALSLOTGUI(mp_Ui->btnService, clicked(), this, OnBtnServiceClicked());
    CONNECTSIGNALSLOTGUI(mp_Ui->btnUser, clicked(), this, OnBtnUserClicked());

    mp_KeyBoardWidget = p_KeyBoardWidget;
    SetPanelTitle(QApplication::translate("Users::CUserPrivilegeWidget", "User", Q_NULLPTR, -1));
	
	m_Timer = new QTimer(this);
    m_Timer->setInterval(8*60*1000);//8 minutes, enter idle;
    (void)connect(m_Timer, SIGNAL(timeout()), this, SLOT(AppIdleForLongTime()));
    m_Timer->start();

    m_ScrollTimer = new QTimer(this);
    m_ScrollTimer->setInterval(200);
    (void)connect(m_ScrollTimer, SIGNAL(timeout()), this, SLOT(ScrollScreen()));
    mp_Ui->label->setVisible(false);

    m_PressTimer = new QTimer(this);
    m_PressTimer->setInterval(3000);
    m_PressTimer->setSingleShot(true);
    (void)connect(m_PressTimer, SIGNAL(timeout()), this, SLOT(OnTimeOutPress()));

    m_IntervalPressedTimer = new QTimer(this);
    m_IntervalPressedTimer->setInterval(0);
    m_IntervalPressedTimer->setSingleShot(true);
    (void)connect(m_IntervalPressedTimer, SIGNAL(timeout()), this, SLOT(OnTimeOutIntervalPressedTimer()));

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CUserPrivilegeWidget::~CUserPrivilegeWidget()
{
    try
    {
        delete mp_Ui;
        delete m_Timer;
        delete m_ScrollTimer;
        delete m_PressTimer;
        delete m_IntervalPressedTimer;
    }
    catch(...)
    {}
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CUserPrivilegeWidget::changeEvent(QEvent *p_Event)
{
    // forward the change event to base class
    MainMenu::CPanelFrame::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        mp_Ui->retranslateUi(this);
        RetranslateUI();
        break;
    default:
        break;
    }
}

void CUserPrivilegeWidget::mousePressEvent (QMouseEvent * p_Event)
{
    if (m_ScrollTimer->isActive())
        return;

    //LeftBottom
    //QPoint p = p_Event->pos();
    QRect  rectRightTop(597, 32, 200, 200);
    QRect  rectRightBottom(597, 400, 200, 200);

    if (rectRightTop.contains(p_Event->pos()))
    {
        m_bIsRightTopPressed = true;
    }
    else if (rectRightBottom.contains(p_Event->pos()))
    {
        m_bIsRightBottomPressed = true;
    }
    else
        return;

    m_yScroll = 280;
    m_PressTimer->start();
}

void CUserPrivilegeWidget::mouseReleaseEvent (QMouseEvent* p_Event)
{
    Q_UNUSED(p_Event);
    if (m_bIsRightTopPressed && m_bValidRightTopPressed)
    {
        m_IntervalPressedTimer->start();
    }
    m_bIsRightTopPressed = false;
    m_bIsRightBottomPressed = false;
    m_bValidIntervalPressed = false;
    m_bValidRightTopPressed = false;

}

void CUserPrivilegeWidget::OnTimeOutPress()//after 3 seconds
{
    if (m_bIsRightTopPressed)
    {
        m_bValidRightTopPressed = true;
    }
    else if (m_bValidIntervalPressed)
    {
        mp_Ui->label->setVisible(true);
        mp_Ui->label->move(30, m_yScroll);
        m_ScrollTimer->start();
    }
}

void CUserPrivilegeWidget::OnTimeOutIntervalPressedTimer()
{
    m_bIsRightTopPressed = false;
    m_bValidIntervalPressed = true;
}

void CUserPrivilegeWidget::ScrollScreen()
{
    m_yScroll = m_yScroll - 4;
    mp_Ui->label->move(30, m_yScroll);
    if (m_yScroll < -1700)
        m_ScrollTimer->stop();
}

void CUserPrivilegeWidget::AppIdleForLongTime()
{
    m_Timer->stop();
    if (m_UserLevel == MainMenu::CMainWindow::Admin || m_UserLevel == MainMenu::CMainWindow::Service)
        OnBtnUserClicked();
}

void CUserPrivilegeWidget::OnInteractStart()
{
    if (m_UserLevel == MainMenu::CMainWindow::Admin || m_UserLevel == MainMenu::CMainWindow::Service)
    {
        m_Timer->stop();
        m_Timer->start();
    }
}

/****************************************************************************/
/*!
 *  \brief Switch the user role to the standard user
 */
/****************************************************************************/
void CUserPrivilegeWidget::OnBtnUserClicked()
{
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
        return ;

    m_UserLevel = MainMenu::CMainWindow::Operator;
    if (mp_MainWindow) {
        Global::EventObject::Instance().RaiseEvent(Global::EVENT_GLOBAL_USER_ACTIVITY_OPERATOR_LOGIN);
        // set user role to be operator.
        m_UserLevel = MainMenu::CMainWindow::Operator;
        mp_MainWindow->SetUserRole(m_UserLevel);

        QByteArray ByteArray;
        QDataStream UserLevelInfo(&ByteArray, QIODevice::ReadWrite);
        int UserLevel = (int) m_UserLevel; // to avoid PC-Lint
        UserLevelInfo << UserLevel << "";
        emit UserLevelClicked(UserLevelInfo);
    }
}

/****************************************************************************/
/*!
 *  \brief Switch the role to the supervisor after requesting the password
 */
/****************************************************************************/
void CUserPrivilegeWidget::OnBtnAdministratorClicked()
{
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Admin)
        return ;

    m_SupervisorBtnClicked = true;
    if (mp_KeyBoardWidget) {
        mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Users::CUserPrivilegeWidget",
                            "Enter Password", Q_NULLPTR, -1));
        mp_KeyBoardWidget->SetPasswordMode(true);
        mp_KeyBoardWidget->SetValidation(true);
        mp_KeyBoardWidget->SetMaxCharLength(MAX_CHAR_LENGTH);
        mp_KeyBoardWidget->SetMinCharLength(MIN_CHAR_LENGTH);
        mp_KeyBoardWidget->GetLineEditPtr()->setValidator(NULL);
        mp_KeyBoardWidget->show();
        m_UserLevel = MainMenu::CMainWindow::Admin;

        // Connect signals and slots to keyboard.
        ConnectKeyBoardSignalSlots();
    }
}

/****************************************************************************/
/*!
 *  \brief Switch the role to the service user after requesting the password
 */
/****************************************************************************/
void CUserPrivilegeWidget::OnBtnServiceClicked()
{
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Service)
        return ;

    m_ServiceUserBtnClicked = true;
    if (mp_KeyBoardWidget) {
        mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Users::CUserPrivilegeWidget",
                            "Enter Password", Q_NULLPTR, -1));
        mp_KeyBoardWidget->SetPasswordMode(true);
        mp_KeyBoardWidget->SetValidation(true);
        mp_KeyBoardWidget->SetMinCharLength(4);
        // ^ and $ is used for any character. * is used to enter multiple characters
        // [0-9] is used to allow user to enter only 0 to 9 digits
        mp_KeyBoardWidget->SetLineEditValidatorExpression(REGEXP_NUMERIC_VALIDATOR);
        mp_KeyBoardWidget->DisplayNumericKeyBoard();
        mp_KeyBoardWidget->show();
        m_UserLevel = MainMenu::CMainWindow::Service;

        // Connect signals and slots to keyboard.
        ConnectKeyBoardSignalSlots();
    }
}

/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to change the supervisor password
 */
/****************************************************************************/
void CUserPrivilegeWidget::OnBtnChangePasswordClicked()
{
    m_ChangePasswdBtnClicked = true;
    m_PwdType = "Old";
    if (mp_KeyBoardWidget) {
        mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Users::CUserPrivilegeWidget",
                            "Enter Password", Q_NULLPTR, -1));
        mp_KeyBoardWidget->SetPasswordMode(true);
        mp_KeyBoardWidget->SetValidation(true);
        mp_KeyBoardWidget->SetMaxCharLength(MAX_CHAR_LENGTH);
        mp_KeyBoardWidget->SetMinCharLength(MIN_CHAR_LENGTH);
        mp_KeyBoardWidget->show();

        // Connect signals and slots to keyboard.
        ConnectKeyBoardSignalSlots();
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when OK button on Keyboard is clicked.
 */
/****************************************************************************/
void CUserPrivilegeWidget::OnESCClicked()
{
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
    // reset the button flags
    m_SupervisorBtnClicked = false;
    m_ServiceUserBtnClicked = false;
    m_StdUserBtnClicked = false;
    m_ChangePasswdBtnClicked = false;

}

/****************************************************************************/
/*!
 *  \brief This slot is called when OK button on Keyboard is clicked.
 *
 *  \iparam EnteredString = Entered String from keyboard.
 */
/****************************************************************************/
void CUserPrivilegeWidget::OnOkClicked(QString EnteredString)
{
    QString LineEditString = EnteredString;
    qDebug() << "ok clicked:" << LineEditString;
    if (mp_KeyBoardWidget) {
        mp_KeyBoardWidget->hide();

        if (m_StdUserBtnClicked) {
            m_StdUserBtnClicked = false;
            m_ChangePasswdBtnClicked = false;
            m_PwdType = "Old";
        }
        else if (m_SupervisorBtnClicked){
            m_SupervisorBtnClicked = false;
            m_ChangePasswdBtnClicked = false;
            m_PwdType = "Old";
            m_Timer->setInterval(10*60*1000); //10 minutes timeout for supervisor;
        }
        else if (m_ServiceUserBtnClicked){
            m_ServiceUserBtnClicked = false;
            m_ChangePasswdBtnClicked = false;
            m_PwdType = "Old";
            m_Timer->setInterval(20*60*1000); //20 minutes timeout for service;
        }
        else if (m_ChangePasswdBtnClicked) {
            /// don't reset the change password button flag
            // this resets the password
            qDebug("Change Passwd");
        }
        else {
            return;
        }

        // check the length of the string
        if (LineEditString.length() > 0) {
            QByteArray ByteArray;
            if (!m_ChangePasswdBtnClicked) {
                QDataStream UserLevelInfo(&ByteArray, QIODevice::ReadWrite);
                int UserLevel = (int) m_UserLevel; // to avoid PC-Lint
                UserLevelInfo << (int) UserLevel << LineEditString;

                emit UserLevelClicked(UserLevelInfo);
            }
            else {
                QDataStream PasswordInfo(&ByteArray, QIODevice::ReadWrite);
                PasswordInfo << m_PwdType << LineEditString;
                emit ChangePasswordClicked(PasswordInfo);
            }
        }
        // Disconnect signals and slots connected to keyboard.
        DisconnectKeyBoardSignalSlots();
    }
}

/****************************************************************************/
/*!
 *  \brief Checks the whether user authentication is done or not
 *
 *  \param AuthenticatedLevel    Authenticated user level
 */
/****************************************************************************/
void CUserPrivilegeWidget::UserAuthenticated(const qint32 &AuthenticatedLevel)
{
    if (mp_MainWindow) {
        m_UserLevel = (MainMenu::CMainWindow::UserRole_t)AuthenticatedLevel;

        //m_UserLevel
        mp_MainWindow->SetUserRole(m_UserLevel);

        //user/service/supervisor mode
        switch (m_UserLevel) {
        case MainMenu::CMainWindow::Operator:
            mp_MainWindow->SetUserIcon(MainMenu::CMainWindow::Operator);
            break;
        case MainMenu::CMainWindow::Admin:
            mp_MainWindow->SetUserIcon(MainMenu::CMainWindow::Admin);
            break;
        case MainMenu::CMainWindow::Service:
            mp_MainWindow->SetUserIcon(MainMenu::CMainWindow::Service);
            break;
        default:
            break;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Changes the admin password step by step
 *
 *  \param PasswordType     Gives the password type
 */
/****************************************************************************/
void CUserPrivilegeWidget::ChangeInAdminPassword(const QString &PasswordType)
{
    bool InitKeyboard = true;
    if (mp_KeyBoardWidget) {
        // check the password type
        if (PasswordType.compare("New") == 0) {
            m_PwdType = "New";
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Users::CUserPrivilegeWidget",
                                "Enter New Password", Q_NULLPTR, -1));
            // make it true so that still we are in change password
            m_ChangePasswdBtnClicked = true;
        }
        else if(PasswordType.compare("Confirm") == 0) {
            m_PwdType = "Confirm";
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Users::CUserPrivilegeWidget",
                               "Confirm New Password", Q_NULLPTR, -1));
            // make it true so that still we are in change password
            m_ChangePasswdBtnClicked = true;
        }
        else {
            if (PasswordType.compare("Failed") == 0) {
                // if the authentication fails then change the user level to operator
                m_UserLevel = MainMenu::CMainWindow::Operator;
                if (mp_MainWindow) {
		    mp_MainWindow->SetUserRole(m_UserLevel);
                    mp_MainWindow->SetUserIcon(MainMenu::CMainWindow::Operator);
                    //logan add: inform master to change to operator
                    QByteArray ByteArray;
                    QDataStream UserLevelInfo(&ByteArray, QIODevice::ReadWrite);
                    int UserLevel = (int) MainMenu::CMainWindow::Operator; // to avoid PC-Lint
                    UserLevelInfo << (int) UserLevel << "";
                    emit UserLevelClicked(UserLevelInfo);
                    //logan add: end
                }
            }
            // reset the falgs
            m_PwdType = "Old";
            // make it false , there is some problem in the change password
            m_ChangePasswdBtnClicked = false;
            InitKeyboard = false;
        }
        // initialize the keyboard
        if (InitKeyboard) {
            mp_KeyBoardWidget->SetPasswordMode(true);
            mp_KeyBoardWidget->SetValidation(true);
            mp_KeyBoardWidget->SetMaxCharLength(MAX_CHAR_LENGTH);
            mp_KeyBoardWidget->SetMinCharLength(MIN_CHAR_LENGTH);
            mp_KeyBoardWidget->show();

            // Connect signals and slots to keyboard.
            ConnectKeyBoardSignalSlots();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Connects signals and slots of keyboard.
 */
/****************************************************************************/
void CUserPrivilegeWidget::ConnectKeyBoardSignalSlots()
{
    if (mp_KeyBoardWidget) {
        // Connect signals and slots to keyboard.
        CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, OkButtonClicked(QString), this, OnOkClicked(QString));
        CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, EscButtonClicked(), this, OnESCClicked());
    }
}

/****************************************************************************/
/*!
 *  \brief Disconnects signals and slots of keyboard.
 */
/****************************************************************************/
void CUserPrivilegeWidget::DisconnectKeyBoardSignalSlots()
{
    if (mp_KeyBoardWidget) {
        // Disconnect signals and slots connected to keyboard.
        (void) disconnect(mp_KeyBoardWidget, SIGNAL(OkButtonClicked(QString)), this, SLOT(OnOkClicked(QString)));
        (void) disconnect(mp_KeyBoardWidget, SIGNAL(EscButtonClicked()), this, SLOT(OnESCClicked()));
    }
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CUserPrivilegeWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Users::CUserPrivilegeWidget", "User", 0, -1));

    if (mp_KeyBoardWidget) {
        if (m_SupervisorBtnClicked || m_ServiceUserBtnClicked || m_ChangePasswdBtnClicked) {
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Users::CUserPrivilegeWidget",
                                                                              "Enter Password", Q_NULLPTR, -1));
        }
        if (m_PwdType == "New") {
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Users::CUserPrivilegeWidget",
                                                                              "Enter New Password",
                                                                              Q_NULLPTR, -1));
        }
        if (m_PwdType == "Confirm") {
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Users::CUserPrivilegeWidget",
                                                                              "Confirm New Password",
                                                                              Q_NULLPTR, -1));
        }
    }
}

} // end namespace Users
