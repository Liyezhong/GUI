/****************************************************************************/
/*! \file ReagentGroupModel.h
 *
 *  \brief Definition file for class CReagentGroupModel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-02-11
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

#ifndef CORE_ReagentGroupModel_H
#define CORE_ReagentGroupModel_H

#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColorList.h"
#include <QAbstractTableModel>

namespace Core {


/****************************************************************************/
/**
 * \brief This is the data model for the reagent tables
 */
/****************************************************************************/
class CReagentGroupModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CReagentGroupModel(QObject *p_Parent = NULL);
    void SetReagentGroupList(DataManager::CDataReagentGroupList *p_ReagentGroupList, qint32 Columns, bool bShowCleaningReagent = true);
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;  //data Reagent SubMenu
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function dataStatus
     *
     *  \param Index =  QModelIndex type parameter
     *  \param Role =  int type parameter
     *
     *  \return from dataStatus
     */
    /****************************************************************************/
    QVariant dataStatus(const QModelIndex &Index, int Role) const; //data Reagent Staus SubMenu

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
        return m_ReagentGroupNames.indexOf(ReagentName, 0);
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
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetReagentLongName
     *
     *  \param Row = int type parameter
     *
     *  \return from GetReagentLongName
     */
    /****************************************************************************/
    QString GetReagentLongName(int Row);
    bool ContainsReagent(QString ReagentID);
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateReagentGroupList
     */
    /****************************************************************************/
    void UpdateReagentGroupList();
private:

    DataManager::CDataReagentGroupList *mp_ReagentGroupList; //!< Reagent group list
    QMap<QString, QString> m_Identifiers;               //!< Identifiers of reagents currently displayed
    QStringList m_ReagentGroupNames;                         //!< Long names of reagents currently displayed
    qint32 m_Columns;                                   //!< Number of table columns
    QString m_CurrentReagentName;                       //!< Current Reagent Name
    qint32 m_VisibleRowCount;                           //!< Number of rows visible in the table
    QStringList m_VisibleReagentIds;                    //!< Reagent Ids list
    DataManager::CUserSettings m_UserSettings;          //!< UserSettings object
    bool m_ShowCleaningReagent;                         //!< Sometimes CleaningReagent group should exclude
};

} // end namespace Core

#endif // CORE_ReagentGroupModel_H

