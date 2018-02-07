
/****************************************************************************/
/*! \file CTestSettings.cpp
 *
 *  \brief Unit test for Reagents menu
 *
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-07-21
 *  $Author:    $ Jeff Chen
 *
 *  \b Company:
 *
 *       Leica Biosystems Shanghai
 *
 *  (C) Copyright 2010 by Leica Biosystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <QtTest>
#include <QDebug>
#include <QObject>
#include <Core/Include/DataConnector.h>
#include <KeyBoard/Include/KeyBoard.h>
#include <Settings/Include/AlarmSettingsDlg.h>
#include <Settings/Include/AlarmSettingsWidget.h>
#include <Settings/Include/DataManagementWidget.h>
#include <Settings/Include/DateTimeWidget.h>
#include <QDataStream>
#include <Settings/Include/FileViewWidget.h>
#include <Settings/Include/InstallationSettingsWidget.h>
#include <Settings/Include/LanguageModel.h>
#include <Settings/Include/LanguageWidget.h>
#include <QModelIndex>
#include <Settings/Include/NetworkSettingsWidget.h>
#include <Settings/Include/ParametersWidget.h>
#include <Settings/Include/RegionalSettingsWidget.h>
#include <Settings/Include/ServiceSettingsWidget.h>
#include <Settings/Include/SettingsWidget.h>
#include <Settings/Include/SystemSetupSettingsWidget.h>


namespace Settings {

/****************************************************************************/
/**
 * \brief Test class for Reagents class.
 */
/****************************************************************************/
class CTestSettings : public QObject {

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
    void utTestSettingsWidget();

};
/****************************************************************************/
void CTestSettings::initTestCase() {
}

/****************************************************************************/
void CTestSettings::init() {
}

/****************************************************************************/
void CTestSettings::cleanup() {
}

/****************************************************************************/
void CTestSettings::cleanupTestCase() {
}

/****************************************************************************/
void CTestSettings::utTestSettingsWidget() {

    // NOTE: instance for parameter will no delete for test;
    //
    MainMenu::CMainWindow MainWindow;
    //note: do not remvoe the following 2 lines;
    Core::CDataConnector *p_DataConnector = new Core::CDataConnector(&MainWindow);
    KeyBoard::CKeyBoard KeyBoard;
    Q_UNUSED(KeyBoard);
    //--

    //*//
    CAlarmSettingsDlg *alarmSetDlg = new CAlarmSettingsDlg(CAlarmSettingsDlg::Warning);
    alarmSetDlg->UpdateDisplay();
    alarmSetDlg->SetPtrToMainWindow(&MainWindow);
    alarmSetDlg->SetUserSettings(new DataManager::CHimalayaUserSettings);
    alarmSetDlg->SetDialogType(CAlarmSettingsDlg::Warning);

    alarmSetDlg->changeEvent(new QEvent(QEvent::None));
    alarmSetDlg->showEvent(new QShowEvent);
    alarmSetDlg->OnPeriodicChanged(MainMenu::CSliderControl::PosLeft);
    alarmSetDlg->OnProcessStateChanged();
    alarmSetDlg->OnApply();
    alarmSetDlg->OnPlayTone();
    DataManager::CUserSettings us = DataManager::CUserSettings();
    alarmSetDlg->AlarmSettingsChanged(us);
    alarmSetDlg->PlayTestTone(0, 0, Global::ALARM_WARNING);
    alarmSetDlg->PlayTestTone(0, 0, Global::ALARM_ERROR);
    delete alarmSetDlg;

    //------
    CAlarmSettingsWidget *alarmSetWgt = new CAlarmSettingsWidget;
    alarmSetWgt->SetUserSettings(new DataManager::CHimalayaUserSettings);
    alarmSetWgt->SetPtrToMainWindow(&MainWindow);
    alarmSetWgt->SetDataConnector(p_DataConnector);
    alarmSetWgt->RetranslateUI();
    alarmSetWgt->ResetButtons();
    alarmSetWgt->OnNoteEdit();
    alarmSetWgt->OnWarningEdit();
    alarmSetWgt->OnErrorEdit();
    alarmSetWgt->OnProcessStateChanged();
    alarmSetWgt->UpdateLabels();
    alarmSetWgt->AlarmSettingsChanged(us);
    alarmSetWgt->PlayTestTone(0, 0, Global::ALARM_WARNING);
    alarmSetWgt->PlayTestTone(0, 0, Global::ALARM_ERROR);
    delete alarmSetWgt;
    //---

    CDataManagementWidget *dataMngWgt = new CDataManagementWidget;
    dataMngWgt->SetPtrToMainWindow(&MainWindow);
    dataMngWgt->RetranslateUI();
    dataMngWgt->ResetButtons();
    dataMngWgt->ServiceExportDialog();
    dataMngWgt->SaveToUSBDialog();
    dataMngWgt->ImportDialog();
    dataMngWgt->SWUpdate();
    dataMngWgt->RemoteSWUpdate();
    //dataMngWgt->DisplaySelectionDialog(QStringList());
    dataMngWgt->SetRemoteSWButtonState(false);
    dataMngWgt->SetRemoteSWButtonState(true);
    QDataStream ds;
    dataMngWgt->OnUserLevelChanged(ds);
    dataMngWgt->ExecSending(QString(), QStringList());
    dataMngWgt->EmitSWUpdate(false);
    dataMngWgt->EmitSWUpdate(true);
    dataMngWgt->SelectedImportFileList(QStringList());
    dataMngWgt->UpdateSoftwareFromRC();
    delete dataMngWgt;

    //-----
    CDateTimeWidget *dateWgt = new CDateTimeWidget;
    dateWgt->SetUserSettings(new DataManager::CHimalayaUserSettings);
    dateWgt->SetPtrToMainWindow(&MainWindow);
    dateWgt->GetContent();
    dateWgt->showEvent(new QShowEvent);
    dateWgt->changeEvent(new QEvent(QEvent::None));
    dateWgt->RetranslateUI();
    delete dateWgt;

    //-----
    CFileViewWidget *fileViewWgt = new CFileViewWidget;
    fileViewWgt->SetPanelSize(0, 0);
    fileViewWgt->DisableButton();
    fileViewWgt->RetranslateUI();
    fileViewWgt->changeEvent(new QEvent(QEvent::None));
    fileViewWgt->SelectedLogFile("");
    //QDataStream ds2;
    //fileViewWgt->DayRunLogFileContent(ds2);
    QStringList strList;
    fileViewWgt->DayRunLogFileNames(strList);
    delete fileViewWgt;

    //------
    CInstallationSettingsWidget *instSetWgt = new CInstallationSettingsWidget;
    instSetWgt->ConnectKeyBoardSignalSlots();
    instSetWgt->DisconnectKeyBoardSignalSlots();
    instSetWgt->SetUserSettings(new DataManager::CHimalayaUserSettings);
    instSetWgt->SetPtrToMainWindow(&MainWindow);
    instSetWgt->SetKeyBoardInstance(&KeyBoard);
    instSetWgt->RetranslateUI();
    instSetWgt->ResetButtons();
    instSetWgt->changeEvent(new QEvent(QEvent::None));
    //instSetWgt->showEvent(new QShowEvent);
    instSetWgt->OnEditclicked();
    //instSetWgt->OnOkClicked("");
    instSetWgt->OnESCClicked();
    delete instSetWgt;

    //-----
    CLanguageModel *langMdl = new CLanguageModel;
    QModelIndex index;
    langMdl->rowCount(index);
    langMdl->columnCount(index);
    //langMdl->data(index, 0);
    langMdl->flags(index);
    langMdl->headerData(0, Qt::Vertical);
    langMdl->SetVisibleRowCount(0);
    QStringList lst;
    langMdl->SetLanguageList(lst);
    delete langMdl;
    //---

    //------
    CLanguageWidget *langWgt = new CLanguageWidget;
    langWgt->SetPtrToMainWindow(&MainWindow, p_DataConnector);
    QStringList langLst;
    langWgt->SetLanguages(langLst);
    langWgt->PopulateLanguageList();
    langWgt->RetranslateUI();
    langWgt->ResetButtons();
    langWgt->InitDialog();

    langWgt->changeEvent(new QEvent(QEvent::None));
    langWgt->showEvent(new QShowEvent);
    langWgt->OnBtnApplyClicked();
    langWgt->OnProcessStateChanged();
    //langWgt->SelectionChanged(QModelIndex());
    DataManager::CUserSettings us2;
    langWgt->UserSettingsChanged(us2);
    delete langWgt;
    //---

    CNetworkSettingsWidget *netwkSetWgt = new CNetworkSettingsWidget;
    netwkSetWgt->SetPtrToMainWindow(&MainWindow);
    netwkSetWgt->SetKeyBoardInstance(&KeyBoard);
    netwkSetWgt->RetranslateUI();
    netwkSetWgt->ResetButtons();
    netwkSetWgt->ConnectKeyBoardSignalSlots();
    netwkSetWgt->DisconnectKeyBoardSignalSlots();
    netwkSetWgt->changeEvent(new QEvent(QEvent::None));
    netwkSetWgt->showEvent(new QShowEvent);
    netwkSetWgt->SetUserSettings(new DataManager::CHimalayaUserSettings);
    netwkSetWgt->OnOkClicked("");
    netwkSetWgt->OnESCClicked();
    netwkSetWgt->OnProcessStateChanged();
    netwkSetWgt->OnDirectConnectionStateChanged(0);
    netwkSetWgt->OnProxyUserName();
    netwkSetWgt->OnProxyPassword();
    netwkSetWgt->OnProxyIPAddress();
    netwkSetWgt->OnProxyPort();
    netwkSetWgt->OnSave();
    DataManager::CUserSettings us3;
    netwkSetWgt->SettingsChanged(us3);
    delete netwkSetWgt;


    CParametersWidget *paramWgt = new CParametersWidget;
    paramWgt->RetranslateUI();
    paramWgt->changeEvent(new QEvent(QEvent::None));
    paramWgt->CurrentRowChanged(0);
    delete paramWgt;


    CRegionalSettingsWidget *rgnSetWgt = new CRegionalSettingsWidget;
    rgnSetWgt->RetranslateUI();
    rgnSetWgt->ResetButtons();
    rgnSetWgt->SetUserSettings(new DataManager::CHimalayaUserSettings);
    rgnSetWgt->SetPtrToMainWindow(&MainWindow);
    rgnSetWgt->changeEvent(new QEvent(QEvent::None));
    rgnSetWgt->showEvent(new QShowEvent);
    rgnSetWgt->OnApply();
    rgnSetWgt->OnProcessStateChanged();
    DataManager::CUserSettings us4;
    rgnSetWgt->RegionalSettingsChanged(us4);
    delete rgnSetWgt;


    //-----------
    CServiceSettingsWidget *srvSetWgt = new CServiceSettingsWidget;
    DataManager::CHimalayaUserSettings us5;
    srvSetWgt->SetUserSettings(&us5);
    srvSetWgt->RetranslateUI();
    srvSetWgt->ResetButtons();
    srvSetWgt->GetFormattedCurrentDateString();
    QDateTime date;
    srvSetWgt->GetFormattedDateString(date);
    srvSetWgt->changeEvent(new QEvent(QEvent::None));
    srvSetWgt->showEvent(new QShowEvent);
    srvSetWgt->OnProcessStateChanged();
    srvSetWgt->OnUserRoleChanged();
    srvSetWgt->OnResetOperationDays();
    srvSetWgt->OnResetCarbonFilter();
    srvSetWgt->OnSaveSetting();
    //srvSetWgt->OnPrepareShutdown();
    srvSetWgt->OnStartServiceApp();
    DataManager::CUserSettings us6;
    srvSetWgt->ServiceSettingsChanged(us6);
    srvSetWgt->ResetOperationDays(DataManager::RESETOPERATIONHOURS_UNDEFINED);
    srvSetWgt->AppQuitSystemPrepareShutdown(DataManager::QUITAPPSHUTDOWNACTIONTYPE_UNDEFINED);
    delete srvSetWgt;



    CSettingsWidget *setWgt = new CSettingsWidget(p_DataConnector, &MainWindow, &KeyBoard);
    //setWgt->PropagateDateTime();
    setWgt->UserSettingsUpdated();
    setWgt->PanelSelected(0);
    setWgt->UpdateLanguages();

    DataManager::CUserSettings us7;
    setWgt->SettingsChanged(us7);
    QDataStream ds7;
    setWgt->UserLevelChanged(ds7);
    delete setWgt;

    CSystemSetupSettingsWidget *sysSetupWgt = new CSystemSetupSettingsWidget;
    sysSetupWgt->SetUserSettings(new DataManager::CHimalayaUserSettings);
    sysSetupWgt->SetPtrToMainWindow(&MainWindow);
    sysSetupWgt->InitTemperatureWidget();
    sysSetupWgt->RetranslateUI();
    sysSetupWgt->ResetButtons();
    sysSetupWgt->changeEvent(new QEvent(QEvent::None));
    //sysSetupWgt->showEvent(new QShowEvent);
    sysSetupWgt->OnProcessStateChanged();
    sysSetupWgt->OnUserRoleChanged();
    //sysSetupWgt->OnApply();
    DataManager::CUserSettings us8;
    sysSetupWgt->TemperatureChanged(us8);
    delete sysSetupWgt;

    //*/
 }

} // end namespace Programs

QTEST_MAIN(Settings::CTestSettings)
#include "TestSettings.moc"

