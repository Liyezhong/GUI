/****************************************************************************/
/*! \file ReagentRMSModel.cpp
 *
 *  \brief ReagentRMSModel Implementation.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2011-08-23, 2013-01-17 ,2013-02-26
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Core/Include/ReagentRMSModel.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MessageDlg.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/MainWindow.h"
#include "Programs/Include/ProgramWidget.h"
#include "Application/Include/LeicaStyle.h"
#include "Programs/Include/ModifyProgramStepDlg.h"
#include "Reagents/Include/ReagentRMSWidget.h"


namespace Core {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CReagentRMSModel::CReagentRMSModel(QObject *p_Parent) : QAbstractTableModel(p_Parent)
{
    mp_ReagentList = NULL;
    mp_ReagentGroupList =NULL;
    mp_Parent = NULL;
    m_HideSpecialReagents = false;
    m_FilterLeicaReagent = false;
    m_CleaningReagent = true;
    m_Columns = 0;
    m_VisibleRowCount = 7;
    (void)m_PixmapTickOk.load(QString(":/%1/Icons/MISC/TickOk.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
}

/****************************************************************************/
/*!
 *  \brief Initializes the reagent data
 *
 *  \iparam p_ReagentList = Reagent data
 *  \iparam Columns = Table columns
 */
/****************************************************************************/
void CReagentRMSModel::SetReagentList(DataManager::CDataReagentList *p_ReagentList, qint32 Columns)
{
    mp_ReagentList = p_ReagentList;
    m_Columns = Columns;
    UpdateReagentList();
}

/****************************************************************************/
/*!
 *  \brief Initializes the reagent group data
 *
 *  \iparam p_ReagentGroupList = Reagent data
 */
 /****************************************************************************/
void CReagentRMSModel::SetReagentGroupList(DataManager::CDataReagentGroupList *p_ReagentGroupList)
{
    mp_ReagentGroupList = p_ReagentGroupList;
    UpdateReagentList();
}

/****************************************************************************/
/*!
 *  \brief Updates the reagent data. This slot needs to be called whenever
 *  the content of the reagent list is changed.
 */
/****************************************************************************/
void CReagentRMSModel::UpdateReagentList()
{
    beginResetModel();
    m_Identifiers.clear();
    m_ReagentNames.clear();
    m_ReagentID.clear();
    m_CleaningGroupReagentNames.clear();
    m_CleaningGroupReagentID.clear();

    if (mp_ReagentList && mp_ReagentGroupList) {
        for(qint32 i = 0; i < mp_ReagentList->GetNumberOfReagents(); i++) {
            DataManager::CReagent *p_Reagent = NULL;
            p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(i));
            DataManager::CReagentGroup *p_ReagentGroup=mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID());
            bool Check = p_ReagentGroup->IsCleaningReagentGroup();
            if (p_Reagent) {
                if ((p_Reagent->GetReagentType() != LEICA_REAGENT || (!m_FilterLeicaReagent)) \
                       && Check == false && m_CleaningReagent == false){
                     UpdateList(p_Reagent);
                 }
                else if ((p_Reagent->GetReagentType() != LEICA_REAGENT || (!m_FilterLeicaReagent)) \
                    && Check == true && m_CleaningReagent == true) {
                    UpdateList(p_Reagent);
                }

                if(p_Reagent->GetVisibleState()== true){
                    m_VisibleReagentIds << p_Reagent->GetReagentName();
                }
            }
        }
    }
    endResetModel();
}

void CReagentRMSModel ::UpdateList( DataManager::CReagent *p_Reagent)
{
     m_ReagentNames << p_Reagent->GetReagentName();
     m_ReagentID<<p_Reagent->GetReagentID();
     m_Identifiers[p_Reagent->GetReagentID()] = p_Reagent->GetReagentName();
}
/****************************************************************************/
/*!
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount = Visible row count
 */
/****************************************************************************/
void CReagentRMSModel::SetVisibleRowCount(int RowCount)
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
int CReagentRMSModel::rowCount(const QModelIndex &) const
{
    return ((m_Identifiers.count() < m_VisibleRowCount) ? m_VisibleRowCount : m_Identifiers.count());
}
/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *
 *  \return Columns count
 */
/****************************************************************************/
int CReagentRMSModel::columnCount(const QModelIndex &) const
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
QVariant CReagentRMSModel::data(const QModelIndex &Index, int Role) const
{
    DataManager::CReagent *p_Reagent = NULL;
    if (mp_ReagentList == NULL) {
        return QVariant();
    }

    if (Index.row() < m_Identifiers.count() && (p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(m_ReagentID[Index.row()])))){
        if (Role == (int)Qt::DisplayRole) {
            switch (Index.column()) {
            case 0:
                return p_Reagent->GetReagentName();
            case 1:
                if (mp_ReagentGroupList) {
                    DataManager::CReagentGroup *p_ReagentGroup = const_cast<DataManager::CReagentGroup*>(mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID()));
                    if (p_ReagentGroup) {
                        return p_ReagentGroup->GetReagentGroupName();
                    }
                    else {
                        return QString("");
                    }
                }
                break;
            case 2:
                if(m_CleaningReagent == false){
                    switch (Reagents::CReagentRMSWidget::RMSPROCESSINGOPTION) {
                    default:
                        return QString("");
                    case Global::RMS_CASSETTES:
                        return p_Reagent->GetMaxCassettes();
                    case Global::RMS_CYCLES:
                        return p_Reagent->GetMaxCycles();
                    case Global::RMS_DAYS:
                        return p_Reagent->GetMaxDays();
                    case Global::RMS_OFF:
                        return QString("");
                    }
                }
                else {
                    switch (Reagents::CReagentRMSWidget::RMSCLEANINGOPTIONS) {
                    default:
                        return QString("");
                    case Global::RMS_CYCLES:
                        return p_Reagent->GetMaxCycles();
                    case Global::RMS_DAYS:
                        return p_Reagent->GetMaxDays();
                    case Global::RMS_OFF:
                        return QString("");
                    }
                }
            }
        }

        if (Role == (int)Qt::UserRole) {
            return p_Reagent->GetReagentID();
        }

        if (Role == (int)Qt::TextColorRole) {
            if (Index.column() == 1) {
                QColor Color;
                QPalette Palete(Color.black());
                return QVariant(Palete.color(QPalette::Window));
            }
        }

        if (Role == (int)Qt::BackgroundColorRole) {
            if (Index.column() == 1) {
                if (mp_ReagentGroupList) {
                    DataManager::CReagentGroup *p_ReagentGroup = const_cast<DataManager::CReagentGroup*>(mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID()));
                    if (p_ReagentGroup) {
                        QColor Color;
                        // add '#' to hex value to change to color value
                        Color.setNamedColor("#" + p_ReagentGroup->GetGroupColor().trimmed());
                        QPalette Palete(Color);
                        return QVariant(Palete.color(QPalette::Window));
                    }
                    else {
                        QPalette Palette;
                        return QVariant(Palette.color(QPalette::Window));
                    }
                }
            }
        }
    }
    else if (Role == (int)Qt::BackgroundRole) {
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
QVariant CReagentRMSModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {
        switch (Section) {
        case 0:
            return tr("Reagent");
        case 1:
            return tr("Reagent Group");
        case 2:
            if(m_CleaningReagent == false){
                switch (Reagents::CReagentRMSWidget::RMSPROCESSINGOPTION) {
                default:
                    return QString("");
                case Global::RMS_CASSETTES:
                    return tr("Cassettes until change");
                case Global::RMS_CYCLES:
                    return tr("Cycles until change");
                case Global::RMS_DAYS:
                    return tr("Days until change");
                }
            }
            else{
                switch (Reagents::CReagentRMSWidget::RMSCLEANINGOPTIONS) {
                default:
                    return QString("");
                case Global::RMS_CYCLES:
                    return tr("Cycles until change");
                case Global::RMS_DAYS:
                    return tr("Days until change");
                }
             }
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
QString CReagentRMSModel::GetReagentID(const QString ReagentName)
{
    return m_Identifiers.value(ReagentName);

}

/****************************************************************************/
/*!
 *  \brief This Function Gets Reagent ID of given Reagent Long name.
 *
 *  \iparam Index
 *
 *  \return  return from GetReagentID
 */
/****************************************************************************/
QString CReagentRMSModel::GetReagentID(int Index)
{
    return m_ReagentID.value(Index);

}
/****************************************************************************/
/*!
   * \brief Returns the Reagent long Name for the particular row.
   *
   * \iparam Row = Selected row
   *
   * \return  m_ReagentNames = Reagent Name
   */
/****************************************************************************/
QString CReagentRMSModel::GetReagentLongName(int Row)
{
    if (!m_ReagentNames.isEmpty()) {
        return m_ReagentNames[Row];
    }
    else {
        return "";
    }
}

/****************************************************************************/
/*!
  * \brief Returns true if reagent is present in the reagent list else false
  *        is returned.
  *
  * \iparam ReagentID = Selected Reagent ID
  *
  * \return  True = If reagent exists in the list else False.
  */
/****************************************************************************/
bool CReagentRMSModel::ContainsReagent(QString ReagentID)
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
void CReagentRMSModel::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{
        m_UserSettings = *p_UserSettings;
}

/****************************************************************************/
QVariant CReagentRMSModel::dataStatus(const QModelIndex &Index, int Role) const
{
    DataManager::CReagent *p_Reagent = NULL;
    if (mp_ReagentList == NULL) {
        return QVariant();
    }

    if (Index.row() < m_ReagentNames.count() && (p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(m_Identifiers[m_ReagentNames[Index.row()]])))){
        if (Role == (int)Qt::DisplayRole) {
            switch (Index.column()) {
//            case 0:
//                return p_Reagent->GetReagentStation();
            case 1:
                return p_Reagent->GetReagentName();
//            case 2:
//                return p_Reagent->GetCassetteUntilChange();
//            case 3:
//                return p_Reagent->GetcassetteOverdue();
//            case 4:
//                return p_Reagent->GetExpirayDate();
//            case 5:
//                return p_Reagent->GetStatus();
            }
        }

        if (Role == (int)Qt::UserRole) {
            return p_Reagent->GetReagentName();
        }
    }
    else if (Role == (int)Qt::BackgroundRole) {
        QPalette Palette;
        return QVariant(Palette.color(QPalette::Window));
    }
    return QVariant();
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
Qt::ItemFlags CReagentRMSModel::flags(const QModelIndex &Index) const
{
    if (mp_ReagentList) {
        if (Index.row() >= mp_ReagentList->GetNumberOfReagents()) {
            return Qt::NoItemFlags;
        }
    }
    return QAbstractItemModel::flags(Index);
}

/****************************************************************************/
/*!
 *  \brief Returns item model flags of a cell
 *
 *  \iparam Row
 *  \param  Column
 *
 *  \return Flags of the cell
 */
/****************************************************************************/
QModelIndex CReagentRMSModel::CreateIndex(int Row, int Column)
{
    return createIndex(Row, Column);
}



} // end namespace Core
