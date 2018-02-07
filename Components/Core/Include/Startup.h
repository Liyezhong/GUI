/****************************************************************************/
/*! \file Himalaya/HimalayaMain/Gui/Components/Core/Include/Startup.h
 *
 *  \brief Startup definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-31
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

//lint -e1536

#ifndef CORE_STARTUP_H
#define CORE_STARTUP_H

#include "Core/Include/DataConnector.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/StatusBarManager.h"

#include "Dashboard/Include/ScreenSaverWidget.h"
#include "Programs/Include/ProgramWidget.h"
#include "Reagents/Include/ReagentWidget.h"
#include "Settings/Include/SettingsWidget.h"
#include "Users/Include/UserPrivilegeWidget.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "MainMenu/Include/DataManagement.h"

class SplashWidget;

namespace Dashboard
{
    class CDashboardWidget;
}

namespace Core {

class CGlobalHelper;

/****************************************************************************/
/**
 * \brief This class handles the initialization of the user interface.
 */
/****************************************************************************/
class CStartup : public QObject
{
    Q_OBJECT

public:
    CStartup();
    virtual ~CStartup();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function InitialiseDeviceMode
     *
     *  \param Mode = QString type parameter
     *
     *  \return from InitialiseDeviceMode
     */
    /****************************************************************************/
    void InitialiseDeviceMode(QString Mode);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function instance
     *
     *  \return from instance
     */
    /****************************************************************************/
    static CStartup* instance() { return self; }
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function MainWindow
     *
     *  \return from MainWindow
     */
    /****************************************************************************/
    MainMenu::CMainWindow* MainWindow() { return &m_MainWindow; }
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function DataConnector
     *
     *  \return from DataConnector
     */
    /****************************************************************************/
    Core::CDataConnector* DataConnector() { return mp_DataConnector; }
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Dashboard
     *
     *  \return from Dashboard
     */
    /****************************************************************************/
    Dashboard::CDashboardWidget* Dashboard() { return mp_Dashboard; }
private:
    Core::CGlobalHelper* mp_GlobalHelper;       ///<  Definition/Declaration of variable mp_GlobalHelper
    // Data Manager Components
    Core::CDataConnector *mp_DataConnector; //!< Connects data manager to the network
    // GUI components
    MainMenu::CMainWindow m_MainWindow;     //!< The main window of the application
    Dashboard::CDashboardWidget *mp_Dashboard;  //!< Dashboard screen
    Programs::CProgramWidget *mp_Programs;  //!< Programs screen
    Reagents::CReagentWidget *mp_Reagents; //!< Reagents screen
    Settings::CSettingsWidget *mp_Settings; //!< Settings screen
    Users::CUserPrivilegeWidget *mp_Users;  //!< Users screen
    ScreenSaverWidget* mp_ScreenSaver;       ///<  Definition/Declaration of variable mp_ScreenSaver
    KeyBoard::CKeyBoard *mp_KeyBoardWidget; //!< On-screen Keyboard
    QString m_Mode;							//!< GUI mode
    static CStartup *self;       ///<  Definition/Declaration of variable self
    SplashWidget    *mp_SplashWidget;       ///<  Definition/Declaration of variable mp_SplashWidget
    MainMenu::CStatusBarManager *mp_StatusBarManager; //!< Statusbar Manager instance

};

} // end namespace Core

#endif // CORE_STARTUP_H

