/****************************************************************************/
/*! \file ManufacturingDiagnosticsHandler.cpp
 *
 *  \brief ManufacturingDiagnosticsHandler class implementation.
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
#include <QApplication>
#include "Global/Include/UITranslator.h"
#include "Core/Include/ManufacturingDiagnosticsHandler.h"
#include "Core/Include/CMessageString.h"
#include "DiagnosticsManufacturing/Include/OvenManufacturing.h"

namespace Core {

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_ServiceGUIConnector = GUI connector object
 *  \iparam p_MainWindow = Mainwindow object
 */
/****************************************************************************/
CManufacturingDiagnosticsHandler::CManufacturingDiagnosticsHandler(CServiceGUIConnector *p_ServiceGUIConnector,
                                                                   MainMenu::CMainWindow *p_MainWindow) :
    mp_ServiceConnector(p_ServiceGUIConnector),
    mp_MainWindow(p_MainWindow)
{

    //Diagnostics Manufacturing
    mp_DiagnosticsManufGroup = new MainMenu::CMenuGroup;
    mp_OvenManuf = new DiagnosticsManufacturing::COven(mp_ServiceConnector, mp_MainWindow);

    CONNECTSIGNALSLOTGUI(mp_OvenManuf, BeginModuleTest(Service::ModuleNames_t), this, BeginManufacturingSWTests(Service::ModuleNames_t));


    /* Manufacturing SW Reset status */
    CONNECTSIGNALSLOTGUI(mp_DiagnosticsManufGroup, PanelChanged(), mp_OvenManuf, ResetTestStatus());

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CManufacturingDiagnosticsHandler::~CManufacturingDiagnosticsHandler()
{
    try {
        delete mp_OvenManuf;
        delete mp_DiagnosticsManufGroup;
    }
    catch (...) {

    }
}

/****************************************************************************/
/*!
 *  \brief Adds GUI Diagnostics Manufactruing SW Components
 */
/****************************************************************************/
void CManufacturingDiagnosticsHandler::LoadManufDiagnosticsComponents()
{
//    mp_DiagnosticsManufGroup->Clear();
    //Diagnostics
    mp_DiagnosticsManufGroup->AddPanel(QApplication::translate("Core::CManufacturingDiagnosticsHandler",
                                       "Oven", 0, QApplication::UnicodeUTF8), mp_OvenManuf);
    mp_MainWindow->AddMenuGroup(mp_DiagnosticsManufGroup, QApplication::translate
                   ("Core::CManufacturingDiagnosticsHandler", "Diagnostics", 0, QApplication::UnicodeUTF8));
}

/****************************************************************************/
/*!
 *  \brief Slot called for Module tests for manufacturing SW
 *  \iparam ModuleName = Name of the module
 */
/****************************************************************************/
void CManufacturingDiagnosticsHandler::BeginManufacturingSWTests(Service::ModuleNames_t ModuleName)
{
    qDebug()<<"CManufacturingDiagnosticsHandler::BeginManufacturingSWTests : ModuleName="<<ModuleName;
    switch(ModuleName) {
    case Service::OVEN:
        PerformManufOvenTests();
        break;
    default:
        break;
    }
}

/****************************************************************************/
/*!
 *  \brief Heated Cuvettes Test response
 *  \return true or false
 */
/****************************************************************************/
bool CManufacturingDiagnosticsHandler::GetTestResponse()
{
    mp_ServiceConnector->ShowBusyDialog();
    QTimer timer;
    qint32 ret;
    timer.setSingleShot(true);
    timer.setInterval(900000);
    timer.start();
    CONNECTSIGNALSLOT(&timer, timeout(), &m_LoopManufacturingTest, quit());
    ret = m_LoopManufacturingTest.exec();
    mp_ServiceConnector->HideBusyDialog();
    if (ret != 1)
    {
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief Function called for Module tests for manufacturing SW
 */
/****************************************************************************/
void CManufacturingDiagnosticsHandler::PerformManufOvenTests()
{
    qDebug()<<"CManufacturingDiagnosticsHandler::PerformManufOvenTests ---";
    emit PerformManufacturingTest(Service::OVEN_COVER_SENSOR);
#if 0
    QString Title;
    QString GBox;
    QString Instr;

    qDebug()<<"CManufacturingDiagnosticsHandler::PerformManufOvenTests -- emit ASB3_0_FWUPDATE";

    emit PerformManufacturingTest(Service::ASB3_0_FWUPDATE);
    if(!GetTestResponse())
    {
        mp_OvenManuf->SetTestResult(Service::OVEN_FIRMWARE_UPDATE, false);
        Title = ( QApplication::translate("Core::CStartup", "Firmware Update Failed", 0, QApplication::UnicodeUTF8));
        GBox = Service::CMessageString::MSG_ASB3_0_FWUPDATE_FAILURE;
        Instr = ( QApplication::translate("Core::CStartup", "Firmware update failed ! Are you sure you want to continue with the tests.\nPress Next button to continue and Abort button to abort.",
                                          0, QApplication::UnicodeUTF8));
        mp_ServiceConnector->SetPopUpDialogBackNext(Title, GBox, Instr);
        MainMenu::ButtonPress_t Response1 = mp_ServiceConnector->GetPopUpResponse();
        mp_ServiceConnector->ClosePopUp();
        if (MainMenu::BTN_NEXT_PRESSED != Response1)
        {
            return;
        }
    }
    else
    {
        mp_OvenManuf->SetTestResult(Service::OVEN_FIRMWARE_UPDATE, true);
    }

    emit PerformManufacturingTest(Service::ASB12_0_FWUPDATE);
    if(!GetTestResponse())
    {
        Title = ( QApplication::translate("Core::CStartup", "Firmware Update Failed", 0, QApplication::UnicodeUTF8));
        GBox = Service::CMessageString::MSG_ASB12_0_FWUPDATE_FAILURE;
        Instr = ( QApplication::translate("Core::CStartup", "Firmware update failed ! Are you sure you want to continue with the tests.\nPress Next button to continue and Abort button to abort.",
                                          0, QApplication::UnicodeUTF8));
        mp_ServiceConnector->SetPopUpDialogBackNext(Title, GBox, Instr);
        MainMenu::ButtonPress_t Response1 = mp_ServiceConnector->GetPopUpResponse();
        mp_ServiceConnector->ClosePopUp();
        if (MainMenu::BTN_NEXT_PRESSED != Response1)
        {
            return;
        }
    }

    (void) OnOvenHeatingTest();

    emit PerformManufacturingTest(Service::OVEN_PHOTOSENSOR_TEST);
    if(!GetTestResponse())
    {
        mp_OvenManuf->SetTestResult(Service::OVEN_PHOTOSENSOR_TEST, false);
    }
    else
    {
        mp_OvenManuf->SetTestResult(Service::OVEN_PHOTOSENSOR_TEST, true);
    }

    QString Title_ =  QApplication::translate("Core::CManufacturingDiagnosticsHandler", "Slide Count",
                                              0, QApplication::UnicodeUTF8);
    QString GBox_ =  QApplication::translate("Core::CManufacturingDiagnosticsHandler", "Slide Count",
                                             0, QApplication::UnicodeUTF8);
    QString Instr_ =  QApplication::translate("Core::CManufacturingDiagnosticsHandler",
                      "Place the golden standard rack with 30 slides in R29 station. Do you confirm the rack is in place? ",
                                              0, QApplication::UnicodeUTF8);
    mp_ServiceConnector->SetPopUpDialogYesNo(Title_, GBox_, Instr_);
    MainMenu::ButtonPress_t Response_1 = mp_ServiceConnector->GetPopUpResponse();
    mp_ServiceConnector->ClosePopUp();
    if ((MainMenu::BTN_ABORT_PRESSED == Response_1) ||
            (MainMenu::BTN_CANCEL_PRESSED == Response_1) ||
            (MainMenu::BTN_NO_PRESSED == Response_1)) {
        ShowMessage( QApplication::translate("Core::CManufacturingDiagnosticsHandler", "Manufacturing Test Aborted!",
                                             0, QApplication::UnicodeUTF8));
        return;
    }
    emit PerformManufacturingTest(Service::OVEN_SLIDECOUNT_TEST);
    if(!GetTestResponse())
    {
        mp_OvenManuf->SetTestResult(Service::OVEN_SLIDECOUNT_TEST, false);
    }
    else
    {
        mp_OvenManuf->SetTestResult(Service::OVEN_SLIDECOUNT_TEST, true);
    }

    emit PerformManufacturingTestCANID(Service::OVEN_CANID);
    if(!GetTestResponse())
    {
        mp_OvenManuf->SetTestResult(Service::OVEN_CANID, false);
    }
    else
    {
        mp_OvenManuf->SetTestResult(Service::OVEN_CANID, true);
    }
#endif
}


/****************************************************************************/
/*!
 *  \brief Returns message for manufacturing tests
 *  \iparam Result = true or false
 */
/****************************************************************************/
void CManufacturingDiagnosticsHandler::OnReturnManufacturingMsg(bool Result)
{
    qint32 ret(-1);
    if(Result) {
        ret = 1;
    }
    if (m_LoopManufacturingTest.isRunning()) {
        m_LoopManufacturingTest.exit(ret);
    } else {
        qDebug()<<"NOTICE: Unexpected action acknowledgement";
    }
}

/****************************************************************************/
/*!
 *  \brief Throws open a Pop Up Message with Custom Messages
 *  \iparam Message = Dialog title
 */
/****************************************************************************/
void CManufacturingDiagnosticsHandler::ShowMessage(const QString &Message)
{
    mp_ServiceConnector->HideBusyDialog();
    mp_ServiceConnector->ShowMessageDialog(Global::GUIMSGTYPE_INFO, Message);
}

/****************************************************************************/
/*!
 *  \brief Throws open a Pop Up Message with Custom Messages
 *  \iparam Message = Dialog title
 */
/****************************************************************************/
void CManufacturingDiagnosticsHandler::ShowErrorMessage(const QString &Message)
{
    mp_ServiceConnector->HideBusyDialog();
    mp_ServiceConnector->ShowMessageDialog(Global::GUIMSGTYPE_ERROR, Message);
}

} // end of namespace Core
