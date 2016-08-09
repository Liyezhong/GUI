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
#include "Diagnostics/Include/Display.h"
#include "DiagnosticsManufacturing/Include/OvenManufacturing.h"
#include "DiagnosticsManufacturing/Include/MainControlManufacturing.h"
#include "DiagnosticsManufacturing/Include/LaSystemManufacturing.h"
#include "DiagnosticsManufacturing/Include/RotaryValveManufacturing.h"
#include "DiagnosticsManufacturing/Include/SystemManufacturing.h"
#include "DiagnosticsManufacturing/Include/CleaningManufacturing.h"
#include "DiagnosticsManufacturing/Include/RetortManufacturing.h"
#include "Core/Include/ServiceDefines.h"

namespace Core {

/****************************************************************************/
/*!
 *  \brief This class implements Diagnostics functions for Manufacturing SW
 */
/****************************************************************************/
class CManufacturingDiagnosticsHandler : public QObject {

    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief Constructor
     *  \iparam p_ServiceGUIConnector = serivce gui connector
     *  \iparam p_MainWindow = parent windows
     */
    /****************************************************************************/
    CManufacturingDiagnosticsHandler(Core::CServiceGUIConnector *p_ServiceGUIConnector,
                                     MainMenu::CMainWindow *p_MainWindow);

    /****************************************************************************/
    /*!
     *  \brief Destructor
     */
    /****************************************************************************/
    ~CManufacturingDiagnosticsHandler();

    /****************************************************************************/
    /*!
     *  \brief To load manufacturing diagnostics components.
     */
    /****************************************************************************/
    void LoadManufDiagnosticsComponents();

    /****************************************************************************/
    /*!
     *  \brief To show message dialog
     *  \iparam Message = message text
     */
    /****************************************************************************/
    void ShowMessage(const QString &Message);

    /****************************************************************************/
    /*!
     *  \brief To hide message dialog
     */
    /****************************************************************************/
    void HideMessage();

    /****************************************************************************/
    /*!
     *  \brief To show current pressure
     *  \iparam Message = message text
     */
    /****************************************************************************/
    void ShowCurrentPressure(const QString &Message);

private:
    Core::CServiceGUIConnector                  *mp_ServiceConnector;       //!< Service GUI connector object
    MainMenu::CMainWindow                       *mp_MainWindow;             //!< The main window of the application

    MainMenu::CMenuGroup                        *mp_DiagnosticsManufGroup;  //!< Menu group containing Diagnostics 1 for Manuacturing Sub menu's


    Diagnostics::CDisplay                       *mp_DisplayManuf;                //!< Display tests
    DiagnosticsManufacturing::COven             *mp_OvenManuf;              //!< Oven object provides tests for oven
    DiagnosticsManufacturing::CMainControl      *mp_MainControlManuf;       //!< Main Control object provides tests for main control
    DiagnosticsManufacturing::CLaSystem         *mp_LaSystemManuf;          //!< LaSystem object provides tests for Liquid and Air System
    DiagnosticsManufacturing::CRotaryValve      *mp_RotaryValveManuf;       //!< Rotary Valve object provides tests for rotary valve
    DiagnosticsManufacturing::CSystem           *mp_SystemManuf;            //!< System object provides tests for system
    DiagnosticsManufacturing::CRetort           *mp_RetortManuf;            //!< Retort object provides tests for retort
    DiagnosticsManufacturing::CCleaning         *mp_CleaningManuf;          //!< Cleaning system object provides test for clean system

    QEventLoop                                   m_LoopManufacturingTest;    //!< Loop for blocking Manufacturing Test command

    MainMenu::CMessageDlg                       *mp_WaitDialog;             //!< Displayed when busy


    //QString                                      m_FailStr;                  //!< - Fail
    //QString                                      m_SuccessStr;              //!< - Success

    bool ShowGuide(Service::ModuleTestCaseID Id, int Index = 0, bool haveNext = true);
    void ShowHeatingFailedResult(Service::ModuleTestCaseID Id);
    quint8 GetPositionForRVTest(Service::ModuleTestCaseID Id, int Index);
    void SelectOptionFor110v220Switch();
    bool ShowConfirmDlgForRVSelecting(quint8 Position);
    bool ShowConfirmDlgForRVSealing(quint8 Position);
    bool ShowConfirmDlgForSystemVentilationFan();
    bool ShowConfirmDlgForSystemSealing();
    void PerformManufDisplayTests(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformManufOvenTests(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformManufMainControlTests(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformManufRVTests(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformManufRetortTests(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformManufLATests(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformManufSystemTests(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformManufCleaningSystem(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformFirmwareUpdate(const QList<Service::ModuleTestCaseID> &TestCaseList);
    void PerformOperationTimeReset(const QList<Service::ModuleTestCaseID> &TestCaseList);


    bool GetTestResponse();

    void LockTest(bool LockFlag);

signals:
    /****************************************************************************/
    /**
       * \brief Signal emitted to perform manufacturing tests
       * \iparam Test = Test name
       * \iparam AbortId = Abort Id
       */
    /****************************************************************************/
    void PerformManufacturingTest(Service::ModuleTestCaseID Test, Service::ModuleTestCaseID AbortId=Service::TEST_CASE_ID_UNUSED);

    /*******************************************************************************/
    /*!
     *  \brief Signal emitted to ResetCarbonFilterLifeTimeComplete
     *  \iparam result = Reset result
     */
    /*******************************************************************************/
    void ResetCarbonFilterLifeTimeComplete(bool result);

public slots:

    /****************************************************************************/
    /*!
     *  \brief Slot for reset carbon filter life cycle
     */
    /****************************************************************************/
    void ResetCarbonFilterLifeTime(void);

    /****************************************************************************/
    /*!
     *  \brief Slot for begin manufacturing test
     *  \iparam ModuleName = The name of module
     *  \iparam TestCaseList = The list of test case
     */
    /****************************************************************************/
    void BeginManufacturingSWTests(Service::ModuleNames_t ModuleName, const QList<Service::ModuleTestCaseID> &TestCaseList);

    /****************************************************************************/
    /*!
     *  \brief Slot for return manufacturing message
     *  \iparam Result = flag of result
     */
    /****************************************************************************/
    void OnReturnManufacturingMsg(bool Result);

    /****************************************************************************/
    /*!
     *  \brief Slot for show error message dialog
     *  \iparam Message = message text
     */
    /****************************************************************************/
    void ShowErrorMessage(const QString &Message);

};

} // end of namespace Core
#endif // CORE_MANUFACTURINGDIAGNOSTICSHANDLER_H
