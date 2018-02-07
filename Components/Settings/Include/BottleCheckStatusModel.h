/****************************************************************************/
/*! \file BottleCheckStatusModel.h
 *
 *  \brief Definition file for class CBottleCheckStatusModel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2015-04-30
 *   $Author:  $ Abe Yang
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

#ifndef BOTTLECHECKSTATUSMODEL_H
#define BOTTLECHECKSTATUSMODEL_H

#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "Global/Include/GlobalDefines.h"
#include <QAbstractTableModel>

namespace Settings {


/****************************************************************************/
/**
 * \brief This is the data model for the reagent tables
 */
/****************************************************************************/
class CBottleCheckStatusModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CBottleCheckStatusModel(QObject *p_Parent = NULL);
    void SetRequiredContainers(DataManager::CDataReagentList *p_ReagentList,
                               DataManager::CDashboardDataStationList *p_StationList);
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;  //data Reagent SubMenu
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;

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

    void SetVisibleRowCount(int RowCount);
    QString GetReagentID(const QString ReagentName);
    void SetBottleCheckStatusMap(QMap<QString, QString> & statusMap);
public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of LoadStationData
     */
    /****************************************************************************/
    void LoadStationData();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateStatusData
     *  \iparam stationID = station ID
     *  \iparam status = bottle check status
     *  \iparam currentRow = the current processed row
     */
    /****************************************************************************/
    void UpdateStatusData(const QString& stationID, const QString& status, int currentRow);

private:

    DataManager::CDataReagentList *mp_ReagentList;      //!< Reagent list
    DataManager::CDashboardDataStationList *mp_StationList; //!< Station list
    QMap<QString, QString> m_Identifiers;               //!< Identifiers of reagents currently displayed
    QMap<QString, QString> m_StationIdentifiers;               //!< Identifiers of stations currently displayed
    QMap<QString, QString> m_StatusIdentifiers;               //!< Identifiers of stations status displayed
    QStringList m_ReagentNames;                         //!< Long names of reagents currently displayed
    QStringList m_StationNames;                         //!< sttion names
    qint32 m_Columns;                                   //!< Number of table columns
    QString m_CurrentReagentName;                       //!< Current Reagent Name
    qint32 m_VisibleRowCount;                           //!< Number of rows visible in the table
    QStringList m_VisibleReagentIds;                    //!< Reagent Ids list
    int m_CurrentRow;
};

} // end namespace Settings

#endif // BOTTLECHECKSTATUSMODEL_H

