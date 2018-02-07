/****************************************************************************/
/*! \file Himalaya/HimalayaMain/Gui/Components/Main/Source/Main.cpp
 *
 *  \brief Main implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-17
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

#include "Application/Include/Application.h"
#include "Core/Include/Startup.h"
#include <QTranslator>
#include <QDir>
#include "Global/Include/SignalHandler.h"
#include <Global/Include/SystemPaths.h>

/****************************************************************************/
/*!
 *  \brief The program's main function.
 *
 *  \iparam Argc = Command line argument count
 *  \iparam p_Argv = List of command line arguments
 *  \return returns the code set to exit() method
 */
/****************************************************************************/
int main(int Argc, char *p_Argv[])
{
    Global::SignalHandler signalHandler;
    signalHandler.init();

    //lint -esym(526, qInitResources_*)
    //lint -esym(534, qInitResources_*)
    Q_INIT_RESOURCE(MainMenu);
    Q_INIT_RESOURCE(Application);
    Q_INIT_RESOURCE(KeyBoard);
    Q_INIT_RESOURCE(SpellerButtons);
    Q_INIT_RESOURCE(Speller);

//    Global::InitSupportedLanguages();// set global directories.
    Application::CApplication App(Argc, p_Argv, Application::DEVICE_HIMALAYA);
    Global::SystemPaths::Instance().SetComponentTestPath("../Tests");
    Core::CStartup StartUp;

    return App.exec();
}
