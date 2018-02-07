/****************************************************************************/
/*! \file Himalaya/HimalayaMain/Gui/Components/Core/Source/Startup.cpp
 *
 *  \brief Startup implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-31 - 29-Nov-2012
 *   $Author:  $ M.Scherer,Ningu
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

#include "Core/Include/Startup.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "MainMenu/Include/StatusBarManager.h"
#include "Application/Include/LeicaStyle.h"
#include "Application/Include/Application.h"
#include "Dashboard/Include/SplashWidget.h"
#include "Core/Include/GlobalHelper.h"
#include "Dashboard/Include/DashboardWidget.h"

namespace Core {

CStartup* CStartup::self = NULL;
/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CStartup::CStartup() : QObject()
{
    //m_MainWindow.setWindowTitle("MainWindow");
    CStartup::self = this;
    m_MainWindow.setFixedSize(800, 600);
    m_MainWindow.SetVerticalOrientation(true);
    m_MainWindow.SetUserIcon(MainMenu::CMainWindow::Operator);

    #ifdef Q_WS_QWS
        m_MainWindow.showFullScreen();
    #else
        m_MainWindow.show();
    #endif


    mp_DataConnector = new Core::CDataConnector(&m_MainWindow);
    mp_GlobalHelper = new CGlobalHelper(mp_DataConnector);

    mp_Dashboard = new Dashboard::CDashboardWidget(mp_DataConnector, &m_MainWindow);
    mp_KeyBoardWidget = new KeyBoard::CKeyBoard(KeyBoard::SIZE_1, KeyBoard::QWERTY_KEYBOARD);
    mp_KeyBoardWidget->setModal(true);
    mp_Programs = new Programs::CProgramWidget(mp_DataConnector, &m_MainWindow, mp_KeyBoardWidget);
    mp_Reagents = new Reagents::CReagentWidget(mp_DataConnector, &m_MainWindow, mp_KeyBoardWidget);
    mp_Settings = new Settings::CSettingsWidget(mp_DataConnector, &m_MainWindow, mp_KeyBoardWidget);
    mp_Users = new Users::CUserPrivilegeWidget(&m_MainWindow, mp_KeyBoardWidget);
    mp_ScreenSaver = new ScreenSaverWidget();
    mp_SplashWidget = new SplashWidget(&m_MainWindow);
    mp_SplashWidget->setModal(true);
    (void)MainMenu::CStatusBarManager::CreateInstance(&m_MainWindow,mp_DataConnector->SettingsInterface);

    // Dashboard Signals & Slots
    CONNECTSIGNALSLOT(mp_Reagents, UnselectProgram(), mp_Dashboard, OnUnselectProgram());
    CONNECTSIGNALSLOT(mp_Programs, UnselectProgram(), mp_Dashboard, OnUnselectProgram());
    CONNECTSIGNALSLOT(mp_Settings, UnselectProgram(), mp_Dashboard, OnUnselectProgram());

    Application::CApplication* pApp =  dynamic_cast<Application::CApplication*>(QCoreApplication::instance());
    CONNECTSIGNALSLOT(pApp, InteractStart(), mp_ScreenSaver, OnInteractStart());
    CONNECTSIGNALSLOT(pApp, InteractStart(), mp_Users, OnInteractStart());
    CONNECTSIGNALSIGNAL(pApp, InteractStart(), mp_Dashboard, OnInteractStart());

    CONNECTSIGNALSIGNAL(mp_DataConnector, ProgramsInitialized(), mp_Dashboard, UpdateItemsToFavoritePanel());
    CONNECTSIGNALSIGNAL(mp_DataConnector, ProgramDeleted(), mp_Dashboard, UpdateItemsToFavoritePanel());

    CONNECTSIGNALSIGNAL(mp_Programs, FavoriteProgramListUpdated(DataManager::CProgram &), mp_Dashboard, UpdateItemsToFavoritePanel());
    CONNECTSIGNALSIGNAL(mp_Programs, FavoriteProgramListUpdated(DataManager::CProgram &), mp_Programs, UpdateProgram(DataManager::CProgram &));

    CONNECTSIGNALSIGNAL(mp_Dashboard, UpdateSelectedStationList(QList<QString>&), mp_Reagents, UpdateSelectedStationList(QList<QString>&));
    CONNECTSIGNALSLOT(mp_Dashboard, ProgramSelected(QString&, QList<QString>&), mp_GlobalHelper, OnProgramSelected(QString&, QList<QString>&));

    CONNECTSIGNALSLOT(mp_DataConnector, ProgramUpdated(), mp_Programs, ProgramUpdated());
    CONNECTSIGNALSLOT(mp_DataConnector, ProgramAdded(), mp_Programs, ProgramAdded());
    CONNECTSIGNALSLOT(mp_DataConnector, RevertChangedProgram(), mp_Programs, RevertChangedProgram());

    CONNECTSIGNALSLOT(mp_Programs, UpdateProgram(DataManager::CProgram &),
                      mp_DataConnector, SendProgramUpdate(DataManager::CProgram &));
    CONNECTSIGNALSLOT(mp_Programs, AddProgram(DataManager::CProgram &),
                      mp_DataConnector, SendProgramAdd(DataManager::CProgram &));
    CONNECTSIGNALSLOT(mp_Programs, DeleteProgram(QString &),mp_DataConnector,SendProgramRemove(QString &));

    CONNECTSIGNALSLOT(mp_Reagents, BottleCheck(), mp_DataConnector, SendBottleCheck());

    // this signal is to update program icon on program panel widget
    CONNECTSIGNALSIGNAL(mp_Programs, UpdateProgram(DataManager::CProgram &), mp_Dashboard, UpdateProgram(DataManager::CProgram &));


    // Signals for the Users
    CONNECTSIGNALSLOT(mp_Users, UserLevelClicked(QDataStream &), mp_DataConnector,
                      SendUserLevel(QDataStream &));
    CONNECTSIGNALSIGNAL(mp_Users, UserLevelClicked(QDataStream &), mp_Settings,
                        UserLevelChanged(QDataStream &));
    CONNECTSIGNALSLOT(mp_Users, ChangePasswordClicked(QDataStream &), mp_DataConnector,
                      SendChangepassword(QDataStream &));

    CONNECTSIGNALSLOT(mp_DataConnector, LanguageChanged(const QString&), mp_KeyBoardWidget, SwitchLayout(const QString&));

    // signals for the users
    CONNECTSIGNALSLOT(mp_DataConnector, UserAuthentication(const qint32 &), mp_Users, UserAuthenticated(const qint32 &));
    CONNECTSIGNALSLOT(mp_DataConnector, ChangeAdminPassword(const QString &), mp_Users, ChangeInAdminPassword(const QString &));

    CONNECTSIGNALSLOT(mp_DataConnector, EndGUIInitialization(), mp_SplashWidget, ShowModel());
    CONNECTSIGNALSLOT(mp_DataConnector, ProgramSelfTestPassed(), mp_SplashWidget, accept());
    CONNECTSIGNALSLOT(mp_DataConnector, ProgramSelfTestFailed(), mp_SplashWidget, accept());
    CONNECTSIGNALSLOT(mp_DataConnector, BottleCheckReply(const QString&, DataManager::BottleCheckStatusType_t),
                      mp_Settings, BottleCheckReply(const QString&, DataManager::BottleCheckStatusType_t));

    mp_StatusBarManager = MainMenu::CStatusBarManager::GetInstance();
    CONNECTSIGNALSLOTGUI(mp_DataConnector,SetProcessStateIcon(bool &),mp_StatusBarManager,SetProcessState(bool &));
    CONNECTSIGNALSLOTGUI(mp_DataConnector,SetRemoteCareIcon(bool &),mp_StatusBarManager,SetRemoteCareState(bool &));

    m_MainWindow.AddMenuGroup(mp_Dashboard, QPixmap(QString(":/%1/Icons/Maintabs/Maintabs_Dashboard.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
    m_MainWindow.AddMenuGroup(mp_Programs, QPixmap(QString(":/%1/Icons/Maintabs/Maintabs_Programs.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
    m_MainWindow.AddMenuGroup(mp_Reagents, QPixmap(QString(":/%1/Icons/Maintabs/Maintabs_Reagents.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
    m_MainWindow.AddMenuGroup(mp_Settings, QPixmap(QString(":/%1/Icons/Maintabs/Maintabs_Settings.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
    m_MainWindow.AddMenuGroup(mp_Users, QPixmap(QString(":/%1/Icons/Maintabs/Maintabs_User.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CStartup::~CStartup()
{
    try {
        delete mp_Users;
        delete mp_Settings;
        delete mp_Reagents;
        delete mp_Programs;
        delete mp_Dashboard;
        delete mp_KeyBoardWidget;
        delete mp_DataConnector;
        delete mp_ScreenSaver;
    }
    catch (...) {}
}

} // end namespace Core
