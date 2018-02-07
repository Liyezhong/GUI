/****************************************************************************/
/*! \file ReagentStatusModel.cpp
 *
 *  \brief ReagentStatusModel Implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-02-11 , 2013-02-23
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Core/Include/ReagentStatusModel.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MessageDlg.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/MainWindow.h"
#include "Programs/Include/ProgramWidget.h"
#include "Application/Include/LeicaStyle.h"
#include "HimalayaDataContainer/Helper/Include/Global.h"

//lint -e613
//lint -e525

namespace Core {

Global::RMSOptions_t CReagentStatusModel:: RMSPROCESSINGOPTION = Global::RMS_UNDEFINED;
Global::RMSOptions_t CReagentStatusModel:: RMSCLEANINGOPTIONS = Global::RMS_UNDEFINED;

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CReagentStatusModel::CReagentStatusModel(QObject *p_Parent) : QAbstractTableModel(p_Parent)
{
    mp_ReagentList = NULL;
    mp_ReagentGroupList = NULL;
    mp_Parent = NULL;
    mp_StationList = NULL;
    m_FilterLeicaReagent = false;
    mp_UserSettings = NULL;
    m_Columns = 0;
    m_VisibleRowCount = 7;
}

/****************************************************************************/
/*!
 *  \brief Initializes the reagent data
 *
 *  \iparam p_ReagentList = Reagent data
 *  \iparam p_ReagentGroupList = ReagentGroup data
 *  \iparam p_StationList = Station data
 *  \iparam Columns = Table columns
 */
 /****************************************************************************/
void CReagentStatusModel::SetRequiredContainers(DataManager::CDataReagentList *p_ReagentList,
                                                DataManager::CDataReagentGroupList *p_ReagentGroupList,
                                                DataManager::CDashboardDataStationList *p_StationList, qint32 Columns)
{
    mp_ReagentList = p_ReagentList;
    mp_ReagentGroupList = p_ReagentGroupList;
    mp_StationList = p_StationList;
    m_Columns = Columns;
    UpdateReagentList();
}


/****************************************************************************/
/*!
 *  \brief Updates the reagent data
 *
 *  This slot needs to be called whenever the content of the reagent list is
 *  changed.
 */
/****************************************************************************/
void CReagentStatusModel::UpdateReagentList()
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
                        if(p_Reagent->GetVisibleState()== true){
                            m_VisibleReagentIds << p_Reagent->GetReagentName();
                        }
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
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount
 */
/****************************************************************************/
void CReagentStatusModel::SetVisibleRowCount(int RowCount)
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
int CReagentStatusModel::rowCount(const QModelIndex &) const
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
int CReagentStatusModel::columnCount(const QModelIndex &) const
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
QVariant CReagentStatusModel::data(const QModelIndex &Index, int Role) const
{
    DataManager::CReagent *p_Reagent = NULL;
    DataManager::CDashboardStation *p_Station = NULL;
    bool Expired;
    if (mp_ReagentList == NULL || mp_StationList == NULL) {
        return QVariant();
    }

    if (Index.row() < m_ReagentNames.count()) {
        p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(m_Identifiers[m_StationNames[Index.row()]])); 
        p_Station = const_cast<DataManager::CDashboardStation*>(mp_StationList->GetDashboardStation(m_StationIdentifiers[m_StationNames[Index.row()]]));
    }

    if (!p_Reagent && p_Station) {
        if ((Role == (int)Qt::DisplayRole) && (0 == Index.column()))
            return p_Station->GetDashboardStationName();
        else
            return QVariant();
    }

    if (Index.row() < m_ReagentNames.count())
    {
            DataManager::ReagentStatusType_t ReagentStatus = p_Station->GetReagentStatus(*p_Reagent, RMSPROCESSINGOPTION);
            if ( ReagentStatus == DataManager::REAGENT_STATUS_EXPIRED )
                Expired = true;
            else
                Expired = false;

            if( true == Expired && Role == (int) Qt::TextColorRole)
            {
                switch (Index.column()) {
                case 2:
                    QVariant v = QColor(Qt::red);
                    return v;//if the reagent expired, text is red
                }
            }

            ReagentStatus = p_Station->GetReagentStatus(*p_Reagent, RMSCLEANINGOPTIONS);
            if ( ReagentStatus == DataManager::REAGENT_STATUS_EXPIRED )
                Expired = true;
            else
                Expired = false;

            if( true == Expired && Role == (int) Qt::TextColorRole)
            {
                switch (Index.column()) {
                case 3:
                    return QColor(Qt::red);//if the reagent expired, text is red
                }
            }

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
            case 2://column 3
                if (mp_ReagentGroupList) {

                    bool isCleaningReagentGroup = mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID())->IsCleaningReagentGroup();
                    if (((Global::RMS_OFF == RMSCLEANINGOPTIONS) || (RMSPROCESSINGOPTION != RMSCLEANINGOPTIONS))
                            && isCleaningReagentGroup)
                    {
                        return "";
                    }

                    switch (RMSPROCESSINGOPTION) {
                        default:
                            return QString("");
                        case Global::RMS_CASSETTES:
                            return p_Station->GetDashboardReagentActualCassettes();
                        case Global::RMS_CYCLES:
                            return p_Station->GetDashboardReagentActualCycles();
                        case Global::RMS_DAYS:
                           QDateTime tempDateTime = p_Station->GetDashboardReagentExchangeDate();
                           tempDateTime = tempDateTime.addSecs(Global::AdjustedTime::Instance().GetOffsetSeconds());
                           QDate tempDate = tempDateTime.date();
                           if (mp_UserSettings)
                           {
                               switch(mp_UserSettings->GetDateFormat()){
                               default:
                                   return QString("");
                               case Global::DATE_INTERNATIONAL:
                                   return tempDate.addDays(p_Reagent->GetMaxDays()).toString("dd.MM.yyyy");
                               case Global::DATE_ISO:
                                   return tempDate.addDays(p_Reagent->GetMaxDays()).toString("yyyy-MM-dd");
                               case Global::DATE_US:
                                   return tempDate.addDays(p_Reagent->GetMaxDays()).toString("MM/dd/yyyy");
                             }
                           }
                           break;
                      }
                }
                else
                   return QString("");

            case 3:
                if (mp_ReagentGroupList) {
                    bool isCleaningReagentGroup = mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID())->IsCleaningReagentGroup();
                    if (((Global::RMS_OFF == RMSPROCESSINGOPTION) || (RMSPROCESSINGOPTION != RMSCLEANINGOPTIONS)) && !isCleaningReagentGroup)
                    {
                        return "";
                    }
                    switch (RMSCLEANINGOPTIONS) {
                        default:
                            return QString("");
                    case Global::RMS_CYCLES:
                        return p_Station->GetDashboardReagentActualCycles();
                    case Global::RMS_DAYS:
                       QDateTime tempDateTime = p_Station->GetDashboardReagentExchangeDate();
                       tempDateTime = tempDateTime.addSecs(Global::AdjustedTime::Instance().GetOffsetSeconds());
                       QDate tempDate = tempDateTime.date();
                       if (mp_UserSettings)
                       {
                           switch(mp_UserSettings->GetDateFormat()){
                           default:
                               return QString("");
                           case Global::DATE_INTERNATIONAL:
                               return tempDate.addDays(p_Reagent->GetMaxDays()).toString("dd.MM.yyyy");
                           case Global::DATE_ISO:
                               return tempDate.addDays(p_Reagent->GetMaxDays()).toString("yyyy-MM-dd");
                           case Global::DATE_US:
                               return tempDate.addDays(p_Reagent->GetMaxDays()).toString("MM/dd/yyyy");
                         }
                       }
                       break;
                     }
                    return QString("");
                }
                else
                   return QString("");

            case 4:
                if (mp_UserSettings) {
                    QDateTime tempDateTime = p_Station->GetDashboardReagentExchangeDate();
                    tempDateTime = tempDateTime.addSecs(Global::AdjustedTime::Instance().GetOffsetSeconds());
                    QDate tempDate = tempDateTime.date();
                    switch(mp_UserSettings->GetDateFormat()){
                    default:
                        return QString("");
                    case Global::DATE_INTERNATIONAL:
                        return tempDate.toString("dd.MM.yyyy");
                    case Global::DATE_ISO:
                        return tempDate.toString("yyyy-MM-dd");
                    case Global::DATE_US:
                        return tempDate.toString("MM/dd/yyyy");
                    }
                    return QString("");
                 }
                 else
                   return QString("");
            case 5:
                if (p_Reagent)
                     return p_Station->GetDashboardReagentStatus();
                else
                     return QString("");
            }

        }
        if (Role == (int)Qt::UserRole)
            return p_Station->GetDashboardStationID();
    } else if (Role == (int)Qt::BackgroundRole) {
         QPalette Palette;
         return QVariant(Palette.color(QPalette::Window));
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
QVariant CReagentStatusModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::SizeHintRole && Orientation == Qt::Horizontal) {
        QSize size(100, 70);
        return size;
    }

    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {
        switch (Section) {//column head
        case 0:
            return tr("Station");
        case 1:
            return tr("Reagent");
        case 2:
            switch (RMSPROCESSINGOPTION) {
                default:
                    return QString("");
                case Global::RMS_CASSETTES:
                    return tr("Cassettes\nsince\nchange");
                case Global::RMS_CYCLES:
                    return tr("Cycles\nsince\nchange");
                case Global::RMS_DAYS:
                    return tr("Expiry\nDate");
            }
        case 3:
            switch (RMSCLEANINGOPTIONS) {
                default:
                    return QString("");
            case Global::RMS_CYCLES:
                return tr("Cycles\nsince\nchange");
            case Global::RMS_DAYS:
                return tr("Expiry\nDate");

            }
        case 4:
            return tr("Exchange\nDate");
        case 5:
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
QString CReagentStatusModel::GetReagentID(const QString ReagentName)
{
    return m_Identifiers.value(ReagentName);

}

/****************************************************************************/
/*!
  * \brief Returns true if reagent is present in the reagent list else false
  *  is returned.
  * \iparam ReagentID
  * \return  ReagentID = Reagent ID
  */
/****************************************************************************/
bool CReagentStatusModel::ContainsReagent(QString ReagentID)
{
    if (m_VisibleReagentIds.isEmpty()) {
        return false;
    }
    if (m_VisibleReagentIds.contains(ReagentID)) {
        return true;
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets UserSettings for the model.
 *  \iparam p_UserSettings = UserSettings
 */
/****************************************************************************/
void CReagentStatusModel::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{
        mp_UserSettings = p_UserSettings;
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
Qt::ItemFlags CReagentStatusModel::flags(const QModelIndex &Index) const
{
    return QAbstractItemModel::flags(Index);
}
} // end namespace Core
