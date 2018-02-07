
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
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Settings/Include/LanguageModel.h"
#include <QApplication>
#include <QEvent>


namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CLanguageModel::CLanguageModel(QObject *p_Parent):
            QAbstractTableModel(p_Parent)
{
    m_VisibleRowCount = 8;
    m_Columns = 1;
}

/****************************************************************************/
/*!
 *  \brief Sets the Langauge list
 *
 *  \iparam LanguageList
 */
/****************************************************************************/
void CLanguageModel::SetLanguageList(QStringList LanguageList)
{
    beginResetModel();
    m_LanguageList = LanguageList;
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount
 */
/****************************************************************************/
void CLanguageModel::SetVisibleRowCount(qint32 RowCount)
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
int CLanguageModel::rowCount(const QModelIndex &p_parent) const
{
    Q_UNUSED(p_parent)
    int Count = m_LanguageList.count();
    return ((Count < m_VisibleRowCount) ? m_VisibleRowCount : Count);
}

/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *  \iparam p_parent
 *  \return Columns count
 */
/****************************************************************************/
int CLanguageModel::columnCount(const QModelIndex &p_parent) const
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
QVariant CLanguageModel::data(const QModelIndex &Index, int Role) const
{
    if (!(Index.row() >= m_LanguageList.count())) {
        if (m_LanguageList.at(Index.row()) != "") {
            if (Role == (int)Qt::DisplayRole) {
                return m_LanguageList.at(Index.row());
            }
        }
    }
    else if (Role == (int)Qt::BackgroundRole) {
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
Qt::ItemFlags CLanguageModel::flags(const QModelIndex &Index) const
{
    if (!Index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    if (Index.row() >= m_LanguageList.count()) {
        return Qt::NoItemFlags;
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
QVariant CLanguageModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    Q_UNUSED(Section)
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {        
        return tr("Language");
    }
    return QVariant();
}

} // end namespace Programs

