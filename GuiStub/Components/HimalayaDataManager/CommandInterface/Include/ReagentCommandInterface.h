/****************************************************************************/
/*! \file ReagentCommandInterfaceBase.h
 *
 *  \brief Reagent Command Interface definition
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-10-16
 *  $Author:    $ Bhanu Prasad
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
#ifndef REAGENTCOMMANDINTERFACE_H
#define REAGENTCOMMANDINTERFACE_H

#include "TestStubDataManager/CommandInterface/Include/CommandInterfaceBase.h"
#include "../../HimalayaDataManager/Include/DataManager.h"
#include "HimalayaDataContainer/Containers/StainingReagents/Commands/Include/CmdReagentAdd.h"
#include "HimalayaDataContainer/Containers/StainingReagents/Commands/Include/CmdReagentRemove.h"
#include "HimalayaDataContainer/Containers/StainingReagents/Commands/Include/CmdReagentUpdate.h"
#include "NetCommands/Include/CmdConfigurationFile.h"

namespace DataManager {
class CDataReagentList;
class CReagent;
/****************************************************************************/
/**
 * \brief  Reagent container command interface definition
 */
/****************************************************************************/
class CReagentCommandInterface:public CCommandInterfaceBase
{
    Q_OBJECT
public:
    CReagentCommandInterface(CDataManager *p_DataManager, Threads::MasterThreadController *p_MasterThreadController, CDataContainer *p_DataContainer);

private:
    void RegisterCommands();
    void UpdateReagent(Global::tRefType, const MsgClasses::CmdReagentUpdate &Cmd, Threads::CommandChannel &AckCommandChannel);
    void AddReagent(Global::tRefType, const MsgClasses::CmdReagentAdd &Cmd, Threads::CommandChannel &AckCommandChannel);
    void DeleteReagent(Global::tRefType, const MsgClasses::CmdReagentRemove &Cmd, Threads::CommandChannel &AckCommandChannel);

private slots:
    void UpdateReagent(CReagent &Reagent);
    void UpdateReagentContainer(CDataReagentList &ReagentList);
};

}// end of namespace DataManager

#endif // REAGENTCOMMANDINTERFACE_H
