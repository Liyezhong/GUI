/****************************************************************************/
/*! \file DataManager.cpp
 *
 *  \brief Implementation file for class CDataManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-20
 *  $Author:    $ F. Toth
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

#include <QDebug>
#include <Global/Include/SystemPaths.h>
#include "../Include/DataManager.h"
#include "../CommandInterface/Include/ProgramCommandInterface.h"
#include "../CommandInterface/Include/ReagentCommandInterface.h"
#include "../CommandInterface/Include/ReagentGroupCommandInterface.h"
#include "../CommandInterface/Include/DashboardStationCommandInterface.h"
namespace DataManager {

CDataManager::CDataManager(Threads::MasterThreadController *p_HimalayaMasterThreadController) :CDataManagerBase(p_HimalayaMasterThreadController),
    mp_DataContainer(NULL),
    mp_ReagentCommandInterface(NULL),
    mp_DashboardStationCommandInterface(NULL),
    mp_ProgramCommandInterface(NULL)
{
    quint32 ReturnCode  = InitDataContainer();
    if (ReturnCode != INIT_OK) {
        qDebug() << "CDataManager::Constructor / InitDataContainer failed";
        Global::EventObject::Instance().RaiseEvent(ReturnCode);
    }
}

CDataManager::CDataManager(Threads::MasterThreadController *p_HimalayaMasterThreadController, QString Path) :CDataManagerBase(p_HimalayaMasterThreadController),
    mp_DataContainer(NULL),
    mp_ProgramCommandInterface(NULL)
{
    quint32 ReturnCode = InitDataContainer();
    if (ReturnCode != INIT_OK) {
        Global::EventObject::Instance().RaiseEvent(ReturnCode);
    }
}

CDataManager::~CDataManager()
{
    if (!DeinitDataContainer()) {
        qDebug() << "CDataManager::Destructor / DeinitDataContainer failed";
    }
}

quint32 CDataManager::InitDataContainer()
{
    quint32 ReturnCode = INIT_OK;
    //Create DataContainer - Inturn creates individual containers
    mp_DataContainer = new CDataContainer(mp_MasterThreadController);
    mp_DataContainerCollectionBase = mp_DataContainer;
    ReturnCode = CDataManagerBase::InitDataContainer();
    if ( ReturnCode != INIT_OK) {
        return ReturnCode;
    }

    mp_DataContainer->ProgramList->SetDataVerificationMode(false);
    QString FilenameProgramList = Global::SystemPaths::Instance().GetSettingsPath() + "/Programs.xml";
    if (!mp_DataContainer->ProgramList->Read(FilenameProgramList)) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ProgramList->Read failed with filename: " << FilenameProgramList;
        return EVENT_DM_PROGRAM_XML_READ_FAILED;
    }

    mp_DataContainer->DashboardStationList->SetDataVerificationMode(false);
    QString FilenameDashboardStationList = Global::SystemPaths::Instance().GetSettingsPath() + "/HimalayaStations.xml";
    if (!mp_DataContainer->DashboardStationList->Read(FilenameDashboardStationList)) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->DashboardStationList->Read failed with filename: " << FilenameDashboardStationList;
        return EVENT_DM_REAGENT_XML_READ_FAILED;
    }

    mp_DataContainer->ReagentList->SetDataVerificationMode(false);
    QString FilenameReagentList = Global::SystemPaths::Instance().GetSettingsPath() + "/HimalayaReagents.xml";
    if (!mp_DataContainer->ReagentList->Read(FilenameReagentList)) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ReagentList->Read failed with filename: " << FilenameReagentList;
        return EVENT_DM_REAGENT_XML_READ_FAILED;
    }

    mp_DataContainer->ReagentGroupList->SetDataVerificationMode(false);
    QString FilenameReagentGroupList = Global::SystemPaths::Instance().GetSettingsPath() + "/HimalayaReagentGroups.xml";
    if (!mp_DataContainer->ReagentGroupList->Read(FilenameReagentGroupList)) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ReagentGroupList->Read failed with filename: " << FilenameReagentList;
        return EVENT_DM_REAGENT_XML_READ_FAILED;
    }


    // when all containers are loaded, activate verification mode and verify there initial content
    mp_DataContainer->ProgramList->SetDataVerificationMode(true);
    mp_DataContainer->ReagentList->SetDataVerificationMode(true);
    mp_DataContainer->ReagentGroupList->SetDataVerificationMode(true);

    if (!mp_DataContainer->ProgramList->VerifyData()) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ProgramList->VerifyData failed.";
        return EVENT_DM_PROGRAM_VERIFICATION_FAILED;
    }

    if (!mp_DataContainer->ReagentList->VerifyData()) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ReagentList->VerifyData failed.";
        return EVENT_DM_REAGENT_VERIFICATION_FAILED;
    }

    if (!mp_DataContainer->ProgramList->VerifyData(true)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_HIMALAYA_DM_GV_FAILED);
    }

    if (!mp_DataContainer->SettingsInterface->VerifyData(true)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_HIMALAYA_DM_GV_FAILED);
    }
    Himalaya::HimalayaMasterThreadController *p_HimalayaMasterThread;

    mp_ProgramCommandInterface = new CProgramCommandInterface(this, mp_MasterThreadController, mp_DataContainer);
    mp_ReagentCommandInterface = new CReagentCommandInterface(this, mp_MasterThreadController, mp_DataContainer);
    mp_ReagentGroupCommandInterface = new CReagentGroupCommandInterface(this, mp_MasterThreadController, mp_DataContainer);
    mp_DashboardStationCommandInterface = new CDashboardStationCommandInterface(this, mp_MasterThreadController, mp_DataContainer);
    m_IsInitialized = true;
    return INIT_OK;
}

bool CDataManager::DeinitDataContainer()
{
    if (!CDataManagerBase::DeinitDataContainer()) {
        return false;
    }
    delete mp_ProgramCommandInterface;
    delete mp_ReagentCommandInterface;
    delete mp_ReagentGroupCommandInterface;
    delete mp_DashboardStationCommandInterface;
    delete mp_DataContainer;
    return true;
}

//get copy of complete station list
CDataReagentList* CDataManager::GetReagentList()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetStationList failed. Not initialized!";
        return NULL;
    }

    return mp_DataContainer->ReagentList;
}

//********************************************************************
// some function to access the station list
//********************************************************************

//get copy of complete station list
CDataReagentGroupList* CDataManager::GetReagentGroupList()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetReagentGroupList failed. Not initialized!";
        return NULL;
    }

    return mp_DataContainer->ReagentGroupList;
}

//get copy of complete station list
CDashboardDataStationList* CDataManager::GetDashboardStationList()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetDashboardStationList failed. Not initialized!";
        return NULL;
    }

    return mp_DataContainer->DashboardStationList;
}

CDataProgramList* CDataManager::GetProgramList() {
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetReagentList failed. Not initialized!";
        return NULL;
    }
    return mp_DataContainer->ProgramList;
}

bool CDataManager::GetProgramList(CDataProgramList *p_ProgramList)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetReagentList failed. Not initialized!";
        return false;
    }
    if (p_ProgramList) {
        *p_ProgramList = *(mp_DataContainer->ProgramList);
        return true;
    }
    return false;
}

bool CDataManager::GetReagentList(CDataReagentList *p_ReagentList)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetReagentList failed. Not initialized!";
        return false;
    }
    if (p_ReagentList) {
        *p_ReagentList = *(mp_DataContainer->ReagentList);
        return true;
    }
    return false;
}

bool CDataManager::GetReagentGroupList(CDataReagentGroupList *p_ReagentGroupList)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetReagentGroupList failed. Not initialized!";
        return false;
    }
    if (p_ReagentGroupList) {
        *p_ReagentGroupList = *(mp_DataContainer->ReagentGroupList);
        return true;
    }
    return false;
}


CUserSettingsInterface *CDataManager::GetUserSettingsInterface()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetReagentList failed. Not initialized!";
        return NULL;
    }
    return mp_DataContainer->SettingsInterface;
}

CDeviceConfigurationInterface *CDataManager::GetDeviceConfigurationInterface()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetDeviceConfigurationInterface failed. Not initialized!";
        return NULL;
    }
    return mp_DataContainer->DeviceConfigurationInterface;
}


} // namespace DataManager
