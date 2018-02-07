/****************************************************************************/
/*! \file LanguageModel.cpp
 *
 *  \brief LanguageModel implementation.
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
#ifndef LANGUAGEMODEL_H
#define LANGUAGEMODEL_H

#include "MainMenu/Include/TableMoving.h"
#include <QAbstractTableModel>

namespace Settings {

/****************************************************************************/
/**
 * \brief This is the data model for the program tables
 */
/****************************************************************************/
class CLanguageModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CLanguageModel(QObject *p_Parent = NULL);
    //void SetLanguageList(QStringList p_LanguageList, qint32 Columns);
    int rowCount(const QModelIndex &p_parent) const;
    int columnCount(const QModelIndex &p_parent) const;
    QVariant data(const QModelIndex &Index, int Role) const;
    Qt::ItemFlags flags(const QModelIndex &Index) const;
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role = (int)Qt::DisplayRole) const;
   // bool setData(const QModelIndex &Index, const QVariant &Value, int Role = Qt::EditRole);
    void SetVisibleRowCount(qint32 RowCount);
    void SetLanguageList(QStringList LanguageList);

private:
    QStringList m_LanguageList;   //!< Language list
    qint32 m_Columns;             //!< Number of table columns
    qint32 m_VisibleRowCount;     //!< Number of rows visible in the table
};

} // end namespace Settings

#endif // SETTINGS_LANGUAGEWIDGET_H

