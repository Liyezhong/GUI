/****************************************************************************/
/*! \file DataManager.h
 *
 *  \brief Definition file for class CDataManager.
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

#ifndef DATAMANAGER_DATAMANAGER_H
#define DATAMANAGER_DATAMANAGER_H

#include <QObject>
#include <QString>
#include <QIODevice>
#include "../Include/DataContainer.h"
#include "TestStubDataManager/Include/DataManagerBase.h"

namespace DataManager {
class CDashboardStationCommandInterface;
class CReagentCommandInterface;
class CReagentGroupCommandInterface;
class CProgramCommandInterface;
class CUserSettingsCommandInterface;
class CDeviceConfigurationInterface;


class CDataManager : public CDataManagerBase
{
    Q_OBJECT
private:
    CDataContainer* mp_DataContainer;
    bool m_UpdateList;
    CDashboardStationCommandInterface *mp_DashboardStationCommandInterface; //!< handles commands related to Dashboard station container
    CReagentCommandInterface *mp_ReagentCommandInterface; //!< handles commands related to reagent container
    CReagentGroupCommandInterface *mp_ReagentGroupCommandInterface; //!< handles commands related to reagent container

protected:
    virtual bool DeinitDataContainer();

public:
    CDataManager(Threads::MasterThreadController *p_HimalayaMasterThreadController);
    CDataManager(Threads::MasterThreadController *p_HimalayaMasterThreadController, QString Path);
    virtual ~CDataManager();

    CProgramCommandInterface *mp_ProgramCommandInterface; //!< Handles commands related to program container
    quint32 InitDataContainer();

    const CDataContainer* GetDataContainer() { return mp_DataContainer; }
    CDataProgramList* GetProgramList();
    CDataReagentList* GetReagentList();
    CDataReagentGroupList* GetReagentGroupList();
    CDashboardDataStationList* GetDashboardStationList();
    bool GetProgramList(CDataProgramList *p_ProgramList);
    bool GetReagentList(CDataReagentList *p_ReagentList);
    bool GetReagentGroupList(CDataReagentGroupList *p_ReagentGroupList);

    CUserSettingsInterface* GetUserSettingsInterface();
    CDeviceConfigurationInterface* GetDeviceConfigurationInterface();

signals:
    void ReagentUpdate(CReagent &Reargent);
    void ReagentContainerUpdate(CDataReagentList &);

};
}// namespace DataManager

#endif // DATAMANAGER_DATAMANAGER_H


