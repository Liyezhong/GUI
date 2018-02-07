/****************************************************************************/
/*! \file TestPrograms.cpp
 *
 *  \brief Unit test for Programs menu
 *
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-18
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
#include <Programs/Include/ProgramWidget.h>
#include <Programs/Include/ModifyProgramDlg.h>
#include <Programs/Include/ColorButton.h>
#include <QObject>

namespace Programs {

/****************************************************************************/
/**
 * \brief Test class for Programs class.
 */
/****************************************************************************/
class CTestPrograms : public QObject {
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
    void utTestPrograms();

}; // end class TestPrograms

/****************************************************************************/
void CTestPrograms::initTestCase() {
}

/****************************************************************************/
void CTestPrograms::init() {
}

/****************************************************************************/
void CTestPrograms::cleanup() {
}

/****************************************************************************/
void CTestPrograms::cleanupTestCase() {
}

/****************************************************************************/
void CTestPrograms::utTestPrograms()
{
    MainMenu::CMainWindow MainWindow;   
    KeyBoard::CKeyBoard KeyBoard;

    DataManager::CProgramStep* FirstProgramStep = new DataManager::CProgramStep();
    DataManager::CProgramStep* SecondProgramStep = new DataManager::CProgramStep();
    DataManager::CProgramStep* LeicaProgramStepOne = new DataManager::CProgramStep();
    DataManager::CProgramStep* LeicaProgramStepTwo = new DataManager::CProgramStep();
    DataManager::CProgram Program;

    Core::CDataConnector *p_DataConnector = new Core::CDataConnector(&MainWindow);

   Programs::CProgramWidget *p_ProgramWidget = new
            Programs::CProgramWidget(p_DataConnector, &MainWindow, &KeyBoard);

    Programs::CModifyProgramDlg *p_ModifyProgramDlg = new
            Programs::CModifyProgramDlg(p_ProgramWidget, &KeyBoard, &MainWindow, p_DataConnector);

    Programs::CModifyProgramStepDlg *p_ModifyProgramStepDlg = new
            Programs::CModifyProgramStepDlg(p_ModifyProgramDlg, &MainWindow, p_DataConnector);

    Programs::CColorButton *p_ColorButton = new Programs::CColorButton(p_ProgramWidget);


    DataManager::CDataProgramList ProgramList = *(p_DataConnector->ProgramList);
    DataManager::CDashboardDataStationList StationList = *(p_DataConnector->DashboardStationList);
    DataManager::CDataReagentList ReagentList = *(p_DataConnector->ReagentList);

    //  changed method by setting the ProcessRunning to true in MainWindow
    MainWindow.SetStatusIcons(MainMenu::CMainWindow::ProcessRunning);

    // Set User role to Administrator in MainWindow
    MainWindow.SetUserRole(MainMenu::CMainWindow::Admin);
    const QModelIndex Index;

    //Set Program attributes
    Program.SetID("L1");

    //Test ProgramWidget methods
    p_ProgramWidget->PopulateProgramList();
    p_ProgramWidget->SelectionChanged(Index);
    p_ProgramWidget->OnUserRoleChanged();

    //Set all the ProgramStep parameters for ProgramSteps
    FirstProgramStep->SetStepID("0");
    FirstProgramStep->SetReagentID("S4");


    SecondProgramStep->SetStepID("3");
    SecondProgramStep->SetReagentID("S7");


    LeicaProgramStepOne->SetStepID("1");
    LeicaProgramStepOne->SetReagentID("L1");


    LeicaProgramStepTwo->SetStepID("2");
    LeicaProgramStepTwo->SetReagentID("L2");


    // Adding ProgramSteps to a program
    Program.AddProgramStep(0, FirstProgramStep);
    Program.AddProgramStep(1, LeicaProgramStepOne);
    Program.AddProgramStep(2, LeicaProgramStepTwo);
    Program.AddProgramStep(3, SecondProgramStep);

    //Add Program to ProgramList
    ProgramList.AddProgram(&Program);

    //Test ModifyProgramDlg class member functions
    p_ModifyProgramDlg->InitDialog(&Program);
    p_ModifyProgramDlg->NewProgram();
    p_ModifyProgramDlg->SetRowFocus(1);
    p_ModifyProgramDlg->SetButtonType(EDIT_BTN_CLICKED);
    QCOMPARE(p_ModifyProgramDlg->m_ButtonType, EDIT_BTN_CLICKED);



     //Test ModifyProgramStepDlg class member functions
    p_ModifyProgramStepDlg->SetProgramStep(FirstProgramStep);
    p_ModifyProgramStepDlg->NewProgramStep();
    p_ModifyProgramStepDlg->ResetButtons(false);
    p_ModifyProgramStepDlg->SelectRow(3);


   //Test ColorButton class member functions
    p_ColorButton->SetColor("blue");

}

} // end namespace Programs

QTEST_MAIN(Programs::CTestPrograms)
#include "TestPrograms.moc"

