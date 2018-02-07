/****************************************************************************/
/*! \file BottleCheckStatusModel.cpp
 *
 *  \brief CBottleCheckStatusModel Implementation.
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Settings/Include/BottleCheckStatusModel.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/MainWindow.h"
#include "HimalayaDataContainer/Helper/Include/Global.h"

//lint -e613
//lint -e525

namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CBottleCheckStatusModel::CBottleCheckStatusModel(QObject *p_Parent) :
    QAbstractTableModel(p_Parent), m_CurrentRow(0)
{
    mp_ReagentList = NULL;
    mp_StationList = NULL;
    m_Columns = 3;
    m_VisibleRowCount = 8;

}

/****************************************************************************/
/*!
 *  \brief Initializes the reagent data
 *
 *  \iparam p_ReagentList = Reagent data
 *  \iparam p_StationList = Station data
 *
 */
 /****************************************************************************/
void CBottleCheckStatusModel::SetRequiredContainers(DataManager::CDataReagentList *p_ReagentList,
                                                DataManager::CDashboardDataStationList *p_StationList)
{
    mp_ReagentList = p_ReagentList;
    mp_StationList = p_StationList;
    LoadStationData();
}


/****************************************************************************/
/*!
 *  \brief Loads the station data
 *
 *  This slot needs to be called whenever the content of the status list is
 *  changed.
 */
/****************************************************************************/
void CBottleCheckStatusModel::LoadStationData()
{
    beginResetModel();
    m_Identifiers.clear();
    m_ReagentNames.clear();
    m_StationNames.clear();

    if (mp_StationList) {
        for (qint32 StationCounter = 0; StationCounter < mp_StationList->GetNumberOfDashboardStations(); StationCounter++) {
            DataManager::CDashboardStation* p_Station = const_cast<DataManager::CDashboardStation*>(mp_StationList->GetDashboardStation(StationCounter));
            if (p_Station) {
                QString ReagentName;
                if (mp_ReagentList) {
                    DataManager::CReagent *p_Reagent = NULL;
                    p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(p_Station->GetDashboardReagentID()));
                    if (p_Reagent) {
                        ReagentName =  p_Reagent->GetReagentName();
                    }
                }
                m_ReagentNames << ReagentName;
                // store the station id for the reagent name
                m_StationIdentifiers[p_Station->GetDashboardStationName()] = p_Station->GetDashboardStationID();
                m_StationNames << p_Station->GetDashboardStationName();
                m_Identifiers[p_Station->GetDashboardStationName()] = p_Station->GetDashboardReagentID();
            }
        }
    }

    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief Updates the status data
 *
 *  This slot needs to be called whenever the content of the status list is
 *  changed.
 */
/****************************************************************************/
void CBottleCheckStatusModel::UpdateStatusData(const QString& stationID, const QString& status, int currentRow)
{
    beginResetModel();
    m_StatusIdentifiers[stationID] = status;
    m_CurrentRow = currentRow;
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount
 */
/****************************************************************************/
void CBottleCheckStatusModel::SetVisibleRowCount(int RowCount)
{
    beginResetModel();
    m_VisibleRowCount = RowCount;
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief Returns the number of rows in the table
 *
 *  \return Row count
 */
/****************************************************************************/
int CBottleCheckStatusModel::rowCount(const QModelIndex &) const
{
    return ((m_ReagentNames.count() < m_VisibleRowCount) ? m_VisibleRowCount : m_ReagentNames.count());
}

/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *
 *  \return Columns count
 */
/****************************************************************************/
int CBottleCheckStatusModel::columnCount(const QModelIndex &) const
{
    return m_Columns;
}

/****************************************************************************/
/*!
 *  \brief Returns the data items displayed in the table
 *
 *  \iparam Index = Index of a table cell
 *  \iparam Role = Display role to be returned
 *
 *  \return Data item
 */
/****************************************************************************/
QVariant CBottleCheckStatusModel::data(const QModelIndex &Index, int Role) const
{
    DataManager::CReagent *p_Reagent = NULL;
    DataManager::CDashboardStation *p_Station = NULL;
    QString strStatus;
    if (mp_ReagentList == NULL || mp_StationList == NULL) {
        return QVariant();
    }

    if (Index.row() < m_ReagentNames.count()) {
        p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(m_Identifiers[m_StationNames[Index.row()]]));
        p_Station = const_cast<DataManager::CDashboardStation*>(mp_StationList->GetDashboardStation(m_StationIdentifiers[m_StationNames[Index.row()]]));
        strStatus = m_StatusIdentifiers[p_Station->GetDashboardStationID()];
    }

    if (!p_Reagent && p_Station) {
        if ((Role == (int)Qt::DisplayRole) && (0 == Index.column()))
            return p_Station->GetDashboardStationName();
        else
            return QVariant();
    }

    if (Index.row() < m_ReagentNames.count())
    {
        if (Role == (int)Qt::DisplayRole) {
            switch (Index.column()) {
            case 0:
                return p_Station->GetDashboardStationName();

            case 1:
                if (p_Reagent) {
                    return p_Reagent->GetReagentName();
                }
                else {
                    return QString("None");
                }
            case 2:
                   return strStatus;

            }
        }
        else if (Role == (int)Qt::BackgroundRole) {
            if (m_CurrentRow == Index.row())
            {
                QColor Color;
                Color.setNamedColor("#DA201D");
                QPalette Palette(Color);
                return QVariant(Palette.color(QPalette::Window));
            }
        }

    }
    return QVariant();
}

/****************************************************************************/
/*!
 *  \brief Returns the caption of the column headers
 *
 *  \iparam Section = Row or column number
 *  \iparam Orientation = Horizontal or vertical header
 *  \iparam Role = Display role to be returned
 *
 *  \return Header data
 */
/****************************************************************************/
QVariant CBottleCheckStatusModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {
        switch (Section) {//column head
        case 0:
            return tr("Station");
        case 1:
            return tr("Reagent");
        case 2:
            return tr("Status");
        }
    }
    return QVariant();
}

/****************************************************************************/
/*!
 *  \brief This Function Gets Reagent ID of given Reagent Long name.
 *
 *  \iparam ReagentName
 *
 *  \return return from GetReagentID
 */
/****************************************************************************/
QString CBottleCheckStatusModel::GetReagentID(const QString ReagentName)
{
    return m_Identifiers.value(ReagentName);

}

/****************************************************************************/
/*!
 *  \brief Returns item model flags of a cell
 *
 *  \iparam Index = Index of a table cell
 *
 *  \return Flags of the cell
 */
/****************************************************************************/
Qt::ItemFlags CBottleCheckStatusModel::flags(const QModelIndex &Index) const
{
    Q_UNUSED(Index);
    return Qt::ItemIsEnabled;
}

/****************************************************************************/
/*!
 *  \brief Set BottleCheck Status Map
 *
 *  \iparam statusMap = BottleCheck status map
 *
 *  \return SetBottleCheckStatusMap
 */
/****************************************************************************/
void CBottleCheckStatusModel::SetBottleCheckStatusMap(QMap<QString, QString>& statusMap)
{
    m_StatusIdentifiers.clear();
    m_StatusIdentifiers = statusMap;
}

} // end namespace Settings
