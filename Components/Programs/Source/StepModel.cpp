/****************************************************************************/
/*! \file StepModel.cpp
 *
 *  \brief Implementation file for class CStepModel.
 *
 *  \b Description:
 *          This class implements the Program Step Model required for Edit/Add
 *          Program widget to display list of program steps of selected Program
 *          in the table.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-15
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
#include "Programs/Include/StepModel.h"
#include "Programs/Include/ModifyProgramDlg.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"
#include "Core/Include/GlobalHelper.h"

//lint -e613

namespace Programs {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CStepModel::CStepModel(QObject *p_Parent) : QAbstractTableModel(p_Parent),
                                             mp_Program(NULL), mp_ReagentList(NULL),
                                             m_Columns(0), m_CurrentRow(0),
                                             mp_ModifyProgramDlg(NULL), m_VisibleRowCount(6),
                                             mp_ReagentGroupList(NULL),
                                              mp_UserSettings(NULL),
                                             m_IsShowStation(false),
                                             m_CurSelectRowIndex(-1)
{

}

/****************************************************************************/
/*!
 *  \brief Initializes the program data
 *
 *  \iparam p_Program = Program data
 *  \param  p_UserSettings
 *  \param  p_ReagentGroupList
 *  \iparam p_ReagentList = Reagent list
 *  \iparam Columns = Table columns
 */
/****************************************************************************/
void CStepModel::SetProgram(DataManager::CProgram *p_Program, DataManager::CUserSettings *p_UserSettings, DataManager:: CDataReagentGroupList *p_ReagentGroupList, DataManager::CDataReagentList *p_ReagentList,
                            qint32 Columns)
{
    beginResetModel();
    mp_Program = p_Program;
    mp_ReagentList = p_ReagentList;
    mp_ReagentGroupList = p_ReagentGroupList;
    m_Columns = Columns;
    m_CurrentRow = 0;
    mp_UserSettings = p_UserSettings;
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief A copy of Modify Program Dlg pointer
 *
 *  \iparam p_ModifyProgram = Pointer to Modify Program Dlg
 */
/****************************************************************************/
void CStepModel::SetModifyProgramDlgPtr(CModifyProgramDlg *p_ModifyProgram)
{
    mp_ModifyProgramDlg = p_ModifyProgram;
}

/****************************************************************************/
/*!
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount
 */
/****************************************************************************/
void CStepModel::SetVisibleRowCount(int RowCount) {
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
int CStepModel::rowCount(const QModelIndex &) const
{
    if (mp_Program != NULL) {
        int NoOfSteps = mp_Program->GetNumberOfSteps();
        return ((NoOfSteps < m_VisibleRowCount) ? m_VisibleRowCount : NoOfSteps);
    }
    else {
        return m_VisibleRowCount;
    }
}

/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *
 *  \return Columns count
 */
/****************************************************************************/
int CStepModel::columnCount(const QModelIndex &) const
{
    return m_Columns;
}

QString CStepModel::ShowPV(DataManager::CProgramStep *Step) const
{
    if (Step->GetVacuum() == "On" && Step->GetPressure() == "Off")
        return QString("V");

    else if (Step->GetPressure() == "On" && Step->GetVacuum() == "Off")
        return QString("P");

    else if( Step->GetVacuum() == "Off" && Step->GetPressure() == "Off")
    {
        return QString("-");
    }
    else if (Step->GetVacuum() == "On" && Step->GetPressure() == "On")
    {
        return QString("P/V");
    }
    return QString("");
}

QVariant CStepModel::ShowTemperature(DataManager::CProgramStep *Step) const
{
    if (Step->GetTemperature() == "-1")
    {
        return "-";
    }

    if (mp_UserSettings->GetTemperatureFormat() == Global::TEMP_FORMAT_CELSIUS)
    {
        return Step->GetTemperature();
    }
    else
    {
        double Temperature = Step->GetTemperature().toDouble() * 9 / 5 + 32;
        return qRound(Temperature);
    }
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
QVariant CStepModel::data(const QModelIndex &Index, int Role) const
{
    DataManager::CProgramStep *Step;
    DataManager::CReagent *p_Reagent = NULL;

    if (mp_Program == NULL || mp_ReagentList == NULL) {
        if (Role == (int)Qt::BackgroundRole) {
            QPalette Palette;
            return QVariant(Palette.color(QPalette::Window));
        }
        return QVariant();
    }
    Step = const_cast<DataManager::CProgramStep*>(mp_Program->GetProgramStep(Index.row()));

    if (Step != NULL) {
        if (Role == (int)Qt::DisplayRole) {
            switch (Index.column()) {
            case 0:
                return Index.row() + 1;
            case 1:
            {
                DataManager::CReagent Reagent;
                if (mp_ReagentList->GetReagent(Step->GetReagentID(), Reagent) == true) {
                    return Reagent.GetReagentName();
                }
                else {
                    return QVariant();
                }
            }

            case 2:
            {
                if (!m_IsShowStation) {
                    return Core::CGlobalHelper::TimeToString(Step->GetDurationInSeconds());
                }
                else {
                    if (Index.row() <= mp_DashboardStationNameList.count()-1)
                        return mp_DashboardStationNameList.at(Index.row());
                    else
                        return QVariant();
                }
            }
            case 3:
                if (!m_IsShowStation)
                {
                    return ShowTemperature(Step);
                }
                else
                {
                    return Core::CGlobalHelper::TimeToString(Step->GetDurationInSeconds());
                }
            case 4:
            {
                if (!m_IsShowStation)
                {
                    return ShowPV(Step);
                }
                else
                {
                   return ShowTemperature(Step);
                }
            }
            if (m_IsShowStation)
            {
                case 5:
                {
                    return ShowPV(Step);
                }
            }
        }
        }
        else if (Role == (int)Qt::UserRole) {
            return Step->GetStepID();
        }
        else if (Role == (int)Qt::BackgroundColorRole) {

            p_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(Step->GetReagentID()));

            if (Index.column() == 0) {
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
        else if (Role == (int)Qt::ForegroundRole) {
                    //Grays
                    QPalette Palette;
                    if (m_IsShowStation && (Index.row() < m_CurSelectRowIndex))
                        return QVariant(Palette.color(QPalette::Dark));
                }
    }
    else if (Role == (int)Qt::ForegroundRole) {
        //Grays the empty lines
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
QVariant CStepModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {
        switch (Section) {
        case 0:
            return tr("Step");
        case 1:
            return tr("Reagent");

        case 2:
            if (!m_IsShowStation)
                return tr("Duration");
            else return m_strStation;
        case 3:
            if (!m_IsShowStation)
                return tr("Temperature");
            else return tr("Duration");

        case 4:
            if (!m_IsShowStation)
                return tr("P/V");
            else return m_strTemp;

            if (m_IsShowStation)
            {
               case 5:
                    return tr("P/V");
            }
       }
    }
    return QVariant();
}

void CStepModel::ShowStation(bool bSet,
                             const QString& strStation,
                             const QString& strTemprature)
{
    m_IsShowStation = bSet;
    m_strStation = strStation;
    m_strTemp = strTemprature;
}

void CStepModel::SetStationNameList(QList<QString>& stationNameList)
{
    mp_DashboardStationNameList.clear();
    mp_DashboardStationNameList = stationNameList;
}

void CStepModel::SetCurSelectRowIndex(int idx)
{
    m_CurSelectRowIndex= idx;
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
Qt::ItemFlags CStepModel::flags(const QModelIndex &Index) const
{
    if (mp_Program) {
        if (Index.row() >= mp_Program->GetNumberOfSteps()) {
            return Qt::NoItemFlags;
        }
    }
    else {
        return Qt::NoItemFlags;
    }
    return QAbstractItemModel::flags(Index);
}

/****************************************************************************/
/*!
 *  \brief Sets the StepModel Index
 *  \iparam Index
 */
/****************************************************************************/
void CStepModel::SetIndex(QModelIndex Index)
{
    m_CurrentRow = Index.row();
}

/****************************************************************************/
/*!
 *  \brief Sets the StepModel Index
 *  \return m_CurrentRow
 */
/****************************************************************************/
qint32 CStepModel::GetIndex()
{
    return  m_CurrentRow;
}

} // end namespace Programs
