/****************************************************************************/
/*! \file ReagentGroupCommandInterfaceBase.h
 *
 *  \brief Reagent Command Interface definition
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-22
 *  $Author:    $ Swati Tiwari
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
#ifndef ReagentGroupCommandInterface_H
#define ReagentGroupCommandInterface_H

//#include "CommandInterfaceBase.h"

#include "TestStubDataManager/CommandInterface/Include/CommandInterfaceBase.h"
#include "../../HimalayaDataManager/Include/DataManager.h"
#include "HimalayaDataContainer/Containers/StainingReagentGroups/Commands/Include/CmdReagentGroupUpdate.h"
#include "NetCommands/Include/CmdConfigurationFile.h"

namespace DataManager {
class CDataReagentGroupList;
class CReagentGroup;
/****************************************************************************/
/**
 * \brief  Reagent container command interface definition
 */
/****************************************************************************/
class CReagentGroupCommandInterface:public CCommandInterfaceBase
{
    Q_OBJECT
public:
    CReagentGroupCommandInterface(CDataManager *p_DataManager, Threads::MasterThreadController *p_MasterThreadController, CDataContainer *p_DataContainer);

private:
    void RegisterCommands();
    void UpdateReagentGroup(Global::tRefType, const MsgClasses::CmdReagentGroupUpdate &Cmd, Threads::CommandChannel &AckCommandChannel);

};

}// end of namespace DataManager

#endif // ReagentGroupCommandInterface_H
