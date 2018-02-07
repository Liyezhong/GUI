/****************************************************************************/
/*! \file ReagentStationEditModel.h
 *
 *  \brief Definition file for class CReagentStationEditModel.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2011-08-23, 2013-01-17
 *   $Author:  $ M.Scherer, N.Kamath, Swati Tiwari
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef CORE_REAGENTSTATIONEDITMODEL_H
#define CORE_REAGENTSTATIONEDITMODEL_H

#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include <QAbstractTableModel>

namespace Programs {
    class CModifyProgramStepDlg;
}

/****************************************************************************/
/*!
 *  \brief  Definition of enum StationGroupType_t
 */
/****************************************************************************/
typedef enum {
    ONLY_PARAFFIN_REAGENT,
    NON_PARAFFINBATH_REAGENT,
    NON_CLEANING_REAGENT
}SpecialReagentType_t;

namespace Core {


/****************************************************************************/
/**
 * \brief This is the data model for the reagent tables
 */
/****************************************************************************/
class CReagentStationEditModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CReagentStationEditModel(QObject *p_Parent = NULL);
    void SetRequiredContainers(DataManager::CDataReagentList *p_ReagentList,
                               DataManager::CDataReagentGroupList *p_ReagentGroupList,
                               DataManager::CDashboardDataStationList *p_DashboardDataStationList, qint32 Columns);

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;  //data Reagent SubMenu

    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;
    Qt::ItemFlags flags(const QModelIndex &Index) const;
    QModelIndex CreateIndex(int Row, int Column);
    /****************************************************************************/
    /**
     * \brief Returns the position of Reagent to be selected
     * \iparam ReagentName
     * \return m_ReagentNames = Position of Reagent in the table
     */
    /****************************************************************************/
    int GetReagentPosition(QString ReagentName){
        return m_ReagentNames.indexOf(ReagentName, 0);
    }

    /****************************************************************************/
    /**
     * \brief Returns the position of Reagent to be selected
     * \iparam ReagentID
     * \return m_ReagentNames = Position of Reagent in the table
     */
    /****************************************************************************/
    int GetReagentPositionOfReagent(QString ReagentID){
        return m_ReagentID.indexOf(ReagentID, 0);
    }

    /****************************************************************************/
    /**
     * \brief Sets the ModifiedProgramStepDlg flag
     * \param reagentType = reagent type
     */
    /****************************************************************************/
    void SetReagentType(SpecialReagentType_t reagentType)
    {
        m_ReagentType = reagentType;
    }

    /****************************************************************************/
    /**
     * \brief Sets the parent pointer
     * \iparam p_Parent = Reference to parent widget
     */
    /****************************************************************************/
    void SetParentPtr(Programs::CModifyProgramStepDlg *p_Parent){ mp_Parent = p_Parent; }
    //void OnCheckBoxChanged(int State);
    /****************************************************************************/
    /**
     * \brief Function to hide the Leica reagents in the table.
     *        Note - Call this function before setting Reagent list.
     * \iparam Filter = if true filter Leica reagents else show.
     */
    /****************************************************************************/
    void FilterLeicaReagents(bool Filter) { m_FilterLeicaReagent = Filter; }

    void SetVisibleRowCount(int RowCount);
    QString GetReagentID(int Row);
	/****************************************************************************/
	/*!
	 *  \brief  Definition/Declaration of function GetReagentLongName
	 *
	 *  \param ReagentId = QString type parameter
	 *
	 *  \return from GetReagentLongName
	 */
	/****************************************************************************/
	QString GetReagentLongName(QString ReagentId);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetUserSettings
     *
     *  \param p_UserSettings = DataManager::CUserSettings type parameter
     *
     *  \return from SetUserSettings
     */
    /****************************************************************************/
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);
    void UpdateReagentList();

private:

    DataManager::CDataReagentList *mp_ReagentList;      //!< Reagent list
    DataManager::CDataReagentGroupList *mp_ReagentGroupList; //!< Reagent group list
    DataManager::CDashboardDataStationList *mp_DashboardDataStationList; //!< DashboardDataStationList
    QMap<QString, QString> m_Identifiers;               //!< Identifiers of reagents currently displayed
    QStringList m_ReagentNames;                         //!< Long names of reagents currently displayed
    QStringList m_ReagentID;       ///<  Definition/Declaration of variable m_ReagentID
    qint32 m_Columns;                                   //!< Number of table columns
    Programs::CModifyProgramStepDlg *mp_Parent;         //!< Reference to ModifyProgramStep Dialog
    bool m_FilterLeicaReagent;                          //!< Filter Leica Reagent if set to true
    qint32 m_VisibleRowCount;                           //!< Number of rows visible in the table
    QStringList m_VisibleReagentIds;                    //!< Reagent Ids list
    DataManager::CUserSettings m_UserSettings;          //!< UserSettings object    
    SpecialReagentType_t m_ReagentType;   ///<  Definition/Declaration of variable m_ReagentType
};

} // end namespace Core

#endif // CORE_REAGENTSTATIONEDITMODEL_H

