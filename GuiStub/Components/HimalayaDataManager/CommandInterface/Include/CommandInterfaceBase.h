/****************************************************************************/
/*! \file CommandInterfaceBase.h
 *
 *  \brief Command Interface definition
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
#ifndef COMMANDINTERFACEBASE_H
#define COMMANDINTERFACEBASE_H

#include <QObject>
#include "../../../TestHimalayaMasterThread/Include/TestHimalayaMasterThreadController.h"
#include "Threads/Include/CommandChannel.h"

namespace DataManager {
/****************************************************************************/
/**
 * \brief Base class for all DataManager Command Interfaces
 */
/****************************************************************************/
class CCommandInterfaceBase : public QObject
{
    Q_OBJECT
public:
    /****************************************************************************/
    /*!
     *  \brief Constructor
     *  \iparam p_DataManager = Datamanager object
     *  \iparam p_HimalayaMasterThreadController = Himalaya Master thread Cntrl
     *  \iparam p_DataContainer = DataContainer pointer
     */
    /****************************************************************************/
    explicit CCommandInterfaceBase(CDataManager *p_DataManager,
                                   Himalaya::HimalayaMasterThreadController *p_HimalayaMasterThreadController,
                                   CDataContainer *p_DataContainer):
        QObject(0),
        mp_DataManager(p_DataManager),
        mp_HimalayaMasterThreadController(p_HimalayaMasterThreadController),
        mp_DataContainer(p_DataContainer)
    {
    }
    /****************************************************************************/
    /*!
     *  \brief Destructor
     */
    /****************************************************************************/
    virtual ~CCommandInterfaceBase(){}

protected:
    //Pointer section
    CDataManager                             *mp_DataManager;                     //!< Self Explaining
    Himalaya::HimalayaMasterThreadController *mp_HimalayaMasterThreadController; //!< Will be used to call BroadCastCommand
    CDataContainer                           *mp_DataContainer;                  //!< Provides access to various containers
    virtual void RegisterCommands() = 0;
    virtual void UnRegisterCommands(){}
};

}// end of namespace DataManager

#endif // COMMANDINTERFACEBASE_H
