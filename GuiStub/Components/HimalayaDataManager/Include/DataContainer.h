/****************************************************************************/
/*! \file DataContainer.h
 *
 *  \brief Definition file for class CDataContainer.
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

#ifndef DATAMANAGER_DATACONTAINER_H
#define DATAMANAGER_DATACONTAINER_H

#include <QString>
#include <QIODevice>

#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"
#include "HimalayaDataContainer/Containers/StainingPrograms/Include/DataProgramList.h"
#include "HimalayaDataContainer/Containers/StainingReagents/Include/DataReagentList.h"
#include "TestStubDataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "TestStubDataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h"
#include "TestStubDataManager/Include/DataContainerCollectionBase.h"
#include "HimalayaDataContainer/Containers/StainingReagentGroups/Include/DataReagentGroupList.h"

namespace Himalaya {
    class HimalayaMasterThreadController;
}
namespace DataManager {
//!<  List containing macro steps and normal program steps
typedef QList<CProgramStep*> ListOfExpandedSteps_t;

class CDataContainer: public DataManager::CDataContainerCollectionBase
{
    Q_OBJECT
private:
    bool DeinitContainers();

    bool ResetDCProgramList();
    bool ResetDCReagentList();
    bool ResetDCReagentGroupList();
    bool ResetDCDashboardStationList();
    bool ResetDCUserSettings();
    bool ResetDCBLGConfiguration();
    bool ResetDCAdjustment();
    bool ResetDCDeviceConfiguration();
    bool AddStepsToExpandedStepList(CProgram *p_Program);
    bool CompareSteps(CProgramStep& CurrentProgramStep, CProgramStep& NextProgramStep, bool& OK);
public:
    CDataContainer(Threads::MasterThreadController *p_HimalayaMasterThreadController);
    ~CDataContainer();
    bool InitContainers();
    bool RefreshProgramStepStationlist();
    QString GetReagentName(QString ReagentID);

    // prefix mp_ left because members are intentially accessible from outside (public)
    CDataProgramList* ProgramList;  //!< Container for programs
    CDataReagentList* ReagentList;  //!< Container for reagents
    CDataReagentGroupList* ReagentGroupList; //!< Container for reagent groups
    CDashboardDataStationList* DashboardStationList; //!< Container for dash board stations

};

} // namespace DataManager

#endif // DATAMANAGER_DATACONTAINER_H

