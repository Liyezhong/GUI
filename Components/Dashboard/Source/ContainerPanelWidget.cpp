/****************************************************************************/
/*! \file Dashboard/Source/ContainerPanelWidget.cpp
 *
 *  \brief Implementation file for class ContainerPanelWidget.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2013-01-03, 08-Feb-2013
 *  $Author:    $ Swati Tiwari, Santhosha Kumar Raju
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
#include "Global/Include/Utils.h"
#include "Dashboard/Include/ContainerPanelWidget.h"
#include "ui_ContainerPanelWidget.h"
#include "MainMenu/Include/SliderControl.h"
#include "Dashboard/Include/DashboardProgramStatusWidget.h"
#include "Dashboard/Include/CassetteNumberInputWidget.h"
#include "Dashboard/Include/CommonString.h"
#include "Dashboard/Include/FavoriteProgramsPanelWidget.h"

namespace Dashboard {

QString CContainerPanelWidget::m_strWarning = CContainerPanelWidget::tr("Warning");
QString CContainerPanelWidget::m_strYes = CContainerPanelWidget::tr("Yes");
QString CContainerPanelWidget::m_strCancel = CContainerPanelWidget::tr("Cancel");

CContainerPanelWidget::CContainerPanelWidget(QWidget *p_Parent): MainMenu::CPanelFrame(p_Parent),
                                   mp_Ui(new Ui::CContainerPanelWidget),mp_MainWindow(NULL),
                                   mp_DataConnector(NULL),
                                   mp_DashboardScene(NULL),
                                   mp_ProgramList(NULL),
                                   m_UserRoleChanged(false),
                                   mp_MessageDlg(NULL),
                                   m_strProgram(tr("Program")),
                                   m_strInformation(tr("Information")),
                                   m_strOK(tr("OK")),
                                   m_strNo(tr("No")),
                                   m_pUserSetting(NULL)
{
     mp_Ui->setupUi(GetContentFrame());
     SetPanelTitle(QApplication::translate("Dashboard::CContainerPanelWidget", "Dashboard", Q_NULLPTR, -1));
     QPalette Palette = mp_Ui->dashboardView->palette();
     QColor BaseColor = Palette.color(QPalette::Base);
     BaseColor.setAlpha(0);
     Palette.setColor(QPalette::Base, BaseColor);
     mp_Ui->dashboardView->setPalette(Palette);
}


CContainerPanelWidget::~CContainerPanelWidget()
{
    try {
        delete mp_DashboardScene;
        delete mp_MessageDlg;
        delete mp_Ui;
    } catch(...) {

    }
}

void  CContainerPanelWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector)
{
    mp_MainWindow = p_MainWindow;
    mp_DataConnector = p_DataConnector;
    this->Initialize();
}

void CContainerPanelWidget::Initialize()
{
    mp_DashboardScene = new CDashboardScene(mp_DataConnector, this, mp_MainWindow);
    mp_Ui->dashboardView->setScene(mp_DashboardScene);
    //mp_Ui->dashboardView->setRenderHint(QPainter::Antialiasing);
    m_pUserSetting = mp_DataConnector->SettingsInterface->GetUserSettings();

    mp_ProgramList = mp_DataConnector->ProgramList;

    mp_MessageDlg = new MainMenu::CMessageDlg();


    CONNECTSIGNALSLOT(this, ProgramSelected(QString&, QList<QString>&),
                       mp_DashboardScene, UpdateDashboardSceneReagentsForProgram(QString&, QList<QString>&));

    CONNECTSIGNALSLOT(mp_DataConnector, RetortLockStatusChanged(const MsgClasses::CmdLockStatus &),
                      this, OnRetortLockStatusChanged(const MsgClasses::CmdLockStatus&));

    CONNECTSIGNALSLOT(mp_DataConnector, StationSuckDrain(const MsgClasses::CmdStationSuckDrain &),
                      this, OnStationSuckDrain(const MsgClasses::CmdStationSuckDrain &));
    CONNECTSIGNALSLOT(this, OnInteractStart(), mp_DashboardScene, OnInteractStart());
}

void CContainerPanelWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            CommonString::RetranslateUIString();
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

void CContainerPanelWidget::RetranslateUI()
{
    SetPanelTitle(QApplication::translate("Dashboard::CContainerPanelWidget", "Dashboard", Q_NULLPTR, -1));
    m_strYes = QApplication::translate("Dashboard::CContainerPanelWidget", "Yes", Q_NULLPTR, -1);
    m_strNo = QApplication::translate("Dashboard::CContainerPanelWidget", "No", Q_NULLPTR, -1);
    m_strOK = QApplication::translate("Dashboard::CContainerPanelWidget", "OK", Q_NULLPTR, -1);
    m_strCancel = QApplication::translate("Dashboard::CContainerPanelWidget", "Cancel", Q_NULLPTR, -1);
    m_strWarning = QApplication::translate("Dashboard::CContainerPanelWidget", "Warning", Q_NULLPTR, -1);
    m_strProgram = QApplication::translate("Dashboard::CContainerPanelWidget", "Program", Q_NULLPTR, -1);
    m_strInformation = QApplication::translate("Dashboard::CContainerPanelWidget", "Information", Q_NULLPTR, -1);
}

void CContainerPanelWidget::UpdateRetortStatus(DataManager::ContainerStatusType_t retortStatusType,
                                               const QString& reagentGroupID,
                                               const QString& stationID)
{
    mp_DashboardScene->UpdateRetortStatus(retortStatusType, reagentGroupID, stationID);
}

void CContainerPanelWidget::OnRetortLockStatusChanged(const MsgClasses::CmdLockStatus& cmd)
{
    if (cmd.IsLocked())
    {
		mp_DashboardScene->UpdateRetortLockedStatus(true);
    }
    else
    {
		mp_DashboardScene->UpdateRetortLockedStatus(false);
    }
}


void CContainerPanelWidget::OnStationSuckDrain(const MsgClasses::CmdStationSuckDrain & cmd)
{
    mp_DashboardScene->OnStationSuckDrain(cmd.StationID(), cmd.IsStart(), cmd.IsSuck());
}

} // End of namespace Dashboard
