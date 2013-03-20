/****************************************************************************/
/*! \file CmdDeleteItem.cpp
 *
 *  \brief CmdDeleteItem command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2010
 *   $Author:  $ Y.Novak
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

#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdDeleteItem.h"

namespace MsgClasses {

QString CmdDeleteItem::NAME = "MsgClasses::CmdDeleteItem";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   ItemName    Item name.
 * \param[in]   ItemId      Item ID.
 */
/****************************************************************************/
CmdDeleteItem::CmdDeleteItem(int Timeout, const QString &ItemId) :
    Command(Timeout),
    m_ItemId(ItemId)
{
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdDeleteItem::CmdDeleteItem() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDeleteItem::~CmdDeleteItem()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDeleteItem::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns ID of item to be deleted
 *
 *  \return  item ID (program, macro, etc)
 */
/****************************************************************************/
QString CmdDeleteItem::GetItemId() const
{
    return m_ItemId;
}

} // end namespace MsgClasses
