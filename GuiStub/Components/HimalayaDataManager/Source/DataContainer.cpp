/****************************************************************************/
/*! \file DataContainer.cpp
 *
 *  \brief Implementation file for class CDataContainer.
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
#include  "../Include/DataContainer.h"

#include "HimalayaDataContainer/Containers/StainingPrograms/Include/DataProgramListVerifier.h"


#include "HimalayaDataContainer/Containers/StainingReagents/Include/DataReagentListVerifier.h"

#include "TestStubDataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationVerifier.h"
#include "TestStubDataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"

namespace DataManager {

CDataContainer::CDataContainer(Threads::MasterThreadController *p_MasterThreadController) :
    DataManager::CDataContainerCollectionBase(p_MasterThreadController),
    ProgramList(NULL),
    ReagentList(NULL)
{
    if (!InitContainers()) {
        qDebug() << "CDataContainer::Constructor / InitContainers failed";
    }
}

CDataContainer::~CDataContainer()
{
    if (!DeinitContainers()) {
        qDebug() << "CDataContainer::Destructor / DeinitContainers failed";
    }
}

bool CDataContainer::InitContainers()
{
    if (!CDataContainerCollectionBase::InitContainers()) {
        return false;
    }

    ProgramList = new CDataProgramList();
    if (!ResetDCProgramList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCProgramList failed.";
        return false;
    }

    ReagentList = new CDataReagentList();
    if (!ResetDCReagentList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCReagentList failed.";
        return false;
    }

    ReagentGroupList = new CDataReagentGroupList();
    if (!ResetDCReagentGroupList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCReagentGroupList failed.";
        return false;
    }

    DashboardStationList = new CDashboardDataStationList;
    if (!ResetDCDashboardStationList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCDashboardStationList failed.";
        return false;
    }

    m_IsInitialized = true;
    return true;
}

bool CDataContainer::DeinitContainers()
{
    if (!CDataContainerCollectionBase::DeinitContainers()) {
        return false;
    }

    delete ReagentList;
    delete ReagentGroupList;
    delete ProgramList;
    delete DashboardStationList;
    return true;
}



bool CDataContainer::ResetDCProgramList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCProgramList was already called";
        return false;
    }

    // init program list
    ProgramList->Init();


    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataProgramListVerifier = NULL;
    if (p_DataProgramListVerifier == NULL) {
        p_DataProgramListVerifier = new CDataProgramListVerifier(this);
    }
    // register this verifier object in the data container (=> dependency injection)
    ProgramList->AddVerifier(p_DataProgramListVerifier);

    return true;
}


bool CDataContainer::ResetDCReagentList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCReagentList was already called";
        return false;
    }

    // init reagent list
    ReagentList->Init();

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataReagentListVerifier = NULL;
    if (p_DataReagentListVerifier == NULL) {
        p_DataReagentListVerifier = new CDataReagentListVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    ReagentList->AddVerifier(p_DataReagentListVerifier);

    return true;
}

bool CDataContainer::ResetDCReagentGroupList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCReagentGroupList was already called";
        return false;
    }

    // init reagent list
    ReagentGroupList->Init();
    /// todo need to add verifier
//    // create a verifier object for this data container, if not done before
//    static IVerifierInterface *p_DataReagentListVerifier = NULL;
//    if (p_DataReagentListVerifier == NULL) {
//        p_DataReagentListVerifier = new CDataReagentListVerifier();
//    }
//    // register this verifier object in the data container (=> dependency injection)
//    ReagentList->AddVerifier(p_DataReagentListVerifier);

    return true;
}

bool CDataContainer::ResetDCDashboardStationList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCDashboardStationList was already called";
        return false;
    }

    // init reagent list
    DashboardStationList->Init();
    /// todo need to add verifier

//    // create a verifier object for this data container, if not done before
//    static IVerifierInterface *p_DataReagentListVerifier = NULL;
//    if (p_DataReagentListVerifier == NULL) {
//        p_DataReagentListVerifier = new CDataReagentListVerifier();
//    }
//    // register this verifier object in the data container (=> dependency injection)
//    ReagentList->AddVerifier(p_DataReagentListVerifier);

      return true;
}


bool CDataContainer::ResetDCUserSettings()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCUserSettings was already called";
        return false;
    }

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataUserSettingVerifier = NULL;
    if (p_DataUserSettingVerifier == NULL) {
        //p_DataUserSettingVerifier = new CUserSettingsVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    SettingsInterface->AddVerifier(p_DataUserSettingVerifier);

    return true;
}



bool CDataContainer::ResetDCDeviceConfiguration()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCDeviceConfiguration was already called";
        return false;
    }

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataDeviceConfigurationVerifier = NULL;
    if (p_DataDeviceConfigurationVerifier == NULL) {
        p_DataDeviceConfigurationVerifier = new CDeviceConfigurationVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    DeviceConfigurationInterface->AddVerifier(p_DataDeviceConfigurationVerifier);

    return true;
}

bool CDataContainer::ResetDCAdjustment()
{
//    if (m_IsInitialized == true) {
//        qDebug() << "CDataContainer::ResetDCAdjustment was already called";
//        return false;
//    }

//    // create a verifier object for this data container, if not done before
//    static IVerifierInterface *p_AdjustmentVerifier = NULL;
//    if (p_AdjustmentVerifier == NULL) {
//        p_AdjustmentVerifier = new CAdjustmentVerifier();
//    }
//    // register this verifier object in the data container (=> dependency injection)
//    AdjustmentList->AddVerifier(p_AdjustmentVerifier);

    return true;
}

QString CDataContainer::GetReagentName(QString ReagentID)
{
    if (ReagentList) {
        const CReagent *p_Reagent = ReagentList->GetReagent(ReagentID);
        if (p_Reagent) {
            return p_Reagent->GetReagentName();
        }
    }
}

}// namespace DataManager
