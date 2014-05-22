/****************************************************************************/
/*! \file ManufacturingDiagnosticsHandler.h
 *
 *  \brief ManufacturingDiagnosticsHandler class definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-05-12
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
#ifndef CORE_MANUFACTURINGDIAGNOSTICSHANDLER_H
#define CORE_MANUFACTURINGDIAGNOSTICSHANDLER_H

#include "Core/Include/ServiceGUIConnector.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/MenuGroup.h"
#include "DiagnosticsManufacturing/Include/OvenManufacturing.h"

namespace Core {

/****************************************************************************/
/*!
 *  \brief This class implements Diagnostics functions for Manufacturing SW
 */
/****************************************************************************/
class CManufacturingDiagnosticsHandler : public QObject {

    Q_OBJECT

public:
    CManufacturingDiagnosticsHandler(Core::CServiceGUIConnector *p_ServiceGUIConnector,
                                     MainMenu::CMainWindow *p_MainWindow);
    ~CManufacturingDiagnosticsHandler();

    void LoadManufDiagnosticsComponents();

private:
    Core::CServiceGUIConnector                  *mp_ServiceConnector;       //!< Service GUI connector object
    MainMenu::CMainWindow                       *mp_MainWindow;             //!< The main window of the application

    MainMenu::CMenuGroup                        *mp_DiagnosticsManufGroup;  //!< Menu group containing Diagnostics 1 for Manuacturing Sub menu's

    DiagnosticsManufacturing::COven             *mp_OvenManuf;              //!< Oven object provides tests for oven

    QEventLoop                                   m_LoopManufacturingTest;    //!< Loop for blocking Manufacturing Test command

    void PerformManufOvenTests();
    bool GetTestResponse();

signals:
    /****************************************************************************/
    /**
       * \brief Signal emitted to perform manufacturing tests
       * \iparam Test = Test name
       */
    /****************************************************************************/
    void PerformManufacturingTest(Service::ModuleTestNames Test);

public slots:
    void BeginManufacturingSWTests(Service::ModuleNames_t);
    void OnReturnManufacturingMsg(bool Result);

    /* Return Message Slots */
    void ShowMessage(const QString &Message);
    void ShowErrorMessage(const QString &Message);

};

} // end of namespace Core
#endif // CORE_MANUFACTURINGDIAGNOSTICSHANDLER_H