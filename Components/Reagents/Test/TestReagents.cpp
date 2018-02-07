
/****************************************************************************/
/*! \file CTestReagents.cpp
 *
 *  \brief Unit test for Reagents menu
 *
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-02
 *  $Author:    $ C.Adaragunchi
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

#include <QtTest>
#include <QDebug>
#include <QObject>
#include <QStringList>
#include <QDataStream>
#include <Reagents/Include/ReagentWidget.h>
#include <Reagents/Include/ModifyReagentGroupColorDlg.h>
#include <Reagents/Include/ModifyReagentRMSDlg.h>
#include <Reagents/Include/ModifyReagentStationDlg.h>
#include <Reagents/Include/ReagentGroupWidget.h>
#include <Reagents/Include/ReagentRMSWidget.h>
#include <Reagents/Include/ReagentStatusWidget.h>
#include <Reagents/Include/ReagentSubMenuWidget.h>



namespace Reagents {

/****************************************************************************/
/**
 * \brief Test class for Reagents class.
 */
/****************************************************************************/
class CTestReagents : public QObject {
    Q_OBJECT
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();

    /****************************************************************************/
    /**
     * \brief Test ReagentWidget object
     */
    /****************************************************************************/
    void utTestReagentWidget();

}; // end class TestReagents

/****************************************************************************/
void CTestReagents::initTestCase() {
}

/****************************************************************************/
void CTestReagents::init() {
}

/****************************************************************************/
void CTestReagents::cleanup() {
}

/****************************************************************************/
void CTestReagents::cleanupTestCase() {
}

/****************************************************************************/
void CTestReagents::utTestReagentWidget() {

    MainMenu::CMainWindow MainWindow;
    Core::CDataConnector *p_DataConnector = new Core::CDataConnector(&MainWindow);
    KeyBoard::CKeyBoard KeyBoard;

    //*/
    Reagents::CReagentWidget *ragtWgt = new Reagents::CReagentWidget(p_DataConnector, &MainWindow, &KeyBoard);
    ragtWgt->changeEvent(new QEvent(QEvent::None));
    //ragtWgt->SetUserSettings(new DataManager::CUserSettings);
    ragtWgt->RetranslateUI();
    DataManager::CUserSettings us;
    ragtWgt->SettingsChanged(us);
    ragtWgt->RMSValueChanged(Global::RMS_DAYS);
    ragtWgt->UnselectProgram();
    QStringList lst;
    ragtWgt->UpdateSelectedStationList(lst);
    ragtWgt->OnUserRoleChanged();
    ragtWgt->PanelSelected(0);
    delete ragtWgt;


    CModifyReagentGroupColorDlg *mdyColorDlg = new CModifyReagentGroupColorDlg;
    DataManager::CDataReagentGroupList ragtLst;
    DataManager::CReagentGroup ragtGrp;

    mdyColorDlg->SetReagentGroupList(ragtLst, ragtGrp);
    mdyColorDlg->SetReagentGroupColorListptr(new DataManager::CReagentGroupColorList);
    mdyColorDlg->OnButtonGroup(0);
    mdyColorDlg->OnProcessStateChanged();
    //mdyColorDlg->UpdateReagentGroupColor();
    mdyColorDlg->changeEvent(new QEvent(QEvent::None));
    mdyColorDlg->CancelPressed();
    mdyColorDlg->UpdateReagentGroup(ragtGrp);
    mdyColorDlg->OnCancel();
    mdyColorDlg->OnOk();
    delete mdyColorDlg;


    CModifyReagentRMSDlg * mdyRagtRmsDlg = new CModifyReagentRMSDlg;
    mdyRagtRmsDlg->InitDialog(new DataManager::CReagent,
                              new DataManager::CDataReagentGroupList,
                              Global::RMS_DAYS);

    mdyRagtRmsDlg->UpdateRmsLabel(Global::RMS_DAYS);
    mdyRagtRmsDlg->SetButtonType(COPY_BTN_CLICKED);
    mdyRagtRmsDlg->ResizeHorizontalSection();
    mdyRagtRmsDlg->ShowReagentValue(Global::RMS_DAYS);
    mdyRagtRmsDlg->ConnectKeyBoardSignalSlots();
    mdyRagtRmsDlg->DisconnectKeyBoardSignalSlots();
    mdyRagtRmsDlg->OnESCClicked();
    DataManager::CReagent ragt;
    mdyRagtRmsDlg->UpdateReagent(ragt);
    mdyRagtRmsDlg->AddReagent(ragt);
    mdyRagtRmsDlg->CancelPressed();
    delete mdyRagtRmsDlg;

    CModifyReagentStationDlg * mdyRagtStnDlg = new CModifyReagentStationDlg(NULL, &MainWindow, p_DataConnector);
    mdyRagtStnDlg->SetEditedDashboardStation(new DataManager::CDashboardStation);
    mdyRagtStnDlg->RetranslateUI();
    mdyRagtStnDlg->ResizeHorizontalSection();
    mdyRagtStnDlg->OnOk();
    mdyRagtStnDlg->OnCancel();
    mdyRagtStnDlg->OnProcessStateChanged();
    mdyRagtStnDlg->changeEvent(new QEvent(QEvent::None));
    mdyRagtStnDlg->CancelPressed();
    mdyRagtStnDlg->UpdateStationSetAsEmpty("");
    mdyRagtStnDlg->UpdateStationChangeReagent("", "");
    mdyRagtStnDlg->UnselectProgram();
    delete mdyRagtStnDlg;


    CReagentGroupWidget * ragtGrpWgt = new CReagentGroupWidget;
    ragtGrpWgt->ReagentGroupColorUpdated();
    ragtGrpWgt->UpdateReagentGroupList();
    DataManager::CReagentGroup ragtGrp2;
    ragtGrpWgt->UpdateReagentGroup(ragtGrp2);
    ragtGrpWgt->showEvent(new QShowEvent);
    ragtGrpWgt->changeEvent(new QEvent(QEvent::None));
    ragtGrpWgt->ResizeHorizontalSection();
    ragtGrpWgt->RetranslateUI();
    ragtGrpWgt->ResetButtons();
    ragtGrpWgt->SetUserSettings(new DataManager::CUserSettings);
    ragtGrpWgt->OnEdit();
    ragtGrpWgt->OnUserRoleChanged();
    ragtGrpWgt->OnProcessStateChanged();
    ragtGrpWgt->OnUpdateReagentGroup(ragtGrp2);
    delete ragtGrpWgt;

    CReagentRMSWidget *ragtRmsWgt = new CReagentRMSWidget;
    ragtRmsWgt->SetUserSettings(new DataManager::CUserSettings);

    ragtRmsWgt->SetPtrToMainWindow(p_DataConnector, new DataManager::CDataReagentList,
                                   &MainWindow);

    ragtRmsWgt->PopulateReagentList();
    ragtRmsWgt->ResizeHorizontalSection();
    ragtRmsWgt->ResetButtons();
    ragtRmsWgt->RetranslateUI();
    QString str = QString("");
    ragtRmsWgt->UpdateButtons(str);
    ragtRmsWgt->changeEvent(new QEvent(QEvent::None));
    ragtRmsWgt->showEvent((new QShowEvent));
    ragtRmsWgt->UpdateReagentList();
    DataManager::CReagent ragt2;
    ragtRmsWgt->UpdateReagent(ragt2);
    ragtRmsWgt->AddReagent(ragt);
    //ragtRmsWgt->RemoveReagent("");
    ragtRmsWgt->RMSChanged(Global::RMS_OFF);
    ragtRmsWgt->RMSCleaningChanged(Global::RMS_OFF);
    ragtRmsWgt->OnUserRoleChanged();
    //ragtRmsWgt->OnProcessStateChanged();
    //ragtRmsWgt->CloseDialogs();
    //ragtRmsWgt->OnRMSOFF();
    //ragtRmsWgt->OnRMSCassettes();
    //ragtRmsWgt->OnRMSCycles();
    //ragtRmsWgt->OnRMSDays();
    //ragtRmsWgt->OnCancelPressed();
    //ragtRmsWgt->OnCleaningRMSOFF();
    //ragtRmsWgt->OnCleaningRMSCycles();
    //ragtRmsWgt->OnCleaningRMSDays();
    ragtRmsWgt->UpdateUserSetting();
    delete ragtRmsWgt;

    CReagentStatusWidget *ragtStatWgt = new CReagentStatusWidget;
    ragtStatWgt->showEvent(new QShowEvent);
    ragtStatWgt->changeEvent(new QEvent(QEvent::None));
    ragtStatWgt->PopulateReagentList();
    ragtStatWgt->ResizeHorizontalSection();
    ragtStatWgt->RetranslateUI();
    ragtStatWgt->ResetButtons();
    ragtStatWgt->ControlColumnShow();
    ragtStatWgt->IsReagentExpired();
    ragtStatWgt->SetUserSettings(new DataManager::CUserSettings);
    ragtStatWgt->SetPtrToMainWindow(p_DataConnector, new DataManager::CDataReagentList);
//    ragtStatWgt->OnSetAsEmpty();
//    ragtStatWgt->OnSetAsFull();
//    ragtStatWgt->OnResetData();
    ragtStatWgt->OnProcessStateChanged();
    ragtStatWgt->OnUserRoleChanged();
    ragtStatWgt->StationReagentUpdated("");
    ragtStatWgt->RMSChanged(Global::RMS_DAYS);
    ragtStatWgt->RMSCleaningChanged(Global::RMS_DAYS);
    QList<QString> lst2;
    ragtStatWgt->UpdateSelectedStationList(lst2);
    ragtStatWgt->UpdateReagentList();
    ragtStatWgt->UpdateStationChangeReagent("", "");
    ragtStatWgt->UpdateStationSetAsEmpty("");
    ragtStatWgt->UpdateStationSetAsFull("");
    ragtStatWgt->UpdateStationResetData("");
    ragtStatWgt->UnselectProgram();
    delete ragtStatWgt;



    CReagentSubMenuWidget * ragtSubWgt = new CReagentSubMenuWidget;
    ragtSubWgt->RetranslateUI();
    ragtSubWgt->changeEvent(new QEvent(QEvent::None));
    ragtSubWgt->CurrentRowChanged(0);
    delete ragtSubWgt;

    //*/

 }

} // end namespace Programs

QTEST_MAIN(Reagents::CTestReagents)
#include "TestReagents.moc"

