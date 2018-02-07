/****************************************************************************/
/*! \file ProgramStartableManager.h
 *
 *  \brief Definition file for class ProgramStartableMaanger
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-11-23
 *  $Author:    $ Bhanu Prasad H
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
#ifndef PROGRAMSTARTABLEFLAGMANAGER_H
#define PROGRAMSTARTABLEFLAGMANAGER_H
//Qt Includes
#include <QObject>
#include <QString>
#include <QStringList>

//Project Includes
#include <Global/Include/GlobalDefines.h>

namespace Threads {
    class CommandChannel;
}

namespace DataManager {
    class CDataManager;
    class CUserSettingsInterface;
    class CUserSettings;
    class CDataProgramList;
}

namespace Himalaya {
class HimalayaMasterThreadController;
static const QString UNLOADER_STEP = "S7";
static const QString TRANSFER_STEP = "S8";
static const QString DISTILLED_WATER_STEP = "S6";

/****************************************************************************/
/**
 * \brief This class shall contain the code to manage the Dynamic Startable
 *        Program Flag and Static Flag in some cases.
 *        The object created shall have affinity to Himalaya
 *        Master thread. The components on Himalaya Master thread shall
 *        communicate using Signal slot mechanism, Components on other thread
 *        shall communicate through commands.
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class ProgramStartableManager : public QObject {
    Q_OBJECT
private:
    //Data Members
    Himalaya::HimalayaMasterThreadController *mp_HimalayaMasterThreadController; //!< TestHimalayaMasterThreadController object
    // The below pointers to containers are used so that every method in the class
    // doesnt have to retrieve it every time. The below pointers are initialized
    // in constructor and can be used throughout the class.
    DataManager::CDataManager *mp_DataManager; //!< The DataManager
    DataManager::CUserSettings *mp_UserSettings; //!< User settings
    DataManager::CUserSettingsInterface *mp_UserSettingsInterface; //!< User settings interface
    DataManager::CDataProgramList *mp_ProgramList; //!< ProgramList
    //-------------End Of DataMembers------------------------//
    Q_DISABLE_COPY(ProgramStartableManager) //!< Disable copy and assignment.
    ProgramStartableManager(); //!< Not Implemented
    void ConnectSignals();
    void UpdateGUI() const;

public:
    ProgramStartableManager(Himalaya::HimalayaMasterThreadController *p_HimalayaMasterThreadController);
    QStringList CheckUseOfFiveSlideReagent(QString ProgramID);
    void Init();

public slots:
    void OnUserSettingsChanged();
};

}//End Of namespace Himalaya
#endif // PROGRAMSTARTABLEFLAGMANAGER_H
