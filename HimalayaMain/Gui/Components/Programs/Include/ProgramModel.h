/****************************************************************************/
/*! \file ProgramModel.h
 *
 *  \brief Include file for class CProgramModel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-25
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

#ifndef PROGRAMS_PROGRAMMODEL_H
#define PROGRAMS_PROGRAMMODEL_H

#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "MainMenu/Include/TableMoving.h"
#include <QAbstractTableModel>
#include "MainMenu/Include/MainWindow.h"
#include <QMap>
#include <QStringList>
#include <QStyleOption>
#include <QStandardItem>

namespace Programs {

/****************************************************************************/
/**
 * \brief This is the data model for the program tables
 */
/****************************************************************************/
class CProgramModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CProgramModel(QObject *p_Parent = NULL);
    void SetProgramList(DataManager::CDataProgramList *p_ProgramList, qint32 Columns);
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;
    Qt::ItemFlags flags(const QModelIndex &Index) const;
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;
    bool setData(const QModelIndex &Index, const QVariant &Value, int Role);
    void SetVisibleRowCount(int RowCount);

    void SetColumnNumber(qint32 value) {  m_Columns = value; }
    void ResetandUpdateModel();
    void SetUserRole(MainMenu::CMainWindow::UserRole_t UserRole);

    void SelectedRowIndex(int Index);
private:
    DataManager::CDataProgramList *mp_ProgramList;   //!< Program list
    qint32 m_Columns;                               //!< Number of table columns
    qint32 m_VisibleRowCount;                       //!< Number of rows visible in the table
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    int m_CurrentIndex;
    QStringList m_ProgramID;

private slots:
    void OnUpdateProgramList();

};

} // end namespace Programs

#endif // PROGRAMS_PROGRAMMODEL_H
