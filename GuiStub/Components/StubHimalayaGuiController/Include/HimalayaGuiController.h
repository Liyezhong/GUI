/****************************************************************************/
/*! \file HimalayaGuiController.h
 *
 *  \brief Definition file for class HimalayaGuiController.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.12.2010
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

#ifndef HIMALAYAGUI_HIMALAYAGUICONTROLLER_H
#define HIMALAYAGUI_HIMALAYAGUICONTROLLER_H

#include <StubExternalProcessController/Include/ExternalProcessController.h>
#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataExport.h"
#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataImport.h"



namespace HimalayaGui {

/// internal name of the external process
const QString GUI_PROCESS_NAME = "Himalaya Device GUI";

/****************************************************************************/
/**
 * \brief This is a Himalaya specific Gui Controller. It handles communication
 *        with Himalaya GUI process.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class HimalayaGuiController : public ExternalProcessControl::ExternalProcessController
{
    Q_OBJECT

private:

    HimalayaGuiController();                                                    ///< Not implemented.
    HimalayaGuiController(const HimalayaGuiController &);                       ///< Not implemented.
    const HimalayaGuiController & operator = (const HimalayaGuiController &);   ///< Not implemented.

    void RegisterThreadAcksAndTimeouts();

protected:

    virtual void OnGoReceived();
    virtual void OnStopReceived();
    virtual void OnPowerFail();
    virtual void OnReadyToWork();
    virtual void OnStopWorking();


public:

    HimalayaGuiController(Global::gSourceType TheHeartBeatSource);
    virtual ~HimalayaGuiController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

    virtual void OnCmdTimeout(Global::tRefType Ref, const QString &CmdName);
private:

    /// Network Device for communication with GUI
    ExternalProcessControl::ExternalProcessDevice   *m_myGuiDevice;

signals:
    void SendConfigurationFiles();
};

} // end namespace HimalayaGui

#endif
