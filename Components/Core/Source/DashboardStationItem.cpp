/****************************************************************************/
/*! \file DashboardStationItem.cpp
 *
 *  \brief DashboardStationItem implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16-Jan-2013
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

#include "Core/Include/DashboardStationItem.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "HimalayaDataContainer/Helper/Include/Global.h"
#include "Core/Include/GlobalHelper.h"


namespace Core {


//!< Self explaining. Just a random value.
#define ANIMATION_COUNT_MAX 10000

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_DataConnector = Instance of Data Container
 *  \iparam p_Parent = Parent item
 *  \param StationGroup
 *  \param StationItemID
 *  \param StationLabel
 *  \param p_DashboardStation
 */
/****************************************************************************/
CDashboardStationItem::CDashboardStationItem(Core::CDataConnector *p_DataConnector,
                                             StationGroupType_t StationGroup,
                                             const QString& StationItemID,
                                             QString StationLabel,
                                             DataManager::CDashboardStation *p_DashboardStation,
                                             QGraphicsItem *p_Parent) : QGraphicsItem(p_Parent),
    mp_DataConnector(p_DataConnector),
    m_CurRMSMode(Global::RMS_UNDEFINED),
    mp_DashboardStation(p_DashboardStation),
    m_StationItemID(StationItemID),
    m_DashboardStationGroup(StationGroup),
    m_StationItemLabel(StationLabel),
    m_RetortBoundingRectWidth(177),
    m_RetortBoundingRectHeight(92),
    m_RetortCoverHeight(25),
    m_ParaffinbathBoundingRectWidth(120),
    m_ParaffinbathBoundingRectHeight(65),
    m_ParaffinbathCoverHeight(11),
    m_BottleBoundingRectWidth(62),
    m_BottleBoundingRectHeight(139),
    m_BottleCoverHeight(38),
    m_CurrentBoundingRectReagentHeight(0),
    m_ReagentExpiredFlag(false),
    m_StationSelected(false),
    m_ContainerStatusType(DataManager::CONTAINER_STATUS_FULL),
    m_ExpiredColorRed(true),
    m_EnableBlink(true),
    m_IsRetortContaminated(false),
    m_RetortLocked(false),
    m_PixmapLabel(100, 50),
    m_PixmapReagentName(60, 145),
    m_bRefreshDashboard(true),
    m_ReagentName("")
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    setCacheMode(QGraphicsItem::ItemCoordinateCache);

    if(m_DashboardStationGroup == STATIONS_GROUP_RETORT) {
        m_Image = QPixmap(m_RetortBoundingRectWidth, m_RetortBoundingRectHeight);
    } else if(m_DashboardStationGroup == STATIONS_GROUP_PARAFFINBATH) {
        m_Image = QPixmap(m_ParaffinbathBoundingRectWidth, m_ParaffinbathBoundingRectHeight);
    } else {
        m_Image = QPixmap(m_BottleBoundingRectWidth, m_BottleBoundingRectHeight);
    }

    (void)m_PixmapRetortUnlocked.load(":/HimalayaImages/Icons/Dashboard/Retort/Retort_Unlocked.png");
    (void)m_PixmapRetortLocked01.load(":/HimalayaImages/Icons/Dashboard/Retort/Retort_Locked01.png");
    (void)m_PixmapRetortLocked02.load(":/HimalayaImages/Icons/Dashboard/Retort/Retort_Locked02.png");
    (void)m_PixmapParaffinbathBackground.load(":/HimalayaImages/Icons/Dashboard/Paraffinbath/Paraffinbath_Background.png");
    (void)m_PixmapBottleBackground.load(":/HimalayaImages/Icons/Dashboard/Bottle/Bottle_Background.png");
    (void)m_PixmapBottleHandle.load(":/HimalayaImages/Icons/Dashboard/Bottle/Bottle_Handle.png");
    CONNECTSIGNALSLOT(mp_DataConnector, ReagentUpdated(), this, UpdateDashboardStationItemReagentWhenReagentUpdated());
    CONNECTSIGNALSLOT(mp_DataConnector, DashboardStationsUpdated(), this, UpdateDashboardStationItemReagentWhenReagentUpdated());
    CONNECTSIGNALSLOT(mp_DataConnector, UserSettingsUpdated(), this, UpdateUserSettings());
    CONNECTSIGNALSLOT(mp_DataConnector, ReagentGroupUpdated(), this, UpdateDashboardStationItemWhenReagentGroupUpdated());
    PrepareStationItemLabel();
    UpdateImage();

    mp_SuckDrainTimer = new QTimer(this);
    mp_SuckDrainTimer->setInterval(1000);
    CONNECTSIGNALSLOT(mp_SuckDrainTimer, timeout(), this, SuckDrainAnimation());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDashboardStationItem::~CDashboardStationItem()
{
    try {
        mp_DataConnector = NULL;
        mp_DashboardStation = NULL;
        delete mp_SuckDrainTimer;
    } catch(...) {

    }

}

/****************************************************************************/
/*!
 *  \brief Gets the bounding rectangle
 *
 *  \return Bounding rectangle
 */
/****************************************************************************/
QRectF CDashboardStationItem::boundingRect() const
{
    if(STATIONS_GROUP_RETORT == m_DashboardStationGroup) {
        return QRectF(0, 0, m_RetortBoundingRectWidth, m_RetortBoundingRectHeight);
    } else if(STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup) {
        return QRectF(0, 0, m_ParaffinbathBoundingRectWidth, m_ParaffinbathBoundingRectHeight);
    } else {
        return QRectF(0, 0, m_BottleBoundingRectWidth, m_BottleBoundingRectHeight);
    }
}

/****************************************************************************/
/*!
 *  \brief Paint the item
 *
 *  \iparam p_Painter = Pointer to the painter object
 */
/****************************************************************************/
void CDashboardStationItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *,
                                  QWidget *)
{
    if (this->isVisible())
        p_Painter->drawPixmap(0, 0, m_Image);
}

/****************************************************************************/
/*!
 *  \brief Updated the station item
 *
 */
/****************************************************************************/
void CDashboardStationItem::UpdateImage()
{
    qDebug() << "STATION GROUP = " << (int)m_DashboardStationGroup;
    if (!mp_DashboardStation)
    {
        //Loading the Static Images and Drawingthe Static Label
        QPainter Painter(&m_Image);
        Painter.fillRect(this->boundingRect(), QColor(226, 227, 228));
        LoadStationImages(Painter);
        DrawStationItemLabel(Painter);
    }
}

/****************************************************************************/
/*!
 *  \brief Gets the data manager station data object
 *
 *  \return Station data object
 */
/****************************************************************************/
DataManager::CDashboardStation *CDashboardStationItem::GetDashboardStation() const
{
    if (mp_DashboardStation != NULL) {
        return mp_DashboardStation;
    }
    else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the data manager station data object
 *
 *  \iparam p_DashboardStation = Station data object
 */
/****************************************************************************/
void CDashboardStationItem::SetDashboardStation(DataManager::CDashboardStation* p_DashboardStation)
{
    mp_DashboardStation = p_DashboardStation;
    if(mp_DashboardStation) {
        m_DashboardStationID = mp_DashboardStation->GetDashboardStationID();
    }
}

/****************************************************************************/
/*!
 *  \brief Changes whether the station is pressed or not
 *
 *  \return Empty (true) or not (false)
 */
/****************************************************************************/
bool CDashboardStationItem::IsEmpty()
{
    if (mp_DashboardStation) {
        return true;
    } else {
        return false;
    }
}


void CDashboardStationItem::LoadStationImages(QPainter& Painter)
{
    if(STATIONS_GROUP_RETORT == m_DashboardStationGroup) {
        if (false == m_RetortLocked)
        {
            Painter.drawPixmap(0, 0, m_PixmapRetortUnlocked);
        }
        else
        {
            Painter.drawPixmap(0, 0, m_PixmapRetortLocked01);
            Painter.drawPixmap(0, 0, m_PixmapRetortLocked02);
        }
    } else if( STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup) {
        Painter.drawPixmap(0, 0, m_PixmapParaffinbathBackground);
    } else {
        Painter.drawPixmap(0, 0, m_PixmapBottleBackground);
        Painter.drawPixmap(0, 0, m_PixmapBottleHandle);
    }
}


/****************************************************************************/
/*!
 *  \brief Update Dashboard Station Items Reagent Properties when Reagents are updated
 *
 */
/****************************************************************************/

void CDashboardStationItem::UpdateDashboardStationItemReagentWhenReagentUpdated()
{
    if (!mp_DashboardStation)
        return;

    QString ReagentID = mp_DashboardStation->GetDashboardReagentID();
    DataManager::CReagent *p_Reagent = const_cast<DataManager::CReagent*>(mp_DataConnector->ReagentList->GetReagent(ReagentID));

    if (p_Reagent)
    {
        DataManager::ReagentStatusType_t reagentStatus = mp_DashboardStation->GetReagentStatus(*p_Reagent, m_CurRMSMode);
        bool flag = ( reagentStatus == DataManager::REAGENT_STATUS_EXPIRED );
        bool bRedraw = false;
        if ( flag != m_ReagentExpiredFlag )
        {
            m_ReagentExpiredFlag = flag;
            bRedraw = true;
        }

        if (p_Reagent->GetReagentName() != m_ReagentName)
        {
            bRedraw = true;
            PrepareReagentName();
        }

        if (bRedraw)
            DrawStationItemImage();
    }
    else
        DrawStationItemImage();

}

/****************************************************************************/
/*!
 *  \brief Update Dashboard Station Items Reagent Properties when Reagents Group Color are updated
 *
 */
/****************************************************************************/

void CDashboardStationItem::UpdateDashboardStationItemWhenReagentGroupUpdated()
{
    if (!mp_DashboardStation)
        return;
    QString ReagentID = mp_DashboardStation->GetDashboardReagentID();
    DataManager::CReagent *p_Reagent = const_cast<DataManager::CReagent*>(mp_DataConnector->ReagentList->GetReagent(ReagentID));

    if (p_Reagent)
    {
        QString ReagentGroupId = p_Reagent->GetGroupID();
        DataManager::CReagentGroup const *p_ReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(ReagentGroupId);

        QString ColorValue = p_ReagentGroup->GetGroupColor();
        (void)ColorValue.prepend("#");

        if ( ColorValue != m_ReagentDisplayColorValue )
        {
            DrawStationItemImage();
        }
    }
}


/****************************************************************************/
/*!
 *  \brief Update Dashboard Station Items Reagent Properties when Reagents are updated
 *
 *  \param RefreshFlag = bool type parameter
 *
 */
/****************************************************************************/

void CDashboardStationItem::UpdateDashboardStationItemReagent(bool RefreshFlag)
{
    if (!mp_DashboardStation)
        return;

    QString ReagentID = mp_DashboardStation->GetDashboardReagentID();
    DataManager::CReagent *p_Reagent = const_cast<DataManager::CReagent*>(mp_DataConnector->ReagentList->GetReagent(ReagentID));

    if (p_Reagent)
    {
        //if(m_CurRMSMode != m_UserSettings.GetModeRMSProcessing())
        {
            QString ReagentGroupId = p_Reagent->GetGroupID();
            DataManager::CReagentGroup const *p_ReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(ReagentGroupId);
            if (p_ReagentGroup->IsCleaningReagentGroup()) {
                m_CurRMSMode = m_UserSettings.GetModeRMSCleaning();
            }
            else {
                m_CurRMSMode = m_UserSettings.GetModeRMSProcessing();
            }

            DataManager::ReagentStatusType_t reagentStatus = mp_DashboardStation->GetReagentStatus(*p_Reagent, m_CurRMSMode);

            if (reagentStatus == DataManager::REAGENT_STATUS_EXPIRED)
                m_ReagentExpiredFlag = true;
            else
                m_ReagentExpiredFlag = false;
        }
        if (RefreshFlag)
            PrepareReagentName();
    }

    if (RefreshFlag)
        DrawStationItemImage();
}

void CDashboardStationItem::DrawStationItemImage()
{
    QPainter Painter(&m_Image);
    Painter.fillRect(this->boundingRect(), QColor(226, 227, 228));
    LoadStationImages(Painter);

    QString ReagentStatus = "";
    QString ReagentID = "";
    if (mp_DashboardStation)
    {
        ReagentStatus = mp_DashboardStation->GetDashboardReagentStatus();
        ReagentID = mp_DashboardStation->GetDashboardReagentID();
    }
    // If Reagent Status is Not Empty and Station Selected for the Program then fill the Reagent Color
    if(0 != ReagentStatus.compare("Empty", Qt::CaseInsensitive)) {
        FillReagentColor(Painter);
    }

    if(("" != ReagentID) && (STATIONS_GROUP_BOTTLE == m_DashboardStationGroup))
    {
        DrawReagentName(Painter);
    }

    DrawStationItemLabel(Painter);
    update();
}

void CDashboardStationItem::PrepareReagentName()
{
    m_PixmapReagentName.fill(Qt::transparent);
    QPainter Painter;
    (void)Painter.begin(&m_PixmapReagentName);

    QFont textFont;
    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.setPen(Qt::black);
    textFont.setPointSize(8);
    textFont.setBold(true);
    Painter.setFont(textFont);

    if (mp_DashboardStation)
    {
        QString ReagentID = mp_DashboardStation->GetDashboardReagentID();
        DataManager::CReagent const *p_Reagent = mp_DataConnector->ReagentList->GetReagent(ReagentID);
        if (p_Reagent)
        {
            Painter.rotate(270.0);
            m_ReagentName = p_Reagent->GetReagentName();
            QRect rect(-130, 23, 90, 60 );
            DrawGlowBoundaryText(textFont, m_ReagentName, rect, Painter);
            Painter.rotate(-270.0);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Draw Reagent Name on the Bottle Image
 *
 */
/****************************************************************************/
void CDashboardStationItem::DrawReagentName(QPainter & Painter)
{
    QRect rect(0, 0, 60, 145);
    Painter.drawPixmap(rect, m_PixmapReagentName, rect);
}

void CDashboardStationItem::DrawGlowBoundaryText(QFont& textFont, QString& text, QRect& rect, QPainter& painter)
{
    QFontMetrics fm(textFont);
    int stringWidth = fm.width(text);
    QString tempString(text);
    if (stringWidth > rect.width())
    {
        tempString = CGlobalHelper::TrimText(fm, text, rect.width(), true);
    }

    QPainterPath textPath;
    textPath.addText(rect.x(), rect.y()+ fm.height() -1 - fm.descent(), textFont, tempString);
    QPainterPathStroker pps;
    pps.setWidth(3);
    pps.setDashPattern(Qt::NoPen);
    pps.setCurveThreshold(0.001);
    pps.setCapStyle(Qt::RoundCap);
    pps.setJoinStyle(Qt::RoundJoin);
    QPainterPath strokePath = pps.createStroke(textPath);
    painter.fillPath(strokePath, QBrush(QColor(Qt::white)));
    painter.setPen(Qt::black);
    painter.drawText(rect.x(), rect.y()+ fm.height() -1 - fm.descent(), tempString);
}

void CDashboardStationItem::PrepareStationItemLabel()
{
    m_PixmapLabel.fill(Qt::transparent);
    QPainter painter;
    (void)painter.begin(&m_PixmapLabel);

    QFont textFont;
    textFont.setPointSize(8);
    painter.setRenderHint(QPainter::Antialiasing);
    if(m_StationSelected) {
        textFont.setBold(true);
    }

    painter.setFont(textFont);
    QRect rect;
    if(STATIONS_GROUP_RETORT == m_DashboardStationGroup) {
        textFont.setBold(true);
        rect.setRect(0,0,67,15);
    } else if( STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup) {
        rect.setRect(0,0,67,15);
    } else {
        rect.setRect(0,0,33,16);
    }
    DrawGlowBoundaryText(textFont, m_StationItemLabel, rect, painter);
    (void)painter.end();
}

void CDashboardStationItem::DrawStationItemLabel(QPainter& painter)
{
    QRect rectDest, rectSource;
    if(STATIONS_GROUP_RETORT == m_DashboardStationGroup) {
        rectDest.setRect(71, 45, 67, 15);
        rectSource.setRect(0, 0, 67, 15);
    } else if( STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup) {
        rectDest.setRect(33, 22, 67, 15);
        rectSource.setRect(0, 0, 67, 15);
    } else {
        painter.drawPixmap(0, 0, m_PixmapBottleHandle);
        rectDest.setRect(23, 9, 33, 16);
        rectSource.setRect(0, 0, 33, 16);
    }

    painter.drawPixmap(rectDest, m_PixmapLabel, rectSource);
}

void CDashboardStationItem::EnableBlink(bool bEnable)
{
    m_EnableBlink = bEnable;
    m_ExpiredColorRed = false;
}

bool CDashboardStationItem::IsBlinkEnable()
{
    return m_EnableBlink;
}

void CDashboardStationItem::FillReagentColor(QPainter & Painter)
{
    int fillBottleWidth;
    qreal fillBottleHeight;
    int fillParaffinbathWidth;
    qreal fillParaffinbathHeight;
    int fillRetortWidth;
    qreal fillRetortHeight;
    QString reagentColorValue;

    if(mp_DashboardStation)
    {
        QString ReagentID = mp_DashboardStation->GetDashboardReagentID();
        if ("" == ReagentID)
            return;
        DataManager::CReagent const *p_Reagent = mp_DataConnector->ReagentList->GetReagent(ReagentID);
        if (!p_Reagent)
            return;
        QString ReagentGroupId = p_Reagent->GetGroupID();
        DataManager::CReagentGroup const *p_ReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(ReagentGroupId);

        reagentColorValue = p_ReagentGroup->GetGroupColor();
        (void)reagentColorValue.prepend("#");

        m_ReagentDisplayColorValue = reagentColorValue;
    }
    else
    {
        reagentColorValue = m_CurrentReagentColorValue;
    }

    QColor color(reagentColorValue);
    if(STATIONS_GROUP_BOTTLE == m_DashboardStationGroup || STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup)
    {
        if(m_EnableBlink && m_ReagentExpiredFlag && m_StationSelected)
        {
            //set expired color only once
            if (m_ExpiredColorRed)
            {
                m_ExpiredColorRed = false;
            }
            else
            {
                color.setRgb(255, 0, 0);
                m_ExpiredColorRed = true;
            }
        }
    }
    Painter.setPen(color);
    Painter.setBrush(color);

    if(STATIONS_GROUP_BOTTLE == m_DashboardStationGroup)
    {
        // Since the Painter is rotated, Width and Height Axis Changes.
        fillBottleWidth = m_BottleBoundingRectWidth - 12;  // Manual Pixel Calculation
        fillBottleHeight = m_BottleBoundingRectHeight - m_BottleCoverHeight;  // Manual Pixel Calculation
        if (m_ContainerStatusType == DataManager::CONTAINER_STATUS_SCUKING
                || m_ContainerStatusType == DataManager::CONTAINER_STATUS_DRAINING
                || m_ContainerStatusType == DataManager::CONTAINER_STATUS_EMPTY)
        {
            fillBottleHeight = m_CurrentBoundingRectReagentHeight;
        }
        else if (m_ContainerStatusType == DataManager::CONTAINER_STATUS_FULL)
        {
            m_CurrentBoundingRectReagentHeight = fillBottleHeight;
        }
        qreal top = m_BottleBoundingRectHeight - fillBottleHeight;
        QPainterPath path;
        path.setFillRule( Qt::WindingFill );
        path.addRoundedRect(QRectF(4, top, fillBottleWidth,
                                  fillBottleHeight -2), 8, 4);

        int cornerHeight = 8;
        if (fillBottleHeight > cornerHeight && fillBottleHeight < m_BottleBoundingRectHeight - 25 - cornerHeight + 4)
        {
            path.addRect(QRectF(4, top, 8, cornerHeight));// Top left corner not rounded
            path.addRect(QRectF(fillBottleWidth - 4, top, 8, cornerHeight));// Top right corner not rounded
        }

        Painter.drawPath(path);
    }
    else if(STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup)
    {
        fillParaffinbathWidth = m_ParaffinbathBoundingRectWidth - 9;
        fillParaffinbathHeight = m_ParaffinbathBoundingRectHeight - m_ParaffinbathCoverHeight;
        if (m_ContainerStatusType == DataManager::CONTAINER_STATUS_SCUKING
                || m_ContainerStatusType == DataManager::CONTAINER_STATUS_DRAINING
                || m_ContainerStatusType == DataManager::CONTAINER_STATUS_EMPTY)
        {
            fillParaffinbathHeight = m_CurrentBoundingRectReagentHeight;
        }
        else if (m_ContainerStatusType == DataManager::CONTAINER_STATUS_FULL)
        {
            m_CurrentBoundingRectReagentHeight = fillParaffinbathHeight;
        }

        qreal top = m_ParaffinbathBoundingRectHeight - fillParaffinbathHeight;
        QPainterPath path;
        path.setFillRule( Qt::WindingFill);
        path.addRoundedRect(QRectF(4,  top, fillParaffinbathWidth,
                                  fillParaffinbathHeight -3), 6, 4);
        int cornerHeight = 8;
        if (fillParaffinbathHeight > cornerHeight)
        {
            path.addRect(QRectF(4, top, 8, cornerHeight));// Top left corner not rounded
            path.addRect(QRectF(fillParaffinbathWidth - 4, top, 8, cornerHeight));// Top right corner not rounded
        }
        Painter.drawPath(path);  // Only the Bottome Left and Bottom Right Corner

    }
    else if(STATIONS_GROUP_RETORT == m_DashboardStationGroup)
    {
        fillRetortHeight = m_CurrentBoundingRectReagentHeight - 6;
        fillRetortWidth =  m_RetortBoundingRectWidth;

        QPainterPath path;
        path.setFillRule( Qt::WindingFill);
        if (m_IsRetortContaminated)//Retort CONTAMINATED
        {
            //draw two virtical line align to the wall of container
            path.addRect(4, m_RetortCoverHeight, 3, m_RetortBoundingRectHeight - m_RetortCoverHeight - 3);
            path.addRect(fillRetortWidth - 5, m_RetortCoverHeight, 3, m_RetortBoundingRectHeight - m_RetortCoverHeight - 3);
        }

        if (fillRetortHeight > 0) {
            qreal h = m_RetortBoundingRectHeight - fillRetortHeight - 1;
            path.addRoundedRect(QRectF(4, (h > 0) ? h : 0 , fillRetortWidth - 6, fillRetortHeight -1), 8, 8);
        }

        int cornerHeight = 10;
        if (fillRetortHeight > cornerHeight)
        {
            path.addRect(QRectF(4, m_RetortBoundingRectHeight - fillRetortHeight - 1, 8, cornerHeight));// Top left corner not rounded
            path.addRect(QRectF(fillRetortWidth - 10, m_RetortBoundingRectHeight - fillRetortHeight - 1, 8, cornerHeight));// Top right corner not rounded
        }
        Painter.drawPath(path);  // Only the Bottome Left and Bottom Right Corner
    }
}


void CDashboardStationItem::UpdateUserSettings()
{
    m_UserSettings = *(mp_DataConnector->SettingsInterface->GetUserSettings());
    UpdateDashboardStationItemReagent(m_bRefreshDashboard);
    m_bRefreshDashboard = false;
}

void CDashboardStationItem::SuckDrain(bool isStart, bool isSuck, const QString& ReagentColorValue)
{
    if (isSuck)
        m_ContainerStatusType = DataManager::CONTAINER_STATUS_SCUKING;
    else
        m_ContainerStatusType = DataManager::CONTAINER_STATUS_DRAINING;

    m_CurrentReagentColorValue = ReagentColorValue;

    if (m_StationItemID == "Retort")
    {
        if (isSuck)
            m_CurrentBoundingRectReagentHeight = 0;
        else
            m_CurrentBoundingRectReagentHeight = m_RetortBoundingRectHeight - m_RetortCoverHeight;
    }
    else
    {
        if (!isSuck)//draining
            m_CurrentBoundingRectReagentHeight = 0;
    }

    if (isStart)
    {
        mp_SuckDrainTimer->start();
    }
    else //finished
    {
        mp_SuckDrainTimer->stop();
        OnCompletedSuckDrain(isSuck);
    }
}

void CDashboardStationItem::PauseSuckDrain()
{
    mp_SuckDrainTimer->stop();
}

void CDashboardStationItem::OnCompletedSuckDrain(bool isSuck)
{
    if (m_StationItemID == "Retort")
    {
        if (isSuck)
            m_CurrentBoundingRectReagentHeight = m_RetortBoundingRectHeight - m_RetortCoverHeight;
        else
            m_CurrentBoundingRectReagentHeight = 0;
    }
    else
    {

        if(STATIONS_GROUP_BOTTLE == m_DashboardStationGroup)
        {
            m_CurrentBoundingRectReagentHeight = m_BottleBoundingRectHeight - m_BottleCoverHeight;
        }
        else if (STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup)
        {
            m_CurrentBoundingRectReagentHeight = m_ParaffinbathBoundingRectHeight - m_ParaffinbathCoverHeight;
        }

        if (isSuck)
            m_CurrentBoundingRectReagentHeight = 0;
    }

    DrawStationItemImage();
}

void CDashboardStationItem::SuckDrainAnimation()
{
    if (m_ContainerStatusType == DataManager::CONTAINER_STATUS_SCUKING)
    {
        if (m_StationItemID == "Retort")
        {
            if (m_CurrentBoundingRectReagentHeight <= m_RetortBoundingRectHeight - m_RetortCoverHeight)
            {
                qreal increasedRate = (m_RetortBoundingRectHeight - m_RetortCoverHeight) / 45.0; //45 seconds for filling
                m_CurrentBoundingRectReagentHeight = m_CurrentBoundingRectReagentHeight + increasedRate;
            }
        }
        else //other container
        {
            if (m_CurrentBoundingRectReagentHeight > 0)
            {
                int Containerlength = 0;
                if (STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup)
                    Containerlength = m_ParaffinbathBoundingRectHeight - m_ParaffinbathCoverHeight;
                else
                    Containerlength = m_BottleBoundingRectHeight - m_BottleCoverHeight;

                qreal increasedRate = Containerlength / 45.0;
                m_CurrentBoundingRectReagentHeight = m_CurrentBoundingRectReagentHeight - increasedRate;
            }
        }
    }
    else if (m_ContainerStatusType == DataManager::CONTAINER_STATUS_DRAINING)//drain
    {
        if (m_StationItemID == "Retort")
        {
            if (m_CurrentBoundingRectReagentHeight > 0)
            {
                qreal increasedRate = (m_RetortBoundingRectHeight - m_RetortCoverHeight) / 43.0; //43 seconds for draining
                m_CurrentBoundingRectReagentHeight = m_CurrentBoundingRectReagentHeight - increasedRate;
            }
        }
        else
        {
            int currentLiquidLevel(0);
            if(STATIONS_GROUP_BOTTLE == m_DashboardStationGroup)
            {
                currentLiquidLevel = m_BottleBoundingRectHeight - m_BottleCoverHeight;
            }
            else if(STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup)
            {
                currentLiquidLevel = m_ParaffinbathBoundingRectHeight - m_ParaffinbathCoverHeight;// stop animation when touch to current LiquidLevel
            }

            if (m_CurrentBoundingRectReagentHeight < currentLiquidLevel)
            {
                int Containerlength = 0;
                if (STATIONS_GROUP_PARAFFINBATH == m_DashboardStationGroup)
                    Containerlength = m_ParaffinbathBoundingRectHeight - m_ParaffinbathCoverHeight;
                else
                    Containerlength = m_BottleBoundingRectHeight - m_BottleCoverHeight;

                qreal increasedRate = Containerlength / 43.0;
                m_CurrentBoundingRectReagentHeight = m_CurrentBoundingRectReagentHeight + increasedRate;
            }
        }
    }

    DrawStationItemImage();
}

const QString& CDashboardStationItem::GetStationItemID() const
{
    return m_StationItemID;
}

bool CDashboardStationItem::IsReagentExpired()
{
    return m_ReagentExpiredFlag;
}

DataManager::ContainerStatusType_t CDashboardStationItem::GetContainerStatus()
{
    return m_ContainerStatusType;
}

void CDashboardStationItem::SetContainerStatus(DataManager::ContainerStatusType_t containerStatus, const QString& reagentGroupID)
{
    m_CurrentReagentColorValue = reagentGroupID;
    m_ContainerStatusType = containerStatus;
    if (DataManager::CONTAINER_STATUS_CONTAMINATED == containerStatus)
    {
        m_IsRetortContaminated = true;
        m_CurrentBoundingRectReagentHeight  = 3;
    }
    else if (DataManager::CONTAINER_STATUS_EMPTY == containerStatus)
    {
        m_CurrentBoundingRectReagentHeight  = 0;
    }
    else if (DataManager::CONTAINER_STATUS_FULL == containerStatus)
    {
        if (m_StationItemID == "Retort")
            m_CurrentBoundingRectReagentHeight = m_RetortBoundingRectHeight - m_RetortCoverHeight;
    }

    DrawStationItemImage();
    m_IsRetortContaminated = false;
}

void CDashboardStationItem::SetContainerRetortLockedStatus(bool locked)
{
    m_RetortLocked = locked;
    DrawStationItemImage();
}
} // end namespace Core
