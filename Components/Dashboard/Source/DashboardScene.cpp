/****************************************************************************/
/*! \file DashboardScene.cpp
 *
 *  \brief DashboardScene implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17-Jan-2013
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

#include "Dashboard/Include/DashboardScene.h"
#include "Core/Include/DataConnector.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include <QGraphicsProxyWidget>

namespace Dashboard {

const int PipeWidth = 5;       ///<  Definition/Declaration of variable PipeWidth
const int JointHeight = 10;

//!< Timing interval for the water animation in milliseconds
#define PIPE_ANIMATION_INTERVAL 100     ///<  Definition/Declaration of macro ANIMATION_INTERVAL
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_DataConnector = Global data container
 *  \iparam p_Parent = Parent object
 *  \param  p_MainWindow
 */
/****************************************************************************/
CDashboardScene::CDashboardScene(Core::CDataConnector *p_DataConnector,
                       QObject *p_Parent,
                       MainMenu::CMainWindow *p_MainWindow) :
                       QGraphicsScene(p_Parent),
                       m_CurTabIndex(0),
                       mp_DataConnector(p_DataConnector),                       
                       mp_MainWindow(p_MainWindow),
                       m_bProcessRunning(false),
                       m_pPipeAnimationTimer(NULL),
                       m_pBlinkingIntervalTimer(NULL),
                       m_pStartBlinkingTimer(NULL),
                       m_UsedPipeGraphicsRectItem(NULL),
                       m_currentTimerOrder(0),
                       m_SuckDrainStationId("")

{
    m_pPipeAnimationTimer = new QTimer(this);
    CONNECTSIGNALSLOT(m_pPipeAnimationTimer, timeout(), this, PipeSuckDrainAnimation());

    m_pBlinkingIntervalTimer = new QTimer(this);
    m_pBlinkingIntervalTimer->setInterval(1200);
    CONNECTSIGNALSLOT(m_pBlinkingIntervalTimer, timeout(), this, BlinkingStation());

    m_pStartBlinkingTimer = new QTimer(this);
    m_pStartBlinkingTimer->setInterval(6000);
    CONNECTSIGNALSLOT(m_pStartBlinkingTimer, timeout(), this, OnAppIdle());
    m_pStartBlinkingTimer->start();
    m_DashboardStationList.clear();

    InitDashboardStationItemsPositions();
    InitDashboardStationGroups();
    InitDashboardStationIDs();
    InitDashboardStationLabels();
    InitStationConnectedPipeList();
    AddDashboardStationItemsToScene();

    (void)m_PixMapFlowFrame1.load(":/HimalayaImages/Icons/Dashboard/Pipe/flow1.png");
    (void)m_PixMapFlowFrame2.load(":/HimalayaImages/Icons/Dashboard/Pipe/flow2.png");
    (void)m_PixMapFlowFrame3.load(":/HimalayaImages/Icons/Dashboard/Pipe/flow3.png");

    for (int r = 0; r < 3; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            m_BrushArray[r][c] = NULL;
        }
    }

    // update the stations whenenver the stations.xml is sent
    CONNECTSIGNALSLOT(mp_DataConnector, DashboardStationsUpdated(), this, UpdateDashboardStations());
    CONNECTSIGNALSLOT(mp_DataConnector, UserSettingsUpdated(), this, OnUpdateUserSettings());
    CONNECTSIGNALSLOT(mp_DataConnector, DashboardStationChangeReagent(const QString&), this, UpdateDashboardStation(const QString&));
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, CurrentTabChanged(int), this, OnCurrentTabChanged(int));

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDashboardScene::~CDashboardScene()
{
    try {
        delete mp_DashboardStationRetort;
        for (int i = 0; i < mp_DashboardStationItems.count(); i++)
        {
            delete mp_DashboardStationItems.at(i);
        }

        for (int i = 0; i < m_pGraphicsPathItemPipeList.count(); i++)
        {
            delete m_pGraphicsPathItemPipeList.at(i);
        }
        delete m_WholePipeGraphicsRectItem;
        delete m_UsedPipeGraphicsRectItem;
        delete m_pPipeAnimationTimer;
        delete m_pBlinkingIntervalTimer;
        delete m_pStartBlinkingTimer;
    } catch(...) {
        // Please the PC-Lint
    }
}

/****************************************************************************/
/*!
 *  \brief Initialize Dashboard Station Item Positions on the Screen
 */
/****************************************************************************/
void CDashboardScene::InitDashboardStationItemsPositions()
{

    m_DashboardStationItemPositions << QPoint(15, 227) << QPoint(75, 227) << QPoint(135, 227) << QPoint(195, 227) << QPoint(255, 227) << QPoint(315, 227) << QPoint(375, 227)
                                    << QPoint(15, 385) << QPoint(75, 385) << QPoint(135, 385) << QPoint(195, 385) << QPoint(255, 385) << QPoint(315, 385)
                                    << QPoint(15, 144) << QPoint(135, 144) << QPoint(255, 144)
                                    << QPoint(15, 39);


}

void CDashboardScene::InitStationConnectedPipeList()
{
    m_PipeListP1 << "P1LeftEnd" << "P1" << "P1P2Left" << "Retort";
    m_PipeListP2 << "P2RightEnd" << "P2" << "P1P2Right" << "Retort";
    m_PipeListP3 << "P3RightEnd" << "P3" << "P2P3" << "P1P2Right" << "Retort";

    m_PipeListS1 << "S1" << "S1S2" << "S2S3" << "S3S4" << "S4S5" << "S5S6"     << "S6S7"          << "S7S7RightPoint" << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";
    m_PipeListS2         <<   "S2" << "S2S3" << "S3S4" << "S4S5" << "S5S6"     << "S6S7"          << "S7S7RightPoint" << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";
    m_PipeListS3         <<             "S3" << "S3S4" << "S4S5" << "S5S6"     << "S6S7"          << "S7S7RightPoint" << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";
    m_PipeListS4         <<                       "S4" << "S4S5" << "S5S6"     << "S6S7"          << "S7S7RightPoint" << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";
    m_PipeListS5         <<                                 "S5" << "S5S6"     << "S6S7"          << "S7S7RightPoint" << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";
    m_PipeListS6         <<                                          "S6"      << "S6S7"          << "S7S7RightPoint" << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";

    m_PipeListS7 << "S7" << "S7S7RightPoint" << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";

    m_PipeListS8 << "S8" << "S8S9" << "S9S10" << "S10S11" << "S11S12" << "S12S13" << "S13S13RightPoint" << "VerticalPipe2"
                 << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";


    m_PipeListS9 <<           "S9" << "S9S10" << "S10S11" << "S11S12" << "S12S13" << "S13S13RightPoint" << "VerticalPipe2"
                 << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";

    m_PipeListS10 <<                    "S10" << "S10S11" << "S11S12" << "S12S13" << "S13S13RightPoint" << "VerticalPipe2"
                 << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";



    m_PipeListS11 <<                                "S11" << "S11S12" << "S12S13" << "S13S13RightPoint" << "VerticalPipe2"
                 << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";


    m_PipeListS12 <<                                            "S12" << "S12S13" << "S13S13RightPoint" << "VerticalPipe2"
                 << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";


    m_PipeListS13 <<                                                        "S13" << "S13S13RightPoint" << "VerticalPipe2"
                 << "VerticalPipe1" << "P3P3RightPoint" << "P2P3" << "P1P2Right" << "Retort";



}

/****************************************************************************/
/*!
 *  \brief Initialize Dashboard Stations IDs
 */
/****************************************************************************/
void CDashboardScene::InitDashboardStationIDs()
{
    m_DashboardStationIDs << "S1" << "S2" << "S3" << "S4" << "S5" << "S6" <<"S7"
                          << "S8" << "S9" <<"S10" <<"S11" <<"S12" <<"S13"
                          << "P1" << "P2" << "P3";
}

/****************************************************************************/
/*!
 *  \brief Initialize Dashboard Stations Labels
 */
/****************************************************************************/
void CDashboardScene::InitDashboardStationLabels()
{

    m_DashboardStationLabels << "1" << "2" << "3" << "4" << "5" << "6" << "7"
                             << "8" << "9" << "10" << "11" << "12" << "13"
                             << "Paraffin (1)" << "Paraffin (2)" << "Paraffin (3)";
}

/****************************************************************************/
/*!
 *  \brief Initialize Dashboard Station Groups
 */
/****************************************************************************/
void CDashboardScene::InitDashboardStationGroups()
{

    m_DashboardStationGroup << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE
                            << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE
                            << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE << STATIONS_GROUP_BOTTLE
                            << STATIONS_GROUP_BOTTLE
                            << STATIONS_GROUP_PARAFFINBATH << STATIONS_GROUP_PARAFFINBATH << STATIONS_GROUP_PARAFFINBATH;
}


void CDashboardScene::AddPathLeftTopArc(QPainterPath& path, QPointF& pnt)
{
    QPointF originalPoint(pnt.rx() + PipeWidth, pnt.ry() + PipeWidth);
    QPainterPath pathArc, pathArcTopLeft, pathArcBottomRight;
    pathArcTopLeft.moveTo(originalPoint.rx(), originalPoint.ry() - PipeWidth);
    pathArcTopLeft.arcTo(originalPoint.rx()- PipeWidth, originalPoint.ry() - PipeWidth, 2 * PipeWidth, 2 * PipeWidth, 90.0, 90.0);
    pathArcTopLeft.lineTo(originalPoint);
    pathArcTopLeft.lineTo(originalPoint.rx(), originalPoint.ry() - PipeWidth);
    pathArcTopLeft.closeSubpath();

    int harfPipeWidth = PipeWidth / 2;
    pathArcBottomRight.moveTo(originalPoint.rx() + harfPipeWidth, originalPoint.ry());
    pathArcBottomRight.arcTo(originalPoint.rx(), originalPoint.ry(), PipeWidth, PipeWidth, 90.0, 90.0);
    pathArcBottomRight.lineTo(originalPoint);
    pathArcBottomRight.lineTo(originalPoint.rx() + harfPipeWidth, originalPoint.ry());
    pathArcBottomRight.closeSubpath();

    pathArc = pathArcTopLeft.united(pathArcBottomRight);
    path.addPath(pathArc);
}

void CDashboardScene::AddPathRightBottomArc(QPainterPath& path, QPointF& pnt)
{
    QPainterPath pathRightBottom;
    pathRightBottom.moveTo(pnt.rx(), pnt.ry());
    pathRightBottom.arcTo(pnt.rx() - PipeWidth, pnt.ry() - PipeWidth, 2 * PipeWidth, 2 * PipeWidth, 270.0, 90.0);
    pathRightBottom.lineTo(pnt.rx(), pnt.ry());
    pathRightBottom.lineTo(pnt.rx(), pnt.ry() + PipeWidth);
    pathRightBottom.closeSubpath();
    path = pathRightBottom;
}

void CDashboardScene::AddPathRightTopArc(QPainterPath& path, QPointF& originalPoint, bool leftTopNeed)
{
    QPainterPath pathArc, pathTopArcTopRight, pathTopArcBottomLeft;
    pathTopArcTopRight.moveTo(originalPoint.rx() + PipeWidth, originalPoint.ry());
    pathTopArcTopRight.arcTo(originalPoint.rx()- PipeWidth, originalPoint.ry()- PipeWidth, 2 * PipeWidth, 2 * PipeWidth, 0.0, 90.0);
    pathTopArcTopRight.lineTo(originalPoint);
    pathTopArcTopRight.lineTo(originalPoint.rx() + PipeWidth, originalPoint.ry());
    pathTopArcTopRight.closeSubpath();

    if (leftTopNeed) {
        int harfPipeWidth = PipeWidth / 2;
        pathTopArcBottomLeft.moveTo(originalPoint.rx(), originalPoint.ry() + harfPipeWidth);
        pathTopArcBottomLeft.arcTo(originalPoint.rx() - PipeWidth, originalPoint.ry(), PipeWidth, PipeWidth, 0.0, 90.0);
        pathTopArcBottomLeft.lineTo(originalPoint);
        pathTopArcBottomLeft.lineTo(originalPoint.rx(), originalPoint.ry() + harfPipeWidth);
        pathTopArcBottomLeft.closeSubpath();
        pathArc = pathTopArcTopRight.united(pathTopArcBottomLeft);
    }
    else {
        pathArc = pathTopArcTopRight;
    }
    path.addPath(pathArc);
}

QPainterPath CDashboardScene::MakeHorizontalPipePath(const QString& stationID, const QString& otherStationID, bool bShorter)
{
    QPainterPath path;
    QPointF p1 = m_StationJointList[stationID];
    QPointF p2 = m_StationJointList[otherStationID];
    if (bShorter)
    {
        AddPathLeftTopArc(path, p1);
        path.addRect(p1.rx() + PipeWidth, p1.ry(), p2.rx() - p1.rx() - PipeWidth, PipeWidth);
    }
    else
        path.addRect(p1.rx(), p1.ry(), p2.rx() - p1.rx(), PipeWidth);
    return path;
}

QPainterPath CDashboardScene::MakeHorizontalBinaryPipePath(const QString& stationID, const QString& OtherStationID,
                                    const QString& MidTopStationID, bool IsReturnLeftOne,bool bShortor)//IsReturnLeftOne is false ,it will return the right one

{
    QPainterPath path;
    QPointF p1 = m_StationJointList[stationID];
    QPointF p2 = m_StationJointList[MidTopStationID];
    if (IsReturnLeftOne)
    {
        if (bShortor)
        {
            AddPathLeftTopArc(path, p1);
            path.addRect(p1.rx() + PipeWidth, p1.ry(), p2.rx() - p1.rx() - PipeWidth, PipeWidth);
        }
        else
           path.addRect(p1.rx(), p1.ry(), p2.rx() - p1.rx(), PipeWidth);
    }
    else
    {
         QPointF p3 = m_StationJointList[OtherStationID];
         path.addRect(p2.rx(), p1.ry(), p3.rx() - p2.rx(), PipeWidth);
    }
    return path;
}

void CDashboardScene::CollectPipeRect()
{
    int yOffsetS8 = 0;
    int yOffsetForS1 = -3;
    int yOffsetP1 = -1;
    m_PipeRectList.insert("S8S9", PipePathAndOrientation(MakeHorizontalPipePath("S8", "S9", true), "right", QPoint(0, yOffsetS8)));
    m_PipeRectList.insert("S9S10", PipePathAndOrientation(MakeHorizontalPipePath("S9", "S10"), "right", QPoint(0, yOffsetS8)));
    m_PipeRectList.insert("S10S11", PipePathAndOrientation(MakeHorizontalPipePath("S10", "S11"), "right", QPoint(0, yOffsetS8)));
    m_PipeRectList.insert("S11S12", PipePathAndOrientation(MakeHorizontalPipePath("S11", "S12"), "right", QPoint(0, yOffsetS8)));
    m_PipeRectList.insert("S12S13", PipePathAndOrientation(MakeHorizontalPipePath("S12", "S13"), "right", QPoint(0, yOffsetS8)));
    m_PipeRectList.insert("S13S13RightPoint", PipePathAndOrientation(MakeHorizontalPipePath("S13", "S13RightPoint"), "right", QPoint(0, yOffsetS8)));
    m_PipeRectList.insert("S1S2", PipePathAndOrientation(MakeHorizontalPipePath("S1", "S2", true), "right", QPoint(0, yOffsetForS1)));
    m_PipeRectList.insert("S2S3", PipePathAndOrientation(MakeHorizontalPipePath("S2", "S3"), "right", QPoint(0, yOffsetForS1)));
    m_PipeRectList.insert("S3S4", PipePathAndOrientation(MakeHorizontalPipePath("S3", "S4"), "right", QPoint(0, yOffsetForS1)));
    m_PipeRectList.insert("S4S5", PipePathAndOrientation(MakeHorizontalPipePath("S4", "S5"), "right", QPoint(0, yOffsetForS1)));

    m_PipeRectList.insert("S5S6", PipePathAndOrientation(MakeHorizontalPipePath("S5", "S6"), "right", QPoint(0, yOffsetForS1)));
    m_PipeRectList.insert("S6S7", PipePathAndOrientation(MakeHorizontalPipePath("S6", "S7"), "right", QPoint(0, yOffsetForS1)));
    m_PipeRectList.insert("S7S7RightPoint", PipePathAndOrientation(MakeHorizontalPipePath("S7", "S7RightPoint"), "right", QPoint(0, yOffsetForS1)));

    m_PipeRectList.insert("P2P3", PipePathAndOrientation(MakeHorizontalPipePath("P2", "P3"), "left", QPoint(0, yOffsetP1)));
    m_PipeRectList.insert("P3P3RightPoint", PipePathAndOrientation(MakeHorizontalPipePath("P3", "P3RightPoint"), "left", QPoint(0, yOffsetP1)));

    m_PipeRectList.insert("P1P2Right", PipePathAndOrientation(MakeHorizontalBinaryPipePath("P1", "P2", "Retort", false), "left", QPoint(0, yOffsetP1)));
    m_PipeRectList.insert("P1P2Left", PipePathAndOrientation(MakeHorizontalBinaryPipePath("P1", "P2", "Retort", true, true), "right", QPoint(0, yOffsetP1)));
}

void CDashboardScene::CreateAllPipe()
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    foreach (PipePathAndOrientation pathAndOrientation, m_PipeRectList)
    {
        path = path.united(pathAndOrientation.m_PainterPath);
    }

    m_WholePipeGraphicsRectItem = new QGraphicsPathItem(path);
    m_WholePipeGraphicsRectItem->setPen(QPen(QColor(0, 0, 0), 0, Qt::SolidLine,
                                             Qt::FlatCap, Qt::MiterJoin));
    m_WholePipeGraphicsRectItem->setBrush(QBrush(QColor(255, 255, 255), Qt::SolidPattern));//fill white in the pipe
    addItem(m_WholePipeGraphicsRectItem);
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class Dashboard::CPipeGraphicsPathItem
 */
/****************************************************************************/
class CPipeGraphicsPathItem: public QGraphicsPathItem
{
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CPipeGraphicsPathItem
     *
     *  \param p =  QPainterPath type parameter
     *  \param brushOrigin =  QPointF type parameter
     *
     *  \return from CPipeGraphicsPathItem
     */
    /****************************************************************************/
    CPipeGraphicsPathItem(const QPainterPath &p, const QPointF& brushOrigin):QGraphicsPathItem(p),
        m_BrushOrigin(brushOrigin)
    {}

  /****************************************************************************/
  /*!
   *  \brief  Definition/Declaration of function paint
   *
   *  \param painter = QPainter type parameter
   *  \param option =  QStyleOptionGraphicsItem type parameter
   *  \param widget =  QWidget type parameter
   *
   *  \return from paint
   */
  /****************************************************************************/
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
  {
        painter->setBrushOrigin(m_BrushOrigin);
        QGraphicsPathItem::paint(painter, option, widget);
  }
private:
  const QPointF m_BrushOrigin;       ///<  Definition/Declaration of variable m_BrushOrigin
};

void CDashboardScene::RepresentUsedPipe(const QList<QString>& selectedStationList, bool isRunning)
{
    Q_UNUSED(isRunning);
    if (m_UsedPipeGraphicsRectItem)
    {
        this->removeItem(m_UsedPipeGraphicsRectItem);
        delete  m_UsedPipeGraphicsRectItem;
        m_UsedPipeGraphicsRectItem = NULL;
    }

    QStringList totalPipeList;
    for (int i = 0; i < selectedStationList.size(); ++i)
    {
        QStringList* pipeList = GetPipeList(selectedStationList.at(i));
        if (!pipeList)
            continue;
        totalPipeList << *pipeList;
    }
    (void)totalPipeList.removeDuplicates();

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    foreach (const QString &strPipeID, totalPipeList)
    {
        path = path.united(m_PipeRectList[strPipeID].m_PainterPath);
    }

    m_UsedPipeGraphicsRectItem = new QGraphicsPathItem(path);
    m_UsedPipeGraphicsRectItem->setPen(QPen(QColor(0, 0, 0), 0, Qt::SolidLine,
                                             Qt::FlatCap, Qt::MiterJoin));
    //if (!isRunning)
         m_UsedPipeGraphicsRectItem->setBrush(QBrush(QColor(255, 255, 255), Qt::SolidPattern));//fill white in the pipe

    addItem(m_UsedPipeGraphicsRectItem);
}

void CDashboardScene::ClearUsedPipe()
{
    delete m_UsedPipeGraphicsRectItem;
    m_UsedPipeGraphicsRectItem = NULL;
}

void CDashboardScene::ClearCurrentWorkingPipe()
{
    for (int i = 0; i < m_pGraphicsPathItemPipeList.count(); i++)
    {
        delete m_pGraphicsPathItemPipeList.at(i);
    }
    m_pGraphicsPathItemPipeList.clear();
    m_PipeOrientationList.clear();
}

QStringList* CDashboardScene::GetPipeList(const QString& StationID)
{
    QStringList* pipeList = NULL;
    if ("P1" == StationID)
        pipeList = &m_PipeListP1;
    else if ("P2" == StationID)
        pipeList = &m_PipeListP2;
    else if ("P3" == StationID)
        pipeList = &m_PipeListP3;
    else if ("S1" == StationID)
        pipeList = &m_PipeListS1;
    else if ("S2" == StationID)
        pipeList = &m_PipeListS2;
    else if ("S3" == StationID)
        pipeList = &m_PipeListS3;
    else if ("S4" == StationID)
        pipeList = &m_PipeListS4;
    else if ("S5" == StationID)
        pipeList = &m_PipeListS5;
    else if ("S6" == StationID)
        pipeList = &m_PipeListS6;
    else if ("S7" == StationID)
        pipeList = &m_PipeListS7;
    else if ("S8" == StationID)
        pipeList = &m_PipeListS8;
    else if ("S9" == StationID)
        pipeList = &m_PipeListS9;
    else if ("S10" == StationID)
        pipeList = &m_PipeListS10;
    else if ("S11" == StationID)
        pipeList = &m_PipeListS11;
    else if ("S12" == StationID)
        pipeList = &m_PipeListS12;
    else if ("S13" == StationID)
        pipeList = &m_PipeListS13;
    return pipeList;
}

void CDashboardScene::RepresentCurrentWorkingPipe(const QString& StationID)
{
    //P1: P1, Retort Joint, P1P2Left
    QStringList* pipeList = GetPipeList(StationID);
    if (!pipeList)
        return;

    foreach (const QString &strPipeID, *pipeList)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addPath(m_PipeRectList[strPipeID].m_PainterPath);

        QGraphicsPathItem* pWorkingPipeGraphicsRectItem = new CPipeGraphicsPathItem(path, m_PipeRectList[strPipeID].m_brushOrigin);
        pWorkingPipeGraphicsRectItem ->setPen(QPen(Qt::NoPen));
        addItem(pWorkingPipeGraphicsRectItem);

        m_PipeOrientationList.append(m_PipeRectList[strPipeID].m_orientation);
        m_pGraphicsPathItemPipeList.append(pWorkingPipeGraphicsRectItem);
    }

    DestroyBrushArray();
    BuildBrushArray();
}

void CDashboardScene::BuildBrushArray()
{
    QPainter painter;
    QColor color(m_CurrentReagentColorValue);
    color.setAlpha(150);

    QMatrix matrix;
    (void)matrix.rotate(-90.0);
    QPixmap pixMapUp = m_PixMapFlowFrame1.transformed(matrix);//up

    (void)painter.begin(&pixMapUp);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[0][0] = new QBrush(pixMapUp);

    pixMapUp = m_PixMapFlowFrame2.transformed(matrix);//up
    (void)painter.begin(&pixMapUp);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[1][0] = new QBrush(pixMapUp);

    pixMapUp = m_PixMapFlowFrame3.transformed(matrix);//up
    (void)painter.begin(&pixMapUp);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[2][0] = new QBrush(pixMapUp);

    (void)matrix.rotate(-90.0);
    QPixmap pixMapLeft = m_PixMapFlowFrame1.transformed(matrix);//left
    (void)painter.begin(&pixMapLeft);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[0][2] = new QBrush(pixMapLeft);

    pixMapLeft = m_PixMapFlowFrame2.transformed(matrix);//left
    (void)painter.begin(&pixMapLeft);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[1][2] = new QBrush(pixMapLeft);

    pixMapLeft = m_PixMapFlowFrame3.transformed(matrix);//left
    (void)painter.begin(&pixMapLeft);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[2][2] = new QBrush(pixMapLeft);

    (void)matrix.rotate(-90.0);
    QPixmap pixMapDown = m_PixMapFlowFrame1.transformed(matrix);//down
    (void)painter.begin(&pixMapDown);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[0][1] = new QBrush(pixMapDown);

    pixMapDown = m_PixMapFlowFrame2.transformed(matrix);//down
    (void)painter.begin(&pixMapDown);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[1][1] = new QBrush(pixMapDown);

    pixMapDown = m_PixMapFlowFrame3.transformed(matrix);//down
    (void)painter.begin(&pixMapDown);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[2][1] = new QBrush(pixMapDown);

    QPixmap frame1 = m_PixMapFlowFrame1;
    (void)painter.begin(&frame1);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[0][3] = new QBrush(frame1);//right

    QPixmap frame2 = m_PixMapFlowFrame2;
    (void)painter.begin(&frame2);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[1][3] = new QBrush(frame2);//right

    QPixmap frame3 = m_PixMapFlowFrame3;
    (void)painter.begin(&frame3);
    painter.fillRect(0, 0, 20, 20, color);
    (void)painter.end();
    m_BrushArray[2][3] = new QBrush(frame3);//right
}

void CDashboardScene::DestroyBrushArray()
{
    for (int r = 0; r < 3; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            if (m_BrushArray[r][c] != NULL)
                delete m_BrushArray[r][c];
        }
    }
}

void CDashboardScene::PipeSuckDrainAnimation()
{
    int rowIndex = 0;
    if (0 == m_currentTimerOrder)
    {
        rowIndex = m_currentTimerOrder;
        m_currentTimerOrder++;
    }
    else if(1 == m_currentTimerOrder)
    {
        rowIndex = m_currentTimerOrder;
        m_currentTimerOrder++;
    }
    else if(2 == m_currentTimerOrder)
    {
        rowIndex = m_currentTimerOrder;
        m_currentTimerOrder = 0;
    }

    int j = 0;
    for (int i = 0; i < m_pGraphicsPathItemPipeList.size(); ++i)
    {
        QString pipeOrientation = m_PipeOrientationList.at(j);
        j++;

        int colIndex = 0;
        if ("up" == pipeOrientation)
        {
            if (m_IsSuck)
            {
                colIndex = 0;
            }
            else
            {
               colIndex = 1;
            }
        }
        else if ("left" == pipeOrientation)
        {
            if (m_IsSuck)
            {
                colIndex = 2;
            }
			else
			{
                colIndex = 3;
			}
        }
        else if ("right" == pipeOrientation)
        {
            if (!m_IsSuck)
            {
                colIndex = 2;
            }
			else
			  colIndex = 3;
        }

        m_pGraphicsPathItemPipeList.at(i)->setBrush(*(m_BrushArray[rowIndex][colIndex]));
    }
}

void CDashboardScene::BlinkingStation()
{
    // if the current tab is not program, this will be skipped, for station need not refresh.
    if (m_CurTabIndex != 0)
        return ;

    for (int i = 0; i < mp_DashboardStationItems.size(); i++)
    {
         Core::CDashboardStationItem* item = mp_DashboardStationItems.at(i);
         if (item)
         {
             if (item->IsReagentExpired() && item->isVisible())
             {
                 item->DrawStationItemImage();
             }
         }
    }
}

void CDashboardScene::EnableBlink(bool bEnable)
{
    // if the current tab is not program, this will be skipped, for station need not refresh.
    if (m_CurTabIndex != 0)
        return ;

    for (int i = 0; i < mp_DashboardStationItems.size(); i++)
    {
         Core::CDashboardStationItem* item = mp_DashboardStationItems.at(i);
         if (item)
         {
             if (item->isVisible())
             {
                 if (bEnable != item->IsBlinkEnable())
                 {
                    item->EnableBlink(bEnable);
                    if (item->IsReagentExpired())
                        item->DrawStationItemImage();
                 }
             }
         }
    }
}

void CDashboardScene::OnInteractStart()
{
    if (!m_bProcessRunning)
    {
        ExpiredReagentStationBlinking(false);
        if (!(m_pStartBlinkingTimer->isActive()))
            m_pStartBlinkingTimer->start();//for the next blinking in 6 seconds
    }
}

void CDashboardScene::ExpiredReagentStationBlinking(bool bStart)
{
    if (bStart)
    {
        m_pStartBlinkingTimer->stop();
        m_pStartBlinkingTimer->start();
        m_pBlinkingIntervalTimer->start();
        this->EnableBlink(bStart);
    }
    else//stop blink
    {
        if (m_pStartBlinkingTimer->isActive())
        {
            m_pStartBlinkingTimer->stop();
        }

        if (m_pBlinkingIntervalTimer->isActive())
        {
            m_pBlinkingIntervalTimer->stop();
        }
        //reset the all expired station
        this->EnableBlink(bStart);
    }
}

void CDashboardScene::OnAppIdle()
{
    m_pStartBlinkingTimer->stop();
    if (!m_bProcessRunning && (Global::RMS_OFF != mp_DataConnector->SettingsInterface->GetUserSettings()->GetModeRMSProcessing() ||
                Global::RMS_OFF != mp_DataConnector->SettingsInterface->GetUserSettings()->GetModeRMSCleaning()))
    {
        this->EnableBlink(true);
        m_pBlinkingIntervalTimer->start();
    }
}

void CDashboardScene::OnUpdateUserSettings()
{
     if (Global::RMS_OFF == mp_DataConnector->SettingsInterface->GetUserSettings()->GetModeRMSProcessing()&&
        Global::RMS_OFF == mp_DataConnector->SettingsInterface->GetUserSettings()->GetModeRMSCleaning())
     {
         if (m_pBlinkingIntervalTimer->isActive()) {
             m_pBlinkingIntervalTimer->stop();
         }
     }
     else
     {
         m_pBlinkingIntervalTimer->start();
     }
}

/****************************************************************************/
/*!
 *  \brief Add Dashboard Station Items to Scene
 */
/****************************************************************************/
void CDashboardScene::AddDashboardStationItemsToScene()
{
    m_StationJointList.clear();
    m_PipeRectList.clear();

    //Stations(include paraffin Bath)
    for (int i = 0; i < m_DashboardStationIDs.count(); i++)
    {
        // If station is found in the DashboardStations xmls,
        // take it from there or else
        // give the station a default value
        DataManager::CDashboardStation *p_DashboardStation = mp_DataConnector->DashboardStationList->GetDashboardStation((m_DashboardStationIDs[i]));

        if (p_DashboardStation) {
            m_DashboardStationList.insert(m_DashboardStationIDs[i], p_DashboardStation);
        }

        Core::CDashboardStationItem* p_DashboardStationItem = new Core::CDashboardStationItem(mp_DataConnector,                                                                                             
                                                                                              m_DashboardStationGroup[i],
                                                                                              m_DashboardStationIDs[i],
                                                                                              m_DashboardStationLabels[i],
                                                                                              m_DashboardStationList[m_DashboardStationIDs[i]]);
        p_DashboardStationItem->setPos(m_DashboardStationItemPositions[i]);
        p_DashboardStationItem->StationSelected(false);
        mp_DashboardStationItems.append(p_DashboardStationItem);

        QRectF rectStation = p_DashboardStationItem->boundingRect();
        QPointF posStation = p_DashboardStationItem->pos();
        qreal x = posStation.rx() + rectStation.width()/2 - 5;
        qreal y = posStation.ry() - JointHeight;
        m_StationJointList.insert(m_DashboardStationIDs[i], QPointF(x, y));

        addItem(p_DashboardStationItem);
        int xOrigin = 0;
        const QString& strID = m_DashboardStationIDs[i];
        if ("S9" == strID || "S11" == strID || "S13" == strID || "S6" == strID || "S4" == strID || "S2" == strID) {
           xOrigin = -5;
        }
        else if("P2" == strID) {
            xOrigin = -2;
        }
        else if("P3" == strID) {
            xOrigin = -3;
        }

        QPainterPath path;
        if ("P1" == strID || "P2" == strID || "P3" == strID) {
            path.addRect(x, y + PipeWidth, PipeWidth, JointHeight - PipeWidth);
        }
        else if ("S1" == strID || "S8" == strID) {
            path.addRect(x, y + PipeWidth, PipeWidth, JointHeight - PipeWidth + 2);
        }
        else
            path.addRect(x, y, PipeWidth, JointHeight + 2);

        if ("P2" == strID) {
            QPainterPath pathRigthTopArc;
            QPointF vPoint(m_StationJointList["P2"].rx(), m_StationJointList["P2"].ry() + 5);
            AddPathRightTopArc(pathRigthTopArc, vPoint, false);
            m_PipeRectList.insert("P2RightEnd", PipePathAndOrientation(pathRigthTopArc, "up", QPoint(0, 0)));
        }
        else if ("P3" == strID) {
            QPainterPath pathRigthTopArc;
            QPointF vPoint(m_StationJointList["P3"].rx(), m_StationJointList["P3"].ry() + 5);
            AddPathRightTopArc(pathRigthTopArc, vPoint, false);
            m_PipeRectList.insert("P3RightEnd", PipePathAndOrientation(pathRigthTopArc, "up", QPoint(0, 0)));
        }

        m_PipeRectList.insert(m_DashboardStationIDs[i], PipePathAndOrientation(path, "up", QPoint(xOrigin, 0)));
    }   

    // For Retort, No Station Id
    mp_DashboardStationRetort = new Core::CDashboardStationItem(mp_DataConnector, STATIONS_GROUP_RETORT, "Retort", "Retort");
    mp_DashboardStationRetort->setPos(QPoint(15, 35));
    addItem(mp_DashboardStationRetort);

    //Add Retort's rect and letfTop point
    QRectF rectStation = mp_DashboardStationRetort->boundingRect();
    QPointF posStation = mp_DashboardStationRetort->pos();
    qreal x = posStation.rx() + rectStation.width()/2 - 5;
    qreal y = posStation.ry() + rectStation.height();
    m_StationJointList.insert("Retort", QPointF(x, y));
    int hh = (int)m_StationJointList["P1"].ry()  - y + (PipeWidth / 3);

    QPainterPath pathRigthBottomArc;
    QPointF vPoint(m_StationJointList["Retort"].rx(), m_StationJointList["Retort"].ry()+7);
    AddPathRightBottomArc(pathRigthBottomArc, vPoint);
    m_PipeRectList.insert("P1LeftEnd", PipePathAndOrientation(pathRigthBottomArc, "up", QPoint(0, 0)));

    QPainterPath pathRetort;
    pathRetort.addRect(x, y, PipeWidth, hh);
    m_PipeRectList.insert("Retort", PipePathAndOrientation(pathRetort, "up", QPoint(-1, 0)));

    //P3 Right point
    int harfPipeWidth = PipeWidth / 2;

    int paraffinbathBoundingRectWidth = 120;
    QPointF P3RightPoint(m_StationJointList["P3"].rx() + paraffinbathBoundingRectWidth/2 + 72, m_StationJointList["P3"].ry());
    m_StationJointList.insert("P3RightPoint", P3RightPoint);

    //v1TopPoint
    QPointF v1Point(P3RightPoint.rx(), m_StationJointList["P3"].ry() + PipeWidth);

    QPainterPath pathVerticalPipe1;
    AddPathRightTopArc(pathVerticalPipe1, v1Point);

    int h = (int)m_StationJointList["S5"].ry() - (int)m_StationJointList["P1"].ry();
    QRectF rect;
    rect.setTopLeft(v1Point);//vertical pipe 1
    rect.setWidth(PipeWidth);
    rect.setHeight(h);


    pathVerticalPipe1.addRect(rect);
    m_PipeRectList.insert("VerticalPipe1", PipePathAndOrientation(pathVerticalPipe1, "up", QPoint(-8, 0)));


    //S7 Right point
    QPointF S7RightPoint(P3RightPoint.rx(), m_StationJointList["S7"].ry());
    m_StationJointList.insert("S7RightPoint", S7RightPoint);

    //v2TopPoint
    QPointF v2TopPoint(S7RightPoint.rx(), m_StationJointList["S7RightPoint"].ry() + PipeWidth);

    QPainterPath pathVerticalPipe2;
    //v2BottomPoint
    QPointF v2BottomPoint(S7RightPoint.rx(), m_StationJointList["S13"].ry());

    QPainterPath pathBottomArcVerticalPipe2, pathBottomArcBottomRightVerticalPipe2, pathBottomArcTopLeftVerticalPipe2;
    pathBottomArcBottomRightVerticalPipe2.moveTo(v2BottomPoint.rx(), v2BottomPoint.ry());
    pathBottomArcBottomRightVerticalPipe2.arcTo(v2BottomPoint.rx()- PipeWidth, v2BottomPoint.ry()- PipeWidth, 2 * PipeWidth, 2 * PipeWidth, 270.0, 90.0);//90.0 for counter-clockwise
    pathBottomArcBottomRightVerticalPipe2.lineTo(v2BottomPoint.rx(), v2BottomPoint.ry());
    pathBottomArcBottomRightVerticalPipe2.lineTo(v2BottomPoint.rx(), v2BottomPoint.ry() + PipeWidth);
    pathBottomArcBottomRightVerticalPipe2.closeSubpath();

    pathBottomArcTopLeftVerticalPipe2.moveTo(v2BottomPoint.rx() - harfPipeWidth, v2BottomPoint.ry());
    pathBottomArcTopLeftVerticalPipe2.arcTo(v2BottomPoint.rx() - PipeWidth, v2BottomPoint.ry() - PipeWidth, PipeWidth, PipeWidth, 270.0, 90.0);
    pathBottomArcTopLeftVerticalPipe2.lineTo(v2BottomPoint);
    pathBottomArcTopLeftVerticalPipe2.lineTo(v2BottomPoint.rx() - harfPipeWidth, v2BottomPoint.ry());
    pathBottomArcTopLeftVerticalPipe2.closeSubpath();

    pathBottomArcVerticalPipe2 = pathBottomArcBottomRightVerticalPipe2.united(pathBottomArcTopLeftVerticalPipe2);
    pathVerticalPipe2.addPath(pathBottomArcVerticalPipe2);

    h = (int)m_StationJointList["S13"].ry() - (int)m_StationJointList["S7"].ry() - PipeWidth;
    rect.setTopLeft(v2TopPoint);//vertical pipe 2
    rect.setWidth(PipeWidth);
    rect.setHeight(h);
    pathVerticalPipe2.addRect(rect);
    m_PipeRectList.insert("VerticalPipe2", PipePathAndOrientation(pathVerticalPipe2, "up", QPoint(-8, 0)));

    //S13 right point
    QPointF S13RightPoint(S7RightPoint.rx(), m_StationJointList["S13"].ry());
    m_StationJointList.insert("S13RightPoint", S7RightPoint);

    this->CollectPipeRect();
    CreateAllPipe();



}

/****************************************************************************/
/*!
 *  \brief Handler when entire Stations xml file is sent
 */
/****************************************************************************/

void CDashboardScene::UpdateDashboardStations()
{
    int count = mp_DataConnector->DashboardStationList->GetNumberOfDashboardStations();
    for (int i = 0; i < count; i++)
    {
        DataManager::CDashboardStation *p_DashboardStation = const_cast<DataManager::CDashboardStation*>(mp_DataConnector->DashboardStationList->GetDashboardStation(i));
        m_DashboardStationList.insert(m_DashboardStationIDs[i], p_DashboardStation);

        Core::CDashboardStationItem *pListItem =
                static_cast<Core::CDashboardStationItem *>(mp_DashboardStationItems[i]);

        pListItem->SetDashboardStation(p_DashboardStation);
        pListItem->StationSelected(true);
    }
}

void CDashboardScene::UpdateDashboardStation(const QString& strStationId)
{
   for (int i = 0; i < mp_DashboardStationItems.size(); i++)
   {
        Core::CDashboardStationItem* item = mp_DashboardStationItems.at(i);
        if (item->GetStationItemID()== strStationId)
        {
            item->UpdateDashboardStationItemReagent();
            break;
        }
   }
}

void CDashboardScene::UpdateDashboardSceneReagentsForProgram(QString &programId, QList<QString>& selectedStationList)
{
    m_SelectedStationList.clear();
    m_SelectedStationList << selectedStationList;
    if (programId.isEmpty())
    {
        this->addItem(m_WholePipeGraphicsRectItem);

    }
    else
    {
        this->removeItem(m_WholePipeGraphicsRectItem);
        this->RepresentUsedPipe(m_SelectedStationList, false);
    }

    DataManager::CProgram const *p_Program = mp_DataConnector->ProgramList->GetProgram(programId);

    for(int j = 0 ; j < m_DashboardStationList.count(); j++)
    {
        Core::CDashboardStationItem *pListItem =
            static_cast<Core::CDashboardStationItem *>(mp_DashboardStationItems[j]);

        if(!p_Program)//reset
        {
            pListItem->StationSelected(true);
            pListItem->setVisible(true);
        }
        else
        {
            if(selectedStationList.contains(pListItem->GetDashboardStation()->GetDashboardStationID()))
            {
                pListItem->StationSelected(true);
                pListItem->setVisible(true);
                pListItem->UpdateDashboardStationItemReagent(false);
                if (DataManager::CONTAINER_STATUS_FULL != pListItem->GetContainerStatus())
                {
                    pListItem->SetContainerStatus(DataManager::CONTAINER_STATUS_FULL);
                }
            }
            else
            {
                pListItem->StationSelected(false);
                pListItem->setVisible(false);
            }
        }
    }
}

void CDashboardScene::UpdateRetortStatus(DataManager::ContainerStatusType_t retortStatusType, const QString& reagentGroupID, const QString& stationID)
{
    if (!reagentGroupID.isEmpty())
    {
        DataManager::CReagentGroup const *p_ReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(reagentGroupID);

        m_CurrentReagentColorValue = p_ReagentGroup->GetGroupColor();
        (void)m_CurrentReagentColorValue.prepend("#");
    }
    mp_DashboardStationRetort->SetContainerStatus(retortStatusType, m_CurrentReagentColorValue);

    if (retortStatusType == DataManager::CONTAINER_STATUS_FULL)
    {
        for (int i = 0; i < mp_DashboardStationItems.size(); i++)
        {
             Core::CDashboardStationItem* item = mp_DashboardStationItems.at(i);
             if (item->GetStationItemID()== stationID)
             {
                 item->SetContainerStatus(DataManager::CONTAINER_STATUS_EMPTY, "");
                 break;
             }
        }
    }

}

void CDashboardScene::UpdateRetortLockedStatus(bool locked)
{
    mp_DashboardStationRetort->SetContainerRetortLockedStatus(locked);
}

void CDashboardScene::OnPauseStationSuckDrain()
{
    m_pPipeAnimationTimer->stop();
    //find Station and let Station animation pause
   for (int i = 0; i < mp_DashboardStationItems.size(); i++)
   {
        Core::CDashboardStationItem* item = mp_DashboardStationItems.at(i);
        if (item->GetStationItemID() == m_SuckDrainStationId)
        {
            item->PauseSuckDrain();
            mp_DashboardStationRetort->StationSelected(true);
            mp_DashboardStationRetort->PauseSuckDrain();
            break;
        }
   }
}

void CDashboardScene::OnStationSuckDrain(const QString& StationId, bool IsStart, bool IsSuck)
{
   m_IsSuck = IsSuck;
   m_SuckDrainStationId = StationId;

    //find Station and check it is visible or not,if it is invisible, let it be visible
   for (int i = 0; i < mp_DashboardStationItems.size(); i++)
   {
       Core::CDashboardStationItem* item = mp_DashboardStationItems.at(i);
       if (item->GetStationItemID() == m_SuckDrainStationId)
       {
           if (!item->isVisible())
           {
               item->StationSelected(true);
               item->setVisible(true);
           }
           break;
       }
   }

   ClearCurrentWorkingPipe();
   if (IsStart)
   {
       RepresentCurrentWorkingPipe(StationId);
   }

   if (IsStart)
   {
       //Get Current selected reagent corlor for Retort's representation
      DataManager::CDashboardStation* pDashboardStation = NULL;
      QHash<QString, DataManager::CDashboardStation*>::const_iterator it = m_DashboardStationList.find(StationId);
      if(it != m_DashboardStationList.constEnd()) {
           pDashboardStation = it.value();
      }
      if (pDashboardStation)
      {
          QString ReagentID = pDashboardStation->GetDashboardReagentID();
          if ("" != ReagentID)
          {
              DataManager::CReagent const *p_Reagent = mp_DataConnector->ReagentList->GetReagent(ReagentID);
              if (p_Reagent)
              {
                  QString ReagentGroupId = p_Reagent->GetGroupID();
                  DataManager::CReagentGroup const *p_ReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(ReagentGroupId);

                  m_CurrentReagentColorValue = p_ReagentGroup->GetGroupColor();
                  (void)m_CurrentReagentColorValue.prepend("#");
                   RepresentCurrentWorkingPipe(StationId);
                   m_pPipeAnimationTimer->start(PIPE_ANIMATION_INTERVAL);
              }
          }
      }


   }
   else
   {
        m_pPipeAnimationTimer->stop();
   }

    //find Station and let Station animation start/stop
   for (int i = 0; i < mp_DashboardStationItems.size(); i++)
   {
        Core::CDashboardStationItem* item = mp_DashboardStationItems.at(i);
        if (item->GetStationItemID() == StationId)
        {
            item->SuckDrain(IsStart, IsSuck);
            mp_DashboardStationRetort->StationSelected(true);
            mp_DashboardStationRetort->SuckDrain(IsStart, IsSuck, m_CurrentReagentColorValue);
            break;
        }
   }
}

bool CDashboardScene::HaveExpiredReagent()
{
    for (int i = 0; i < mp_DashboardStationItems.size(); i++)
    {
         Core::CDashboardStationItem* item = mp_DashboardStationItems.at(i);
		 
         if (item)
         {
             if (item->GetStationSelect() && item->IsReagentExpired())
                 return true;
         }
    }
    return false;
}

void CDashboardScene::OnProcessStateChanged()
{
    m_bProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if(m_bProcessRunning)
    {
        this->ExpiredReagentStationBlinking(false);
    }
    else
    {
        this->OnPauseStationSuckDrain();
        this->ExpiredReagentStationBlinking(true);
    }
}

/****************************************************************************/
/*!
 *  \brief OnCurrentTabChanged.
 *
 *  \iparam CurrentIndex = Currently selected
 */
/****************************************************************************/
void CDashboardScene::OnCurrentTabChanged(int CurrentIndex)
{
     m_CurTabIndex = CurrentIndex;
}
} // end namespace Dashboard
