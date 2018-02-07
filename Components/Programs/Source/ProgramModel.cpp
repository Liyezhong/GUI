/****************************************************************************/
/*! \file ProgramModel.cpp
 *
 *  \brief Implementation file for class CProgramModel.
 *
 *  \b Description:
 *          This class implements the Program Model required for Program Widget
 *          to display list of programs in the table.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-25
 *   $Author:  $ M.Scherer, N.Kamath, C.Adaragunchi1234
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
#include "Programs/Include/ProgramModel.h"
#include "Core/Include/GlobalHelper.h"


namespace Programs {
const int VISIBLE_ROW_COUNT = 8;    //!< Number of rows visible in a table
const int NUMBER_OF_COLUMNS = 5;    //!< Number of columns in a table
const int MAX_FAVORITE_PROGRAM_COUNT = 5; //!< Maxiimum number of favourite Programs
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CProgramModel::CProgramModel(QObject *p_Parent) :
QAbstractTableModel(p_Parent),
mp_ProgramList(NULL),
m_Columns(0),
m_VisibleRowCount(VISIBLE_ROW_COUNT),
m_CurrentUserRole(MainMenu::CMainWindow::Operator),
m_CurrentIndex(0)
{

}

/****************************************************************************/
/*!
 *  \brief Initializes the program data
 *
 *  \iparam p_ProgramList = Program data
 *  \iparam Columns = Table columns
 */
/****************************************************************************/
void CProgramModel::SetProgramList(DataManager::CDataProgramList *p_ProgramList, qint32 Columns)
{
    mp_ProgramList = p_ProgramList;
    m_Columns = Columns;
}

/****************************************************************************/
/*!
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount
 */
/****************************************************************************/
void CProgramModel::SetVisibleRowCount(int RowCount) {
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
int CProgramModel::rowCount(const QModelIndex &) const
{
    if (mp_ProgramList != NULL) {
        //Show empty rows if number of programs is less than visible rows in table.
        int Count = mp_ProgramList->GetNumberOfPrograms();
        return ((Count < m_VisibleRowCount) ? m_VisibleRowCount : Count);
    }
    else {
        return 0;
    }
}

/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *
 *  \return Columns count
 */
/****************************************************************************/
int CProgramModel::columnCount(const QModelIndex &) const
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
QVariant CProgramModel::data(const QModelIndex &Index, int Role) const
{
    DataManager::CProgram *p_Program = NULL;
    if (mp_ProgramList == NULL) {
        return QVariant();
    }
    p_Program = const_cast<DataManager::CProgram*>(mp_ProgramList->GetProgram(Index.row()));
    if (p_Program) {
        if (Role == (int)Qt::DisplayRole) {
            switch (5 - m_Columns + Index.column()) {
            case -1:
                return Index.row() + 1;
            case 0:
                return Index.row() + 1;
            case 2:
                return p_Program->GetName();
            case 3:
                return Core::CGlobalHelper::TimeToString(p_Program->GetProgramDurationInSeconds());
            }
        }
        else if (Role ==(int)Qt::DecorationRole)
        {
            switch (5 - m_Columns + Index.column()) {
            case 4:
                QString Icon = p_Program->GetIcon();
                if(Icon.compare("") == true)
                    return QVariant();
                else
                {
                    QPixmap pixmap(":/HimalayaImages/Icons/Program/"+Icon+".png");
                    QPixmap img = pixmap.copy(8, 8, 54, 45);
                    return img.scaledToHeight(32, Qt::SmoothTransformation);
                }
            }
        }
        else if (Role == (int)Qt::CheckStateRole) {
            if (Index.column() == 1)
            {
                if (p_Program->IsFavorite() && p_Program->GetID().at(0) != 'C') {
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
Qt::ItemFlags CProgramModel::flags(const QModelIndex &Index) const
{
    if (mp_ProgramList) {
        if (Index.row() >= mp_ProgramList->GetNumberOfPrograms()) {
            return Qt::NoItemFlags;
        }
    }

    if (Index.column() == 1) {
        if (m_CurrentUserRole == MainMenu::CMainWindow::Admin
                || m_CurrentUserRole == MainMenu::CMainWindow::Service)
        {
            if (mp_ProgramList)
            {
                int FavProgramCount = mp_ProgramList->GetFavoriteProgramIDs().count();
                DataManager::CProgram *p_Program = const_cast<DataManager::CProgram*>(mp_ProgramList->GetProgram(Index.row()));
                if (FavProgramCount == 5 || p_Program->GetID().at(0) == 'C') {
                    if (data(Index, (int)Qt::CheckStateRole) == Qt::Unchecked) {
                        return Qt::ItemIsSelectable;
                    }
                }

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
    if (NUMBER_OF_COLUMNS - m_Columns + Index.column() == 0) {
        return QAbstractItemModel::flags(Index) | Qt::ItemIsUserCheckable;
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
QVariant CProgramModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {
        switch (NUMBER_OF_COLUMNS - m_Columns + Section) {
        case 0:
            return tr("Number");
        case 1:
            return tr("Apply");
        case 2:
            return tr("Name");
        case 3:
            return tr("Duration");
        case 4:
            return tr("Icon");
        }
    }
    return QVariant();
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
bool CProgramModel::setData(const QModelIndex &Index, const QVariant &Value, int Role)
{
    //bool UpdateStat = false;
    if (mp_ProgramList == NULL) {
        return false;
    }
    if (Role == (int)Qt::CheckStateRole) {
        DataManager::CProgram *p_Program = mp_ProgramList->GetProgram(Index.row());
        if (p_Program) {
            if (Index.column() == 1 && p_Program->GetID().at(0) != 'C') {
                int FavProgramCount = mp_ProgramList->GetFavoriteProgramIDs().count();
                if (FavProgramCount < MAX_FAVORITE_PROGRAM_COUNT && Value.toBool() == true)
                {
                    p_Program->SetFavorite(Value.toBool());
                    emit FavoriteProgramListUpdated(*p_Program);
                    return true;
                }
                else if(Value.toBool() == 0)
                {
                    p_Program->SetFavorite(false);
                    emit FavoriteProgramListUpdated(*p_Program);
                    return false;
                }
                else
                {
                    emit FavoriteProgramListUpdated(*p_Program);
                    return false;
                }
            }
        }
    }
    return false;
}

void CProgramModel ::SetUserRole(MainMenu::CMainWindow::UserRole_t UserRole)
{
    m_CurrentUserRole = UserRole;
}

void CProgramModel :: SelectedRowIndex(int Index)
{
    m_CurrentIndex = Index;
}

/****************************************************************************/
/*!
 *  \brief Updates the model whenever GUI receives an updated XML from Master
 */
/****************************************************************************/
void CProgramModel::OnUpdateProgramList()
{
    beginResetModel();
    endResetModel();
}
} // end namespace Programs
