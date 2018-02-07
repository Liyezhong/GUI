/****************************************************************************/
/*! \file DashboardDataTypes.h
 *
 *  \brief DashboardDataTypes definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 27-Mar-2013
 *   $Author:  $ Santhosha Kumar Raju
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef DASHBOARD_DASHBOARDDATATYPES_H
#define DASHBOARD_DASHBOARDDATATYPES_H

#include <QPoint>

namespace Dashboard {

//QPoint FrameStartPosPt;
//QPoint RetortStartOffsetPt;
//QPoint ParaffinbathStartOffsetPt;
//QPoint Row1BottleStartOffsetPt;
//QPoint Row2BottleStartOffsetPt;

//qint8 BottleImageWidth;
//qint8 BottleImageHeight;
//qint8 BottleBoundingRectWidth;
//qint8 BottleBoundingRectHeight;
//qint8 ParaffinbathImageWidth;
//qint8 ParaffinbathImageHeight;
//qint8 ParaffinbathBoundingRectWidth;
//qint8 ParaffinbathBoundingRectHeight;
//qint8 RetortImageWidth;
//qint8 RetortImageHeight;
//qint8 RetortBoundingRectWidth;
//qint8 RetortBoundingRectHeight;

const int firstButton = 1;       ///<  Definition/Declaration of variable firstButton
const int secondButton = 2;       ///<  Definition/Declaration of variable secondButton
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class Dashboard::CProgramRunningPanelWidget
 */
/****************************************************************************/
typedef enum
{
    OP_NONE = -1,
    OP_ABORT,
    OP_PAUSE,
    OP_PLAY
} DashboardOperation_t;

}


#endif // DASHBOARD_DASHBOARDDATATYPES_H

