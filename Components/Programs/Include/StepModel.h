/****************************************************************************/
/*! \file StepModel.h
 *
 *  \brief Include file for class CStepModel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-15
 *   $Author:  $ M.Scherer, C.Adaragunchi1234
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

#ifndef PROGRAMS_STEPMODEL_H
#define PROGRAMS_STEPMODEL_H

#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColorList.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "MainMenu/Include/TableMoving.h"
#include "MainMenu/Include/ScrollTable.h"
#include <QAbstractTableModel>

namespace DataManager
{
    class CDashboardDataStationList;
}

namespace Programs {
class CModifyProgramDlg;
/****************************************************************************/
/**
 * \brief This is the data model for the program step tables
 */
/****************************************************************************/
class CStepModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CStepModel(QObject *p_Parent = NULL);
    void SetProgram(DataManager::CProgram *p_Program, DataManager::CUserSettings *p_UserSettings, DataManager::CDataReagentGroupList *p_ReagentGroupList, DataManager::CDataReagentList *p_ReagentList,
                    qint32 Columns);
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;
    Qt::ItemFlags flags(const QModelIndex &Index) const;
    void SetIndex(QModelIndex Index);
    void SetModifyProgramDlgPtr(CModifyProgramDlg *p_ModifyProgram);
    void SetVisibleRowCount(int RowCount);
    qint32 GetIndex();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ShowStation
     *
     *  \param bSet = bool type parameter
     *  \param strStation = the translated word for "Station"
     *  \param strTemprature = the translated word for "Temp."
     *  \return from ShowStation
     */
    /****************************************************************************/
    void ShowStation(bool bSet,const QString& strStation,
                     const QString& strTemprature);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetStationNameList
     *
     *  \return from SetStationNameList
     */
    /****************************************************************************/
    void SetStationNameList(QList<QString>&);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetCurSelectRowIndex
     *
     *  \param index = int type parameter
     *
     *  \return from SetCurSelectRowIndex
     */
    /****************************************************************************/
    void SetCurSelectRowIndex(int index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ShowPV
     *
     *  \param Step = DataManager::CProgramStep type parameter
     *
     *  \return from ShowPV
     */
    /****************************************************************************/
    QString ShowPV(DataManager::CProgramStep *Step) const;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ShowTemperature
     *
     *  \param Step = DataManager::CProgramStep type parameter
     *
     *  \return from ShowTemperature
     */
    /****************************************************************************/
    QVariant ShowTemperature(DataManager::CProgramStep *Step) const;
private:
    DataManager::CProgram *mp_Program;                  //!< Program displayed in the table
    DataManager::CDataReagentList *mp_ReagentList;      //!< Global reagent list
    qint32 m_Columns;                                   //!< Number of table columns
    qint32 m_CurrentRow;                                //!< Current row selected
    CModifyProgramDlg *mp_ModifyProgramDlg;             //!< View pointer
    qint32 m_VisibleRowCount;                           //!< Number of rows visible in the table
    DataManager::CDataReagentGroupList *mp_ReagentGroupList; //!< Reagent group list
    QStringList m_ReagentID;       ///<  Definition/Declaration of variable m_ReagentID
    DataManager::CUserSettings *mp_UserSettings;    //!< Data object
    QList<QString> mp_DashboardStationNameList;       ///<  Definition/Declaration of variable mp_DashboardStationNameList
    bool m_IsShowStation;       ///<  Definition/Declaration of variable m_IsShowStation
    int m_CurSelectRowIndex;       ///<  Definition/Declaration of variable m_CurSelectRowIndex
    QString m_strStation, m_strTemp;
};

} // end namespace Programs

#endif // PROGRAMS_STEPMODEL_H

