/****************************************************************************/
/*! \file ReagentStationEditModel.cpp
 *
 *  \brief ReagentStationEditModel Implementation.
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Core/Include/ReagentStationEditModel.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MessageDlg.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/MainWindow.h"
#include "Programs/Include/ProgramWidget.h"
#include "Application/Include/LeicaStyle.h"

#include <QPixmap>
#include <QDebug>

namespace Core {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CReagentStationEditModel::CReagentStationEditModel(QObject *p_Parent) : QAbstractTableModel(p_Parent)
{
    mp_ReagentList = NULL;
    mp_Parent = NULL;
    m_FilterLeicaReagent = false;
    m_ParaffinReagent = false;
    m_Columns = 0;
    m_VisibleRowCount = 7;
}

/****************************************************************************/
/*!
 *  \brief Initializes the reagent data
 *
 *  \iparam p_ReagentList = Reagent data
 *  \iparam Columns = Table columns
 *  \iparam BLCheck = True for displaying reagents in bathlayout else False.
 */
 /****************************************************************************/
void CReagentStationEditModel :: SetRequiredContainers(DataManager::CDataReagentList *p_ReagentList,
                           DataManager::CDataReagentGroupList *p_ReagentGroupList,
                           DataManager::CDashboardDataStationList *p_DashboardDataStationList, qint32 Columns)
{
    mp_ReagentList = p_ReagentList;
    mp_ReagentGroupList = p_ReagentGroupList;
    mp_DashboardDataStationList = p_DashboardDataStationList;
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
void CReagentStationEditModel::UpdateReagentList()
{
    beginResetModel();
    m_Identifiers.clear();
    m_ReagentNames.clear();
    m_ReagentNameMap.clear();
    m_ReagentID.clear();

    if (mp_ReagentList) {
        for(qint32 i = 0; i < mp_ReagentList->GetNumberOfReagents(); i++) {
            DataManager::CReagent *p_Reagent = NULL;
            p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(i));
            if (p_Reagent) {
                if (p_Reagent->GetReagentType() == USER_REAGENT) {
                    if(m_ParaffinReagent == true && mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID())->GetReagentGroupName() .toLower() == QString("Paraffin").toLower()){
                        m_ReagentNames << p_Reagent->GetReagentName();
                        m_ReagentID << p_Reagent->GetReagentID();
                        m_Identifiers[p_Reagent->GetReagentName()] = p_Reagent->GetReagentID();
                    }
                    else if(m_ParaffinReagent == false && mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID())->GetReagentGroupName().toLower() != QString("Paraffin").toLower()) {
                        m_ReagentNames << p_Reagent->GetReagentName();
                        m_ReagentID << p_Reagent->GetReagentID();
                        m_Identifiers[p_Reagent->GetReagentName()] = p_Reagent->GetReagentID();
                    }
                    if(p_Reagent->GetVisibleState()== true){
                        m_VisibleReagentIds << p_Reagent->GetReagentName();
                    }
                }

                if (p_Reagent->GetReagentType() == LEICA_REAGENT && (!m_FilterLeicaReagent)) {

                    if(m_ParaffinReagent == true && mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID())->GetReagentGroupName().toLower() == QString("Paraffin").toLower()){
                        m_ReagentNames << p_Reagent->GetReagentName();
                        m_ReagentID << p_Reagent->GetReagentID();
                        m_Identifiers[p_Reagent->GetReagentName()] = p_Reagent->GetReagentID();
                    }
                    else if(m_ParaffinReagent == false && mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID())->GetReagentGroupName().toLower() != QString("Paraffin").toLower()) {
                        m_ReagentNames << p_Reagent->GetReagentName();
                        m_ReagentID << p_Reagent->GetReagentID();
                        m_Identifiers[p_Reagent->GetReagentName()] = p_Reagent->GetReagentID();
                    }

                    if(p_Reagent->GetVisibleState()== true){
                        m_VisibleReagentIds << p_Reagent->GetReagentName();
                    }
                }
            }
        }
        m_ReagentNames.append("");
        m_ReagentID.append("");
    }
    foreach (const QString str, m_ReagentNames)
        (void)m_ReagentNameMap.insertMulti(str.toLower(), str);
    //m_ReagentNames = m_ReagentNameMap.values();

    endResetModel();
}



/****************************************************************************/
/*!
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount
 */
/****************************************************************************/
void CReagentStationEditModel::SetVisibleRowCount(int RowCount)
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
int CReagentStationEditModel::rowCount(const QModelIndex &) const
{
    return ((m_ReagentID.count() < m_VisibleRowCount) ? m_VisibleRowCount : m_ReagentID.count());
}

/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *
 *  \return Columns count
 */
/****************************************************************************/
int CReagentStationEditModel::columnCount(const QModelIndex &) const
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
QVariant CReagentStationEditModel::data(const QModelIndex &Index, int Role) const
{
    DataManager::CReagent *p_Reagent = NULL;
    if (mp_ReagentList == NULL) {
        return QVariant();
    }

        if (Index.row() < m_ReagentID.count()){

            if(m_ReagentID[Index.row()] != QString(""))
                p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(m_ReagentID[Index.row()]));
            else
                p_Reagent = NULL;

            if (Role == (int)Qt::DisplayRole) {
                switch (Index.column()) {
                case 0:
                    if(p_Reagent == NULL)
                        return QString("None");
                    else
                        return p_Reagent->GetReagentName();
                case 1:
                     if (mp_ReagentGroupList) {
                        if(p_Reagent){
                            DataManager::CReagentGroup *p_ReagentGroup = const_cast<DataManager::CReagentGroup*>(mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID()));
                            if (p_ReagentGroup) {
                                return p_ReagentGroup->GetReagentGroupName();
                            }
                         }
                         else {
                             return QString("");
                          }
                      }
                  }
              }

            if (Role == (int)Qt::UserRole) {
                return m_ReagentID[Index.row()];
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
                        if(p_Reagent) {
                            DataManager::CReagentGroup *p_ReagentGroup = const_cast<DataManager::CReagentGroup*>(mp_ReagentGroupList->GetReagentGroup(p_Reagent->GetGroupID()));
                              if (p_ReagentGroup) {
                                    QColor Color;
                                    // add '#' to hex value to change to color value
                                    Color.setNamedColor("#" + p_ReagentGroup->GetGroupColor().trimmed());
                                    QPalette Palete(Color);
                                    return QVariant(Palete.color(QPalette::Window));
                               }
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
QVariant CReagentStationEditModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {
        switch (Section) {
        case 0:
            return tr("Reagent");
        case 1:
            return tr("Group");
        }
    }
    return QVariant();
}

/****************************************************************************/
/*!
 *  \brief This Function Gets Reagent ID of given Reagent Long name.
 *  \iparam ReagentLongName
 */
/****************************************************************************/
QString CReagentStationEditModel::GetReagentID(const QString ReagentName)
{
    return m_Identifiers.value(ReagentName);

}

/****************************************************************************/
  /*!
   * \brief Returns the Reagent long Name for the particular row.
   * \iparam Row
   * \return  m_ReagentNames = Reagent Long Name
   */
/****************************************************************************/
QString CReagentStationEditModel::GetReagentLongName(int Row)
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
  *  is returned.
  * \iparam ReagentID
  * \return  ReagentID = Reagent ID
  */
/****************************************************************************/
bool CReagentStationEditModel::ContainsReagent(QString ReagentID)
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
 *  \brief Returns item model flags of a cell
 *
 *  \iparam Index = Index of a table cell
 *
 *  \return Flags of the cell
 */
/****************************************************************************/
Qt::ItemFlags CReagentStationEditModel::flags(const QModelIndex &Index) const
{
    if (Index.column() == 1) {
        return Qt::NoItemFlags;
    }
    return QAbstractItemModel::flags(Index);
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
QModelIndex CReagentStationEditModel::CreateIndex(int Row, int Column)
{
    return createIndex(Row, Column);
}
/****************************************************************************/

} // end namespace Core
