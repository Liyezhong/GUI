/****************************************************************************/
/*! \file Himalaya/HimalayaMain/Gui/Components/Core/Source/DataContainer.cpp
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
#include "Core/Include/DataContainer.h"

#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramListVerifier.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentListVerifier.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationVerifier.h"

//lint -sem(DataManager::CDataContainer::InitContainers, initializer)
//lint -sem(DataManager::CDataContainer::DeinitContainers,cleanup)

//lint -e1566
//lint -e1579

namespace DataManager {
CDataContainer::CDataContainer() : m_IsInitialized(false), ProgramList(NULL),
                                    ReagentList(NULL), ReagentGroupList(NULL), DashboardStationList(NULL), SettingsInterface(NULL),
                                    ReagentGroupColorList(NULL)

{
    if (!InitContainers()) {
        qDebug() << "CDataContainer::Constructor / InitContainers failed";
        Q_ASSERT(false);
    }

}

CDataContainer::~CDataContainer()
{
    try
    {
        if (!DeinitContainers()) {
            qDebug() << "CDataContainer::Destructor / DeinitContainers failed";
            Q_ASSERT(false);
        }
    }
    catch(...)
    {}
}

bool CDataContainer::InitContainers()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::InitContainers was already called";
        //Q_ASSERT(false);
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

    DashboardStationList = new CDashboardDataStationList();
    if(!ResetDCDashboardStationList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCDashboardStationList failed.";
        return false;
    }
    ReagentGroupColorList = new CReagentGroupColorList();
    if(!ResetDCReagentGroupColorList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCDashboardStationList failed.";
        return false;
    }

    SettingsInterface = new CUserSettingsInterface();
    if (!ResetDCUserSettings()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCSettings failed!";
        return false;
    }

    DeviceConfigurationInterface = new CDeviceConfigurationInterface();
    if (!ResetDCDeviceConfiguration()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCDeviceConfiguration failed.";
        return false;
    }

    SWVersionList = new CSWVersionList();
    if (!ResetSWVersionList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetSWVersionList failed.";
        return false;
    }

    ProgramList->SetDataVerificationMode(false);
    ReagentList->SetDataVerificationMode(false);
    SettingsInterface->SetDataVerificationMode(false);
    SWVersionList->SetDataVerificationMode(false);
    DeviceConfigurationInterface->SetDataVerificationMode(false);
    m_IsInitialized = true;
    return true;
}

bool CDataContainer::DeinitContainers()
{
    if (m_IsInitialized != true) {
        // nothing to do
        return true;
    }
    try {
        delete ProgramList;
        delete ReagentGroupColorList;
        delete ReagentGroupList;
        delete ReagentList;
        delete DashboardStationList;
        delete SettingsInterface;
        delete DeviceConfigurationInterface;
        delete SWVersionList;
        return true;
    } catch (...) {
        return false;
    }

}



bool CDataContainer::ResetDCProgramList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCProgramList was already called";
        return false;
    }

    // init program list
    ProgramList->Init();
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
    return true;
}

bool CDataContainer::ResetDCDashboardStationList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCDashboardStationList was already called";
        return false;
    }

    // init dashboard station list
    DashboardStationList->Init();
    return true;
}

bool CDataContainer:: ResetDCReagentGroupColorList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCReagentGroupColorList was already called";
        return false;
    }

    ReagentGroupColorList->Init();
    /// todo need to add verifier
    return true;

}


bool CDataContainer::ResetDCUserSettings()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCUserSettings was already called";
        return false;
    }
    return true;
}

bool CDataContainer::ResetDCDeviceConfiguration()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCDeviceConfiguration was already called";
        return false;
    }
    return true;
}

bool CDataContainer::ResetSWVersionList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetSWVersionList was already called";
        return false;
    }
    return true;
}

}// namespace DataManager
