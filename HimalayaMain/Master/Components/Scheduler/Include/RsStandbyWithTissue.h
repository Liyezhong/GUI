/****************************************************************************/
/*! \file CRsStandbyWithTissue.h
 *
 *  \brief CRsStandby class definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 22.04.2014
 *   $Author:  $ Songtao Yu
 *
 *  \b Company:
 *
 *       Leica Biosystems SH CN.
 *
 *  (C) Copyright 2014 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef RSSTANDBY_WITH_TISSUE_H
#define RSSTANDBY_WITH_TISSUE_H
#include "ErrorHandlingSMBase.h"
#include <QStateMachine>

namespace Scheduler{

/****************************************************************************/
/*!
 * \brief Error Handling State machine for RS_Standby_WithTissue
 */
/****************************************************************************/
class  CRsStandbyWithTissue : public CErrorHandlingSMBase
{
    Q_OBJECT
public:
    CRsStandbyWithTissue (QStateMachine* pStateMachine, QState* pParentState);
    ~CRsStandbyWithTissue();
    SchedulerStateMachine_t GetCurrentState(QSet<QAbstractState*> statesList);

signals:
    void ReleasePressure();
    void ShutdownFailedHeater();
    void ShutdownFailedHeaterFinished();
    void RTBottomStopTempCtrl();
    void RTTopStopTempCtrl();
    void OnReleasePressure();
    void OnShutdownFailedHeater();
    void OnRTBottomStopTempCtrl();
    void OnRTTopStopTempCtrl();

private:
    QState *mp_ReleasePressure;
    QState *mp_ShutdownFailedHeater;
    QState *mp_RTBottomStopTempCtrl;
    QState *mp_RTTopStopTempCtrl;
};
}
#endif // RSSTANDBY_WITH_TISSUE_H