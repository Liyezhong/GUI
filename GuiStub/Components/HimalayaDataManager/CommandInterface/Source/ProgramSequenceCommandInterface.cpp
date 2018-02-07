/****************************************************************************/
/*! \file ProgramCommandInterface.cpp
 *
 *  \brief Program Command Interface  Implementation
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
#include "../../HimalayaDataManager/CommandInterface/Include/ProgramSequenceCommandInterface.h"



namespace DataManager {
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataManager = Datamanager object
 *  \iparam p_MasterThreadController = Master thread Cntrl
 *  \iparam p_DataContainer = DataContainer pointer
 */
/****************************************************************************/
CProgramSequenceCommandInterface::CProgramSequenceCommandInterface(CDataManager *p_DataManager,
                                                   Threads::MasterThreadController *p_MasterThreadController,
                                                   CDataContainer *p_DataContainer):
    CCommandInterfaceBase(p_DataManager, p_MasterThreadController, p_DataContainer)
{
    RegisterCommands();
}

/****************************************************************************/
/**
 * \brief Register Commands related to Program Container
 */
/****************************************************************************/
void CProgramSequenceCommandInterface::RegisterCommands() {

}

}// End of namespace DataManager



