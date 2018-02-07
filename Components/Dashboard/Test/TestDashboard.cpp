/****************************************************************************/
/*! \file TestDashboard.cpp
 *
 *  \brief Unit test for Dashboard menu
 *
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-1-27
 *  $Author:    $ Abe Yang
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
#include <Dashboard/Include/DashboardWidget.h>
#include <QObject>

#include <Dashboard/Include/ScreenSaverWidget.h>
#include <Dashboard/Include/SplashWidget.h>
#include <Dashboard/Include/StartPushButton.h>
#include <Dashboard/Include/PausePushButton.h>
#include <Dashboard/Include/ProgramRunningPanelWidget.h>
#include <Dashboard/Include/CassetteNumberInputWidget.h>
#include <Dashboard/Include/ProgramLabel.h>
#include <Dashboard/Include/StackedLabel.h>
#include <Dashboard/Include/ProgramPanelWidget.h>
#include <Dashboard/Include/IconTitlePanelFrame.h>
#include <QVBoxLayout>

#include "DataManager/Containers/UserSettings/Include/UserSettings.h"


namespace Dashboard {

/****************************************************************************/
/**
 * \brief Test class for Dashboard class.
 */
/****************************************************************************/
class CTestDashboard : public QObject {
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
     * \brief Test ProgramWidget object
     */
    /****************************************************************************/
    void utTestDashboard();

}; // end class TestDashboard

/****************************************************************************/
void CTestDashboard::initTestCase() {
}

/****************************************************************************/
void CTestDashboard::init() {
}

/****************************************************************************/
void CTestDashboard::cleanup() {
}

/****************************************************************************/
void CTestDashboard::cleanupTestCase() {
}

/****************************************************************************/
void CTestDashboard::utTestDashboard()
{
    //qDebug() << "utTestDashboard--";
    ScreenSaverWidget *ssw = new ScreenSaverWidget;
    SplashWidget *splash = new SplashWidget;
    splash->accept();
    //splash->ShowModel();
    CStartPushButton *pushBtn = new CStartPushButton;
    pushBtn->setText("push button");
    pushBtn->IsStartStatus(false);

    CPausePushButton *pauseBtn = new CPausePushButton;
    pauseBtn->setText("pause button");

//    CProgramRunningPanelWidget *progRunPanel = new CProgramRunningPanelWidget;
//    progRunPanel->SetUserSettings(new DataManager::CUserSettings());
//    progRunPanel->GetProgramRemainingTime();
//    progRunPanel->GetStepRemainingTime();
//    progRunPanel->GetEndDateTime();
//    progRunPanel->EnableProgramDetailButton(true);;
//    progRunPanel->EnableProgramDetailButton(false);
    CCassetteNumberInputWidget *inputWidget = new CCassetteNumberInputWidget;
    inputWidget->accept();
    inputWidget->CassetteNumber();

    CProgramLabel *progLabel = new CProgramLabel;
    progLabel->setText("program label");
    progLabel->setText("program label", true);

    CStackedLabel *stackedLabel = new CStackedLabel();
    stackedLabel->setText("StackLabel", true);
    stackedLabel->setText("StackLabel", false);
    stackedLabel->setHighlight(true);
    stackedLabel->setHighlight(false);
    stackedLabel->IsTextHighlight();
//    CProgramPanelWidget *progPanel = new CProgramPanelWidget;
//    progPanel->ChangeStartButtonToStartState();
//    progPanel->ChangeStartButtonToStopState();
//    progPanel->EnablePauseButton(true);
//    progPanel->EnablePauseButton(false);
//    progPanel->EnableStartButton(true);
//    progPanel->EnableStartButton(false);
//    progPanel->IsResumeRun(true);
//    progPanel->IsResumeRun(false);
//    progPanel->IsResumeRun();
//    progPanel->IsAbortEnabled();

//    progPanel->ResetFocus(true);
//    progPanel->ResetFocus(false);
//    progPanel->RequestAsapDateTime();
//    progPanel->SendAsapDateTime(1);
//    progPanel->AddItemsToFavoritePanel(true);
//    progPanel->AddItemsToFavoritePanel(false);
//    progPanel->PrepareSelectedProgramChecking("", false);
//    progPanel->PrepareSelectedProgramChecking("", true);
//    progPanel->OnSelectEndDateTime(QDateTime());
//    progPanel->ProgramSelected("", 1, false, QList<QString>());
//    progPanel->ProgramSelected("", 1, true, QList<QString>());
//    progPanel->ProgramActionStopped(0, 0, QDateTime(), true);
//    progPanel->ProgramActionStopped(0, 0, QDateTime(), false);
//    progPanel->UndoProgramSelection();
//    progPanel->CheckPreConditionsToAbortProgram();
//    progPanel->OnProgramStartReadyUpdated();

    CIconTitlePanelFrame *titlePanel = new CIconTitlePanelFrame;
    titlePanel->SetPanelTitle(QString(""));
    titlePanel->SetPanelTitle(QString(""), QString(""));
    titlePanel->SetTitleCenter();
    titlePanel->SetContent(new QVBoxLayout());
    titlePanel->GetContentFrame();
    titlePanel->SetPanelIcon(QString(""));

}

} // end namespace Dashboard

QTEST_MAIN(Dashboard::CTestDashboard)
#include "TestDashboard.moc"

