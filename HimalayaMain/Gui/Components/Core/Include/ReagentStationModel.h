/****************************************************************************/
/*! \file ReagentStationModel.h
 *
 *  \brief Definition file for class CReagentStatusModel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-02-11 , 2013-02-24
 *   $Author:  $ Swati Tiwari
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

#ifndef CORE_REAGENTSTATIONMODEL_H
#define CORE_REAGENTSTATIONMODEL_H

#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "Global/Include/GlobalDefines.h"
#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QPixmap>
namespace Programs {
    class CModifyProgramStepDlg;
}
namespace Core {


/****************************************************************************/
/**
 * \brief This is the data model for the reagent tables
 */
/****************************************************************************/
class CReagentStationModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CReagentStationModel(QObject *p_Parent = NULL);
    void SetRequiredContainers(DataManager::CDataReagentList *p_ReagentList,
                               DataManager::CDataReagentGroupList *p_ReagentGroupList,
                               DataManager::CDashboardDataStationList *p_StationList, qint32 Columns);
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;  //data Reagent SubMenu
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;
    void ResetAndUpdateModel();

     Qt::ItemFlags flags(const QModelIndex &Index) const;
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
     * \brief Sets the currently selected reagent in the table
     * \iparam ReagentName
     */
    /****************************************************************************/
    void SetCurrentReagent(QString ReagentName){
        m_CurrentReagentName = ReagentName;
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
    QString GetReagentID(const QString ReagentName);
    QString GetReagentLongName(int Row);
    bool ContainsReagent(QString ReagentID);
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);

public slots:
    void UpdateReagentList();
private:

    DataManager::CDataReagentList *mp_ReagentList;      //!< Reagent list
    DataManager::CDataReagentGroupList *mp_ReagentGroupList; //!< Reagent group list
    DataManager::CDashboardDataStationList *mp_StationList; //!< Station list
    QMap<QString, QString> m_Identifiers;               //!< Identifiers of reagents currently displayed
    QMap<QString, QString> m_StationIdentifiers;               //!< Identifiers of stations currently displayed
    QStringList m_ReagentNames;                         //!< Long names of reagents currently displayed
    QStringList m_ReagentID;                            //!< List of reagent ID
    QStringList m_StationNames;                         //!< station names
    QString m_CurrentReagentName;
    qint32 m_Columns;                                   //!< Number of table columns
    Programs::CModifyProgramStepDlg *mp_Parent;         //!< Reference to ModifyProgramStep Dialog
    bool m_FilterLeicaReagent;                          //!< Filter Leica Reagent if set to true
    qint32 m_VisibleRowCount;                           //!< Number of rows visible in the table
    QMap<QString, QString> m_ReagentNameMap;            //!< Sorts Reagent Longnames
    QMap<QString, QString> m_StationNameMap;            //!< Sorts Station names
    bool m_BLFlag;                                      //!< Flag for reagents in BathLayout
    QStringList m_VisibleReagentIds;                    //!< Reagent Ids list
    QString m_DeviceMode;                               //!< Device mode
    DataManager::CUserSettings m_UserSettings;          //!< UserSettings object
    Global::RMSOptions_t m_RMSOptions;
    Global::DateFormat  m_DateFormat;
};

} // end namespace Core

#endif // CORE_REAGENTSTATUSMODEL_H
