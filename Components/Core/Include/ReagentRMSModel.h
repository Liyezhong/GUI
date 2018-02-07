/****************************************************************************/
/*! \file ReagentRMSModel.h
 *
 *  \brief Definition file for class CReagentRMSModel.
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

#ifndef CORE_REAGENTRMSMODEL_H
#define CORE_REAGENTRMSMODEL_H

#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "Global/Include/GlobalDefines.h"
#include <QAbstractTableModel>
#include <QPixmap>
namespace Programs {
    class CModifyProgramStepDlg;
}
namespace Core {


/****************************************************************************/
/**
 * \brief This is the data model for the reagent tables
 */
/****************************************************************************/
class CReagentRMSModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CReagentRMSModel(QObject *p_Parent = NULL);
    void SetReagentList(DataManager::CDataReagentList *p_ReagentList, qint32 Columns);
    void SetReagentGroupList(DataManager::CDataReagentGroupList *p_ReagentGroupList);
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
    QModelIndex CreateIndex(int Row, int Column);

    /****************************************************************************/
    /**
     * \brief Returns the position of Reagent to be selected
     *
     * \iparam ReagentName
     * \return m_ReagentNames = Position of Reagent in the table
     */
    /****************************************************************************/
    int GetReagentPosition(QString ReagentName){
        return m_ReagentNames.indexOf(ReagentName, 0);
    }

    /****************************************************************************/
    /**
     * \brief Sets the Reagent GroupType
     * \iparam ReagentGroupType
     */
    /****************************************************************************/
    void SetReagentGroupType(bool ReagentGroupType) {
        m_CleaningReagent = ReagentGroupType;
    }

    /****************************************************************************/
    /**
     * \brief Sets the currently selected reagent in the table
     *
     * \iparam ReagentName
     */
    /****************************************************************************/
    void SetCurrentReagent(QString ReagentName){
        m_CurrentReagentName = ReagentName;
    }

    /****************************************************************************/
    /**
     * \brief Sets the parent pointer
     *
     * \iparam p_Parent = Reference to parent widget
     */
    /****************************************************************************/
    void SetParentPtr(Programs::CModifyProgramStepDlg *p_Parent){ mp_Parent = p_Parent; }

    /****************************************************************************/
    /**
     * \brief Function to hide the special reagents in the table
     *
     * \iparam Hide = True hide special reagents else show.
     */
    /****************************************************************************/
    void HideSpecialReagents(bool Hide) { m_HideSpecialReagents = Hide; }

    /****************************************************************************/
    /**
     * \brief Function to hide the Leica reagents in the table.
     *        Note - Call this function before setting Reagent list.
     *
     * \iparam Filter = if true filter Leica reagents else show.
     */
    /****************************************************************************/
    void FilterLeicaReagents(bool Filter) { m_FilterLeicaReagent = Filter; }

    void SetVisibleRowCount(int RowCount);
    QString GetReagentID(const QString ReagentName);
    QString GetReagentID(int Index);
    QString GetReagentLongName(int Row);
    bool ContainsReagent(QString ReagentID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsReagentPresentInBL
     *
     *  \param ReagentID = QString type parameter
     *
     *  \return from IsReagentPresentInBL
     */
    /****************************************************************************/
    bool IsReagentPresentInBL(QString ReagentID);
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateList
     *
     *  \param p_Reagent =  DataManager::CReagent type parameter
     *
     *  \return from UpdateList
     */
    /****************************************************************************/
    void UpdateList( DataManager::CReagent *p_Reagent);

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateReagentList
     */
    /****************************************************************************/
    void UpdateReagentList();

private:
    DataManager::CDataReagentList *mp_ReagentList;      //!< Reagent list
    DataManager::CDataReagentGroupList *mp_ReagentGroupList; //!< Reagent group list
    QMap<QString, QString> m_Identifiers;               //!< Identifiers of reagents currently displayed
    QStringList m_ReagentNames;                         //!< Long names of reagents currently displayed
    QStringList m_ReagentID;       ///<  Definition/Declaration of variable m_ReagentID
    QStringList m_CleaningGroupReagentNames;                         //!< Long names of reagents currently displayed
    QStringList m_CleaningGroupReagentID;       ///<  Definition/Declaration of variable m_CleaningGroupReagentID
    qint32 m_Columns;                                   //!< Number of table columns
    QString m_CurrentReagentName;                       //!< Current Reagent Name
    Programs::CModifyProgramStepDlg *mp_Parent;         //!< Reference to ModifyProgramStep Dialog
    bool m_HideSpecialReagents;                         //!< Hides the special reagents if true.
    bool m_FilterLeicaReagent;                          //!< Filter Leica Reagent if set to true
    qint32 m_VisibleRowCount;                           //!< Number of rows visible in the table
    QStringList m_VisibleReagentIds;                    //!< Reagent Ids list
    QPixmap m_PixmapTickOk;                             //!< Pixmap for TickOk png
    DataManager::CUserSettings m_UserSettings;          //!< UserSettings object
    bool m_CleaningReagent;       ///<  Definition/Declaration of variable m_CleaningReagent
};

} // end namespace Core

#endif // CORE_ReagentRMSModel_H

