/****************************************************************************/
/*! \file RcReport.h
 *
 *  \brief CRcReport class definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.01.2014
 *   $Author:  $ Quan Zhang
 *
 *  \b Company:
 *
 *       Leica Biosystems SH CN.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef RCREPORT_H
#define RCREPORT_H

#include "ErrorHandlingSMBase.h"
#include <QStateMachine>

namespace Scheduler{

/****************************************************************************/
/*!
 * \brief Error Handling State Machine for RC_Report
 */
/****************************************************************************/
class  CRcReport : public CErrorHandlingSMBase
{
    Q_OBJECT
public:
    CRcReport (QStateMachine* pStateMachine, QState* pParentState);
    ~CRcReport();
    SchedulerStateMachine_t GetCurrentState(QSet<QAbstractState*> statesList);

signals:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RcReport
     */
    /****************************************************************************/
    void RcReport();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal OnRcReport
     */
    /****************************************************************************/
    void OnRcReport();

private:
    QState *mp_RcReport;       ///<  Definition/Declaration of variable mp_RcReport
};
}


#endif // RCREPORT_H