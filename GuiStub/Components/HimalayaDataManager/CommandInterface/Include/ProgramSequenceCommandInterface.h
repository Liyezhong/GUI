/****************************************************************************/
/*! \file ProgramCommandInterface.h
 *
 *  \brief Program Command Interface definition
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-30
 *  $Author:    $ N.Kamath
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
#ifndef PROGRAMSEQUENCECOMMANDINTERFACE_H
#define PROGRAMSEQUENCECOMMANDINTERFACE_H
#include "TestStubDataManager/CommandInterface/Include/CommandInterfaceBase.h"

#include "NetCommands/Include/CmdConfigurationFile.h"
#include "../../HimalayaDataManager/Include/DataManager.h"
namespace DataManager {
/****************************************************************************/
/**
 * \brief  Program container command interface definition
 */
/****************************************************************************/
class CProgramSequenceCommandInterface:public CCommandInterfaceBase
{
public:
    CProgramSequenceCommandInterface(CDataManager *p_DataManager, Threads::MasterThreadController *p_MasterThreadController, CDataContainer *p_DataContainer);

private:
    void RegisterCommands();
};

}// end of namespace DataManager
#endif // PROGRAMSEQUENCECOMMANDINTERFACE_H
