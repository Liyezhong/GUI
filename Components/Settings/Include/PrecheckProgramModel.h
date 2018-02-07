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
#ifndef PRECHECK_PROGRAM_MODEL_H
#define PRECHECK_PROGRAM_MODEL_H

#include "MainMenu/Include/TableMoving.h"
#include <QAbstractTableModel>
#include "MainMenu/Include/MainWindow.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"

namespace Settings {

/****************************************************************************/
/**
 * \brief This is the data model for the program tables
 */
/****************************************************************************/
class CPrecheckProgramModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CPrecheckProgramModel(QObject *p_Parent = NULL);
    void SetProgramList(DataManager::CDataProgramList *p_ProgramList);
    int rowCount(const QModelIndex &p_parent) const;
    int columnCount(const QModelIndex &p_parent) const;
    QVariant data(const QModelIndex &Index, int Role) const;
    Qt::ItemFlags flags(const QModelIndex &Index) const;
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role = (int)Qt::DisplayRole) const;
    void SetVisibleRowCount(qint32 RowCount);
    /****************************************************************************/
    /*!
     *  \brief Sets the User Role to Current User Role
     *  \param UserRole = the current user role
     */
    /****************************************************************************/
    void SetUserRole(MainMenu::CMainWindow::UserRole_t UserRole);
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QStringList GetBottleCheckFlags();
private slots:
    /****************************************************************************/
    /*!
    *  \brief  Definition/Declaration of OnUpdateProgramList
    */
    /****************************************************************************/
    void OnUpdateProgramList();
private:
    DataManager::CDataProgramList m_ProgramList;  //!< Container for programs
    qint32 m_Columns;             //!< Number of table columns
    qint32 m_VisibleRowCount;     //!< Number of rows visible in the table
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role

};

} // end namespace Settings

#endif // PRECHECK_PROGRAM_MODEL_H

