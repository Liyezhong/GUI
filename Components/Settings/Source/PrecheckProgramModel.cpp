
/****************************************************************************/
/*! \file PrecheckProgramModel.cpp
 *
 *  \brief PrecheckProgramModel implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-06
 *   $Author:  $ C.Adaragunchi
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
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Settings/Include/PrecheckProgramModel.h"
#include <QApplication>
#include <QEvent>
#include "Core/Include/GlobalHelper.h"


namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CPrecheckProgramModel::CPrecheckProgramModel(QObject *p_Parent):
    QAbstractTableModel(p_Parent),
    m_Columns(2),
    m_VisibleRowCount(6),
    m_CurrentUserRole(MainMenu::CMainWindow::Operator)
{

}

/****************************************************************************/
/*!
 *  \brief Initializes the program data
 *
 *  \iparam p_ProgramList = Program data
 */
/****************************************************************************/
void CPrecheckProgramModel::SetProgramList(DataManager::CDataProgramList *p_ProgramList)
{
    beginResetModel();
    m_ProgramList = *p_ProgramList;
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount
 */
/****************************************************************************/
void CPrecheckProgramModel::SetVisibleRowCount(qint32 RowCount)
{
    beginResetModel();
    m_VisibleRowCount = RowCount;
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief Returns the number of rows in the table
 *  \iparam p_parent
 *  \return Row count
 */
/****************************************************************************/
int CPrecheckProgramModel::rowCount(const QModelIndex &p_parent) const
{
    Q_UNUSED(p_parent)
    int Count = m_ProgramList.GetNumberOfPrograms();
    return ((Count < m_VisibleRowCount) ? m_VisibleRowCount : Count);
}

/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *  \iparam p_parent
 *  \return Columns count
 */
/****************************************************************************/
int CPrecheckProgramModel::columnCount(const QModelIndex &p_parent) const
{
    Q_UNUSED(p_parent)
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
QVariant CPrecheckProgramModel::data(const QModelIndex &Index, int Role) const
{
    const DataManager::CProgram *p_Program = NULL;
    if (m_ProgramList.GetNumberOfPrograms() == 0) {
        return QVariant();
    }

    p_Program = m_ProgramList.GetProgram(Index.row());
    if (p_Program) {
        if (Role == (int)Qt::DisplayRole) {
            if (Index.column() == 1)
            {
                return p_Program->GetName();
            }
        }
        else if (Role == (int)Qt::CheckStateRole) {
            if (Index.column() == 0)
            {
                if (p_Program->GetBottleCheck()) {
                    return (int)Qt::Checked;
                }
                else
                {                    
                    return (int)Qt::Unchecked;
                }
            }
        }
        else if (Role == (int)Qt::UserRole) {
            return p_Program->GetID();
        }
    }
    else if (Role == (int)Qt::BackgroundRole) {
        //Grays the empty lines
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
Qt::ItemFlags CPrecheckProgramModel::flags(const QModelIndex &Index) const
{
    if (!Index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    if (m_ProgramList.GetNumberOfPrograms() > 0 ) {
        if (Index.row() >= m_ProgramList.GetNumberOfPrograms()) {
            return Qt::NoItemFlags;
        }
    }

    if (Index.column() == 0) {
        if (m_CurrentUserRole == MainMenu::CMainWindow::Admin
                || m_CurrentUserRole == MainMenu::CMainWindow::Service)
        {
            if (m_ProgramList.GetNumberOfPrograms() > 0)
            {
                bool ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
                bool CanEdit = Core::CGlobalHelper::CheckIfCanEdit();
                if ( ProcessRunning == true || CanEdit == false ) {
                    return Qt::ItemIsSelectable;
                }
            }
            return QAbstractItemModel::flags(Index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
        }
        else {         
            return Qt::ItemIsSelectable;
        }
    }
    return QAbstractItemModel::flags(Index);
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
QVariant CPrecheckProgramModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {        
         switch (Section) {
        case 0:
            return tr("Apply");
        case 1:
            return tr("Program");
        }
    }
    return QVariant();
}

void CPrecheckProgramModel::SetUserRole(MainMenu::CMainWindow::UserRole_t UserRole)
{
    m_CurrentUserRole = UserRole;
}

/****************************************************************************/
/*!
 *  \brief Sets the content of table items
 *
 *  \iparam Index = Index of a table cell
 *  \iparam Value = The new value
 *  \iparam Role = Display role to be modified
 *
 *  \return Changed something (true) or not (false)
 */
/****************************************************************************/
bool CPrecheckProgramModel::setData(const QModelIndex &Index, const QVariant &Value, int Role)
{
    if (Role == (int)Qt::CheckStateRole) {
        DataManager::CProgram *p_Program = m_ProgramList.GetProgram(Index.row());
        if (p_Program) {
            if (Index.column() == 0) {
                if (Value.toBool() == true)
                {
                    beginResetModel();
                    p_Program->SetBottleCheck(Value.toBool());
                    endResetModel();
                    return true;
                }
                else if(Value.toBool() == 0)
                {
                    beginResetModel();
                    p_Program->SetBottleCheck(false);
                    endResetModel();
                    return false;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return false;
}

void CPrecheckProgramModel::OnUpdateProgramList()
{
    beginResetModel();
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief get all bottle chack flags
 *
 *  \return all bottle chack flags
 */
/****************************************************************************/
QStringList CPrecheckProgramModel::GetBottleCheckFlags()
{
    QStringList strList;
    int count = m_ProgramList.GetNumberOfPrograms();
    for (int j = 0; j< count; j++)
    {
        if (m_ProgramList.GetProgram(j)->GetBottleCheck())
        {
            strList << "1";
        }
        else
            strList << "0";

    }
    return strList;
}

} // end namespace Programs

