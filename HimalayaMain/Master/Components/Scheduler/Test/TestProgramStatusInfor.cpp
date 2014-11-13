/****************************************************************************/
/*! \file TestUserSettings.cpp
 *
 *  \brief Implementation file for class TestProgramStatusInfor.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-09-23
 *  $Author:    $ Logan Yang
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

#include <QTest>
#include "Scheduler/Include/ProgramStatusInfor.h"


namespace Scheduler {


/****************************************************************************/
/**
 * \brief Test class for TestProgramStatusInfor class.
 */
/****************************************************************************/
class TestProgramStatusInfor : public QObject {
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
     * \brief Test the ProgramStatusInfor.
     */
    /****************************************************************************/
    void utTestProgramStatusInfor();
}; // end class TestUserSettings

/****************************************************************************/
void TestProgramStatusInfor::initTestCase() {
    // init languages
    //Global::InitSupportedLanguages();
}

/****************************************************************************/
void TestProgramStatusInfor::init() {
}

/****************************************************************************/
void TestProgramStatusInfor::cleanup() {
}

/****************************************************************************/
void TestProgramStatusInfor::cleanupTestCase() {
}

/****************************************************************************/
void TestProgramStatusInfor::utTestProgramStatusInfor() {
    CProgramStatusInfor ProStatus;
    bool result = false;

    result = ProStatus.InitProgramStatus(54);
    QVERIFY(result);
    ProStatus.SetLastRVPosition(DeviceControl::RV_SEAL_10);
    ProStatus.SetProgramID("L01");
    ProStatus.SetStepID(3);
    ProStatus.SetScenario(4);
    QCOMPARE(ProStatus.GetLastRVPosition(),DeviceControl::RV_SEAL_10);
    QCOMPARE(ProStatus.GetProgramId(),QString("L01"));
    QVERIFY(ProStatus.GetStepID()==3);
    QVERIFY(ProStatus.GetScenario() == 4);

    ProStatus.SetProgramID("");
    QVERIFY(ProStatus.IsProgramFinished());

    ProStatus.SetLastReagentGroup("RG1");
    QCOMPARE(ProStatus.GetLastReagentGroup(), QString("RG1"));

    ProStatus.SetErrorFlag(0);
    QVERIFY(ProStatus.GetErrorFlag()==0);

    quint64 Hour= 60 * 60 * 1000;
    quint64 Hour12 = 12 * Hour;
    quint64 Hour15 = 15 * Hour;
    quint64 CTime = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(ProStatus.GetRemaingTimeForMeltingParffin() == Hour12);
    QVERIFY(ProStatus.GetOvenHeatingTime() == 0);
    ProStatus.ResetOvenHeatingTime(70);
    QVERIFY(ProStatus.GetRemaingTimeForMeltingParffin() == Hour15);
    QVERIFY(ProStatus.GetOvenHeatingTime() == 0);

    ProStatus.UpdateOvenHeatingTime(CTime - Hour,true);
    QVERIFY(ProStatus.GetRemaingTimeForMeltingParffin() == Hour15);
    ProStatus.UpdateOvenHeatingTime(CTime,true);
    QVERIFY(ProStatus.GetOvenHeatingTime() == Hour);
    QVERIFY(ProStatus.GetRemaingTimeForMeltingParffin() == 14 * Hour);

    ProStatus.UpdateOvenHeatingTime(CTime,false);
    QVERIFY(ProStatus.GetRemaingTimeForMeltingParffin() == Hour15);
    QVERIFY(ProStatus.GetOvenHeatingTime() == 0);
}



} // end namespace DataManagement

QTEST_MAIN(Scheduler::TestProgramStatusInfor)

#include "TestProgramStatusInfor.moc"
