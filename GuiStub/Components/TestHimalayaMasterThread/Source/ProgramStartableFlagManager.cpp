/****************************************************************************/
/*! \file ProgramStartableFlagManager.cpp
 *
 *  \brief Implementation file for class ProgramStartableFlagManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-11-05
 *  $Author:    $ N.Kamath
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
//Project Headers
#include <TestHimalayaMasterThread/Include/ProgramStartableFlagManager.h>
#include <TestHimalayaMasterThread/Include/TestHimalayaMasterThreadController.h>
#include <HimalayaDeviceCommandProcessor/Include/Commands/CmdStationDefect.h>
#include <NetCommands/Include/CmdConfigurationFile.h>
#include <HimalayaDataContainer/Containers/StainingPrograms/Include/DataProgramList.h>
#include <Global/Include/EventObject.h>
#include <TestStubDataManager/CommandInterface/Include/UserSettingsCommandInterface.h>
#include "../../HimalayaDataManager/CommandInterface/Include/ProgramCommandInterface.h"
namespace Himalaya {

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_HimalayaMasterThreadController
 *
 */
/****************************************************************************/
ProgramStartableManager::ProgramStartableManager(HimalayaMasterThreadController *p_HimalayaMasterThreadController)
    :mp_HimalayaMasterThreadController(p_HimalayaMasterThreadController),
    mp_DataManager(NULL),
    mp_UserSettings(NULL),
    mp_UserSettingsInterface(NULL),
    mp_ProgramList(NULL) {
    //This object shall now live on himalaya master thread.
    this->setParent(p_HimalayaMasterThreadController);

}

/****************************************************************************/
/*!
 *  \brief Initializes the object
 *  \warning Catch expception thrown by CHECKPTR
 */
/****************************************************************************/
void ProgramStartableManager::Init() {
    //Initialize all pointers that will be used
    CHECKPTR(mp_HimalayaMasterThreadController);
    mp_DataManager = dynamic_cast<DataManager::CDataManager*>((const_cast<DataManager::CDataManagerBase *>(mp_HimalayaMasterThreadController->GetDataManager())));
    CHECKPTR(mp_DataManager);
    mp_UserSettingsInterface = mp_DataManager->GetUserSettingsInterface();
    CHECKPTR(mp_UserSettingsInterface)
    mp_UserSettings = mp_UserSettingsInterface->GetUserSettings();
    CHECKPTR(mp_UserSettings);
    mp_ProgramList = mp_DataManager->GetProgramList();
    CHECKPTR(mp_ProgramList);
    ConnectSignals();
}

/****************************************************************************/
/*!
 *  \brief Register the commands required to be handled
 */
/****************************************************************************/
void ProgramStartableManager::ConnectSignals() {
    CHECKPTR(mp_DataManager->mp_SettingsCommandInterface);
    CHECKPTR(mp_DataManager->mp_ProgramCommandInterface);
    CONNECTSIGNALSLOT(mp_DataManager->mp_SettingsCommandInterface, UserSettingsChanged(), this , OnUserSettingsChanged());
//    CONNECTSIGNALSLOT(mp_DataManager->mp_ProgramCommandInterface, StartableProgramEdited(const QString &), this , OnStartableProgramEdited(const QString &));
}




/****************************************************************************/
/*!
 *  \brief  Manage static Program Startable flag when User changes DI water
 *          type to Tap water type in Bathlayout screen.
 */
/****************************************************************************/
void ProgramStartableManager::OnUserSettingsChanged() {
//    Global::WaterType CurrentWaterType = Global::StringToWaterType(mp_UserSettings->GetValue("Water_Type"));
//    QStringList NonStartableProgramIDs;
//    QStringList StartableProgramIDList =  mp_ProgramSequenceList->GetStartableProgramIDList();
//    for (qint32 I = 0 ; I < StartableProgramIDList.size(); I++) {
//        DataManager::CProgram *p_Program = const_cast<DataManager::CProgram*>(mp_ProgramList->GetProgram(StartableProgramIDList.at(I)));
//        if (p_Program) {
//            for (qint32 I = 0; I < p_Program->GetNumberOfSteps(); I++) {
//                const DataManager::CProgramStep *p_ProgramStep = p_Program->GetProgramStep(I);
//                //Check: Current step is Distilled water and Current Water Type is not distilled water
//                if ((QString::compare(p_ProgramStep->GetReagentID(), DISTILLED_WATER_STEP) == 0)
//                        && QString::compare(p_ProgramStep->GetReagentID(), Global::WaterTypeToString(CurrentWaterType)) != 0) {
//                    //Store Program Ids of Programs which use DI water
//                    NonStartableProgramIDs << p_Program->GetID();
//                    //We dont need to iterate further, check next startable program
//                    continue;
//                }
//            }
//        }
//    }
//    //Now that we have Program IDs of Programs which are no longer startable update Program Sequence Container
//    QStringListIterator It(NonStartableProgramIDs);
//    while (It.hasNext()) {
//        DataManager::CProgramSequence *p_ProgramSequence = const_cast<DataManager::CProgramSequence*>(mp_ProgramSequenceList->GetProgramSequenceStep(It.next()));
//        if (p_ProgramSequence) {
//            p_ProgramSequence->SetStartCapableStatus(false);
//            p_ProgramSequence->SetStartCapableResult("2");
//        }
//    }
//    if (NonStartableProgramIDs.size() >= 1) {
//        //Log and inform EventHandler
//        Global::EventObject::Instance().RaiseEvent(EVENT_DI_WATER_NOT_AVAILABLE);
//        UpdateGUI();
//    }
    if (mp_DataManager->GetUserSettingsInterface()->GetUserSettings(false) != NULL) {
            // get the current language
            QLocale::Language CurrentLanguage =
                    mp_DataManager->GetUserSettingsInterface()->GetUserSettings(false)->GetLanguage();

            // store the langauge name de_DE
            QString LanguageName(QLocale(CurrentLanguage).name());
            // remove the DE
            LanguageName.truncate(LanguageName.lastIndexOf('_'));
            qDebug() << "Language Name ############################" << LanguageName << "#########################";

            QString LanguageFileName = "Himalaya_" + LanguageName + ".qm";
            mp_HimalayaMasterThreadController->SendLanguageFileToGUI(LanguageFileName);
        }

}



/****************************************************************************/
/*!
 *  \brief  Sends GUI the Program Sequence Container
 */
/****************************************************************************/
void ProgramStartableManager::UpdateGUI() const {
    QByteArray BA;
    QDataStream ProgramSequeneStream(&BA, QIODevice::ReadWrite);
    ProgramSequeneStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
}

}//End of namespace Himalaya
