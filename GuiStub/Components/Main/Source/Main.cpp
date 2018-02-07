/****************************************************************************/
/*! \file Main.cpp
 *
 *  \brief Implementation file for main routine.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-12-26
 *  $Author:    $ BhanuPrasad Hulsoor
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

#include <Main/Include/Main.h>
#include <getopt.h>
#include <Main/Include/Startup.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>
#include <QtGui/QLabel>
#include <QThread>
#include <TestHimalayaMasterThread/Include/TestHimalayaMasterThreadController.h>
//Scheduler Demo includes
//#include <FunctionModules/defStepperMotor.h>

static const QString Version = "GUIST_0.002"; ///< version string for the export component


#define GUI_TEST_SW 1
/****************************************************************************/
int main(int Argc, char *p_Argv[]) {

    QApplication App(Argc, p_Argv);

    int CmdLineOption;

    const struct option CmdLongOptions[] = {
        {"version",   no_argument,        0, 'v'},
        {"help",      no_argument,        0, 'h'},
        {0,0,0,0},
    };

    CmdLineOption = getopt_long (Argc, p_Argv, "vh", CmdLongOptions, NULL);

    switch (CmdLineOption)  {
        case 'h':
            std::cout << "Usage: " << p_Argv[0] << " [OPTION]" << std::endl;
            std::cout << "  -v, --version    output version information and exit" << std::endl;
            std::cout << "  -h, --help       display this help and exit" << std::endl;
            return 0;

        case 'v':
            std::cout << Version.toStdString() << std::endl;
            return 0;

        default:
            break;
    }

    Global::InitSupportedLanguages();

    // set global directories.
    Global::SystemPaths::Instance().SetComponentTestPath("../Tests");
    Global::SystemPaths::Instance().SetFirmwarePath("../Firmware");
    Global::SystemPaths::Instance().SetLogfilesPath("../Logfiles");
    Global::SystemPaths::Instance().SetManualPath("../Manual");
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");
    Global::SystemPaths::Instance().SetUpdatePath("../Update");
    Global::SystemPaths::Instance().SetUploadsPath("../Uploads");
    Global::SystemPaths::Instance().SetTranslationsPath("../Translations");
    Global::SystemPaths::Instance().SetTempPath("../Temporary");


    // create master thread
    QThread thrMasterThread;
    // create controller for master thread
    Himalaya::HimalayaMasterThreadController TheMasterThreadController;
    // set OperatingMode to "production"
    TheMasterThreadController.SetOperatingMode("production");
    // base file name for logging is "Events"
    TheMasterThreadController.SetEventLoggerBaseFileName("Events");

    TheMasterThreadController.SetSerialNumber("1234");
    /// \todo set max file size of event logger
    TheMasterThreadController.SetEventLoggerMaxFileSize(1000000);
    // set max file count for day operation logger
    TheMasterThreadController.SetDayEventLoggerMaxFileCount(14);
    // set max date time offset
    TheMasterThreadController.SetMaxAdjustedTimeOffset(24*3600);
    // read time offset from file
    if(!TheMasterThreadController.ReadTimeOffsetFile()) {
        // exit with error code
        return Global::RETCODE_TIME_OFFSET;
    }       

    // add code to quit Master thread if its controller requests it
    if(!QObject::connect(&TheMasterThreadController, SIGNAL(RequestFinish()), &thrMasterThread, SLOT(quit())))
    {
      Global::ToConsole("Connecting TheMasterThreadController::RequestFinish() and thrMasterThread::quit() failed");
      // exit with error code
      return Global::RETCODE_CONNECT_FAILED;
    }

    // Master thread should give its controller automatically a "Go" signal as soon it has started
    if(!QObject::connect(&thrMasterThread, SIGNAL(started()), &TheMasterThreadController, SLOT(Go()))) {
      Global::ToConsole("Connecting thrMasterThread::started() and TheMasterThreadController::Go() failed");
      // exit with error code
      return Global::RETCODE_CONNECT_FAILED;
     }

    // now push controller to master thread
    TheMasterThreadController.moveToThread(&thrMasterThread);

    // and start the Master thread
    thrMasterThread.start();

    // startup of the Test SW
    Startup StartTest(TheMasterThreadController);
    StartTest.GuiInit();

    int ReturnCode;

    ReturnCode = App.exec();

    // wait for Master thread
    if(!thrMasterThread.wait()) {
        Global::ToConsole("Error waiting for thrMasterThread");
        // set error code
        ReturnCode = Global::RETCODE_MASTERTHREAD_WAIT;
    }
    // cleanup controller for master thread.
    TheMasterThreadController.CleanupAndDestroyObjects();    

    return ReturnCode;

}

