/****************************************************************************/
/*! \file ReagentGroupModel.cpp
 *
 *  \brief ReagentGroupModel Implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-02-11 , 2013-02-27
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
#include "Core/Include/ReagentGroupModel.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MessageDlg.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/MainWindow.h"
#include "Programs/Include/ProgramWidget.h"



namespace Core {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CReagentGroupModel::CReagentGroupModel(QObject *p_Parent) : QAbstractTableModel(p_Parent),
    m_ShowCleaningReagent(true)
{
    mp_ReagentGroupList = NULL;
    m_Columns = 0;
    m_VisibleRowCount = 7;
}


/****************************************************************************/
/*!
 *  \brief Initializes the reagent group data
 *
 *  \iparam p_ReagentGroupList = Reagent data
 *  \param Columns =  qint32 type parameter
 *  \param bShowCleaningReagent = whether show cleaning reagent or not
 */
 /****************************************************************************/
void CReagentGroupModel::SetReagentGroupList(DataManager::CDataReagentGroupList *p_ReagentGroupList,
                                             qint32 Columns, bool bShowCleaningReagent)
{
    m_ShowCleaningReagent = bShowCleaningReagent;
    mp_ReagentGroupList = p_ReagentGroupList;
    m_Columns = Columns;
    UpdateReagentGroupList();
}

/****************************************************************************/
/*!
 *  \brief Updates the reagent data
 *
 *  This slot needs to be called whenever the content of the reagent list is
 *  changed.
 */
/****************************************************************************/
void CReagentGroupModel::UpdateReagentGroupList()
{
    beginResetModel();
    m_Identifiers.clear();
    m_ReagentGroupNames.clear();
    if (mp_ReagentGroupList) {
        for(qint32 i = 0; i < mp_ReagentGroupList->GetNumberOfReagentGroups(); i++) {
            DataManager::CReagentGroup *p_ReagentGroup = NULL;
            p_ReagentGroup = const_cast<DataManager::CReagentGroup*>(mp_ReagentGroupList->GetReagentGroup(i));
            if (!m_ShowCleaningReagent)
            {
                if (!p_ReagentGroup->IsCleaningReagentGroup()) {
                    m_ReagentGroupNames << p_ReagentGroup->GetReagentGroupName();
                    m_Identifiers[p_ReagentGroup->GetReagentGroupName()] = p_ReagentGroup->GetGroupID();
                }
            }
            else
            {
                m_ReagentGroupNames << p_ReagentGroup->GetReagentGroupName();
                m_Identifiers[p_ReagentGroup->GetReagentGroupName()] = p_ReagentGroup->GetGroupID();
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
void CReagentGroupModel::SetVisibleRowCount(int RowCount)
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
int CReagentGroupModel::rowCount(const QModelIndex &) const
{
    return ((m_ReagentGroupNames.count() < m_VisibleRowCount) ? m_VisibleRowCount : m_ReagentGroupNames.count());
}

/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *
 *  \return Columns count
 */
/****************************************************************************/
int CReagentGroupModel::columnCount(const QModelIndex &) const
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
QVariant CReagentGroupModel::data(const QModelIndex &Index, int Role) const
{
    DataManager::CReagentGroup *p_ReagentGroup = NULL;
    if (mp_ReagentGroupList == NULL) {
        return QVariant();
    }

    if (Index.row() < m_ReagentGroupNames.count() && (p_ReagentGroup = const_cast<DataManager::CReagentGroup*>(mp_ReagentGroupList->GetReagentGroup(m_Identifiers[m_ReagentGroupNames[Index.row()]])))){
        if (Role == (int)Qt::DisplayRole) {
            switch (Index.column()) {
            case 0:
                return p_ReagentGroup->GetReagentGroupName();
            }
        }

        if (Role == (int)Qt::UserRole) {
            return p_ReagentGroup->GetGroupID();
        }
        if (Role == (int)Qt::BackgroundRole) {
            if (Index.column() == 1) {
                if (mp_ReagentGroupList) {
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
QVariant CReagentGroupModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {
        switch (Section) {
        case 0:
            return tr("Reagent Group");
        case 1:
            return tr("Color");
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
QString CReagentGroupModel::GetReagentID(const QString ReagentName)
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
bool CReagentGroupModel::ContainsReagent(QString ReagentID)
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
void CReagentGroupModel::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{
        m_UserSettings = *p_UserSettings;
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
Qt::ItemFlags CReagentGroupModel::flags(const QModelIndex &Index) const
{
    if (Index.column() == 1) {
        return Qt::ItemIsEnabled;
    }
    if (mp_ReagentGroupList) {
        if (m_ShowCleaningReagent)
        {
            if (Index.row() >= mp_ReagentGroupList->GetNumberOfReagentGroups()) {
                return Qt::NoItemFlags;
            }
        }
        else
        {
            if (Index.row() >= mp_ReagentGroupList->GetNumberOfReagentGroups() - 2) {
                return Qt::NoItemFlags;
            }
        }
    }

    return QAbstractItemModel::flags(Index);
}
/****************************************************************************/

} // end namespace Core
