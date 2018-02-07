/****************************************************************************/
/*! \file Himalaya/HimalayaMain/Gui/Components/Core/Include/DataContainer.h
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

#include <QIODevice>

#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h"
#include "DataManager/Containers/SWVersions/Include/SWVersionList.h"
#include "HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColorList.h"
namespace DataManager {
//!<  List containing macro steps and normal program steps
typedef QList<CProgramStep*> ListOfExpandedSteps_t;

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class DataManager::CDataContainer
 */
/****************************************************************************/
class CDataContainer: public QObject
{
    Q_OBJECT
private:
    ListOfExpandedSteps_t m_ExpandedStepList;   //!< Normal steps + Macro steps
    bool m_IsInitialized;       ///<  Definition/Declaration of variable m_IsInitialized

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function InitContainers
     *
     *  \return from InitContainers
     */
    /****************************************************************************/
    bool InitContainers();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function DeinitContainers
     *
     *  \return from DeinitContainers
     */
    /****************************************************************************/
    bool DeinitContainers();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetDCProgramList
     *
     *  \return from ResetDCProgramList
     */
    /****************************************************************************/
    bool ResetDCProgramList();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetDCReagentList
     *
     *  \return from ResetDCReagentList
     */
    /****************************************************************************/
    bool ResetDCReagentList();
    bool ResetDCReagentGroupList();
    bool ResetDCDashboardStationList();
    bool ResetDCReagentGroupColorList();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetDCUserSettings
     *
     *  \return from ResetDCUserSettings
     */
    /****************************************************************************/
    bool ResetDCUserSettings();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function AddStepsToExpandedStepList
     *
     *  \param Program = CProgram type parameter
     *
     *  \return from AddStepsToExpandedStepList
     */
    /****************************************************************************/
    bool AddStepsToExpandedStepList(CProgram &Program);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetDCDeviceConfiguration
     *
     *  \return from ResetDCDeviceConfiguration
     */
    /****************************************************************************/
    bool ResetDCDeviceConfiguration();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetSWVersionList
     *
     *  \return from ResetSWVersionList
     */
    /****************************************************************************/
    bool ResetSWVersionList();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CompareSteps
     *
     *  \param CurrentProgramStep = CProgramStep type parameter
     *  \param NextProgramStep =  CProgramStep type parameter
     *  \param OK =  bool type parameter
     *
     *  \return from CompareSteps
     */
    /****************************************************************************/
    bool CompareSteps(CProgramStep& CurrentProgramStep, CProgramStep& NextProgramStep, bool& OK);
public:
    CDataContainer();
    ~CDataContainer();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RefreshProgramStepStationlist
     *
     *  \return from RefreshProgramStepStationlist
     */
    /****************************************************************************/
    bool RefreshProgramStepStationlist();

    /* Methods to handle expanded step list */
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RefreshExpandedStepList
     *
     *  \param ProgramID = QString type parameter
     *
     *  \return from RefreshExpandedStepList
     */
    /****************************************************************************/
    bool RefreshExpandedStepList(QString ProgramID);

    /****************************************************************************/
    /*!
     *  \brief Retrieves the expanded steplist
     *
     *  \return Expanded steplist
     *  \todo Currently returning step list count. change to expanded list count
     *        when macro's are added.
     */
    /****************************************************************************/
    int GetNumberOfStepsInExpandedList() {return m_ExpandedStepList.count();}

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetProgramStepExpanded
     *
     *  \param Index =  unsigned int type parameter
     *  \param p_Program =  CProgram type parameter
     *  \param p_ProgStep =  CProgramStep type parameter
     *
     *  \return from GetProgramStepExpanded
     */
    /****************************************************************************/
    bool GetProgramStepExpanded(const unsigned int Index, const CProgram* p_Program, CProgramStep* p_ProgStep);

    // praefix mp_ left because members are intentially accessible from outside (public)
    CDataProgramList* ProgramList;  //!< Container for programs
    CDataReagentList* ReagentList;  //!< Container for reagents
    CDataReagentGroupList* ReagentGroupList;  //!< Container for reagent groups
    CDashboardDataStationList* DashboardStationList;  //!< Container for Dashboard stations
    CUserSettingsInterface *SettingsInterface; //!< Provides interface to read the Settings info from xml
    CDeviceConfigurationInterface *DeviceConfigurationInterface;  //!< Provides interface to read the Device configuration from xml
    CSWVersionList *SWVersionList;  //!< Container for software version
    CReagentGroupColorList *ReagentGroupColorList;       ///<  Definition/Declaration of variable ReagentGroupColorList
};

} // namespace DataManager

#endif // DATAMANAGER_DATACONTAINER_H


