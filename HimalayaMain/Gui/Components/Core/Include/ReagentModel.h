/****************************************************************************/
/*! \file ReagentModel.h
 *
 *  \brief Definition file for class CReagentModel.
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

#ifndef CORE_REAGENTMODEL_H
#define CORE_REAGENTMODEL_H

#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "Global/Include/GlobalDefines.h"
#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>
#include <QObject>
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
class CReagentModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CReagentModel(QObject *p_Parent = NULL);
    void SetReagentList(DataManager::CDataReagentList *p_ReagentList, qint32 Columns);
    void SetReagentGroupList(DataManager::CDataReagentGroupList *p_ReagentGroupList);
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;  //data Reagent SubMenu
    QVariant dataStatus(const QModelIndex &Index, int Role) const; //data Reagent Staus SubMenu
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;
    Qt::ItemFlags flags(const QModelIndex &Index) const;
    QModelIndex CreateIndex(int Row, int Column);

    void ResetAndUpdateModel();
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
     * \brief Returns the position of Reagent to be selected
     * \iparam ReagentName
     * \return m_ReagentNames = Position of Reagent in the table
     */
    /****************************************************************************/
    Global::RMSOptions_t GetRMSOption(){
        return m_RMSOptions;
    }
    /****************************************************************************/
    /**
     * \brief Sets the currently selected reagent in the table
     * \iparam ReagentName
     */
    /****************************************************************************/
    void SetRMSOption(Global::RMSOptions_t Value){
        m_RMSOptions = Value;
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
    QString GetReagentLongName(int Row);
    bool ContainsReagent(QString ReagentID);
    bool IsReagentPresentInBL(QString ReagentID);
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);

public slots:
    void UpdateReagentList();
    void OnDeviceModeChanged(QString DeviceMode);

private:
    QString ConvertClassString(QString ClassNumber) const;
    DataManager::CDataReagentList *mp_ReagentList;      //!< Reagent list
    DataManager::CDataReagentGroupList *mp_ReagentGroupList; //!< Reagent group list
    QMap<QString, QString> m_Identifiers;               //!< Identifiers of reagents currently displayed
    QStringList m_ReagentNames;                         //!< Long names of reagents currently displayed
    QStringList m_ReagentNamesCache;                    //!< Back up of Reagent names
    QStringList m_ReagentsInStation;                    //!< Reagents present in Stations
    QStringList m_SpecialReagentList;                   //!< List of special reagents
    qint32 m_Columns;                                   //!< Number of table columns
    QString m_CurrentReagentName;                       //!< Current Reagent Name
    Programs::CModifyProgramStepDlg *mp_Parent;         //!< Reference to ModifyProgramStep Dialog
    bool m_HideSpecialReagents;                         //!< Hides the special reagents if true.
    bool m_FilterLeicaReagent;                          //!< Filter Leica Reagent if set to true
    qint32 m_VisibleRowCount;                           //!< Number of rows visible in the table
    QMap<QString, QString> m_ReagentNameMap;            //!< Sorts Reagent Longnames
    QMap<QString, QString> m_SpecialReagentNameMap;     //!< Sorts Special Reagents
    bool m_BLFlag;                                      //!< Flag for reagents in BathLayout
    QStringList m_VisibleReagentIds;                    //!< Reagent Ids list
    QPixmap m_PixmapTickOk;                             //!< Pixmap for TickOk png
    QString m_DeviceMode;                               //!< Device mode
    DataManager::CUserSettings m_UserSettings;          //!< UserSettings object
    Global::RMSOptions_t m_RMSOptions;
};

} // end namespace Core

#endif // CORE_REAGENTMODEL_H
