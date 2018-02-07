/****************************************************************************/
/*! \file BottleCheckStatusDlg.cpp
 *
 *  \brief BottleCheckStatusDlg definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2015-05-20
 *   $Author:  $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2015 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#include "Settings/Include/BottleCheckStatusDlg.h"
#include "ui_BottleCheckStatusDlg.h"
#include "Core/Include/DataConnector.h"
#include "MainMenu/Include/MessageDlg.h"

namespace Settings {
    CBottleCheckStatusDlg::CBottleCheckStatusDlg(QWidget *parent, Core::CDataConnector* pDataConnect) :
        MainMenu::CDialogFrame(parent),
        ui(new Ui::CBottleCheckStatusDlg),
        m_Waiting(tr("Waiting")),m_Empty(tr("Empty")),m_Passed(tr("Passed")),
        m_Blockage(tr("Blockage")),m_Checking(tr("Checking")),
        m_BuildPressureFailed(tr("Build pressure failed")),
        m_Failed(tr("Failed")),
        m_bNoneUpdated(true),
        m_bShouldReportError(false),
        m_strReportMsg(tr("Bottle Check failed! Please check the details in the Bottle Check status list.")),
        m_strWarning(tr("Warning")),
        m_strOK(tr("OK"))
    {
        ui->setupUi(GetContentFrame());
        SetDialogTitle(tr("Bottle check"));

        mp_TableWidget = new MainMenu::CBaseTable;
        mp_TableWidget->setModel(&m_BottleCheckStatusModel);
        ui->bottleCheckStatusTable->SetContent(mp_TableWidget);
        m_BottleCheckStatusModel.SetRequiredContainers(pDataConnect->ReagentList, pDataConnect->DashboardStationList);
        mp_TableWidget->horizontalHeader()->show();
        mp_TableWidget->SetVisibleRows(7);
        m_BottleCheckStatusModel.SetVisibleRowCount(7);
        mp_TableWidget->horizontalHeader()->resizeSection(0, 80);
        mp_TableWidget->horizontalHeader()->resizeSection(1, 160);
        mp_TableWidget->horizontalHeader()->resizeSection(2, 120);
        mp_TableWidget->selectRow(0);
        CONNECTSIGNALSLOT(ui->btnClose, clicked(), this, OnClose());

        (void)m_StationIDRowMap.insert("S1", 0);
        (void)m_StationIDRowMap.insert("S2", 1);
        (void)m_StationIDRowMap.insert("S3", 2);
        (void)m_StationIDRowMap.insert("S4", 3);
        (void)m_StationIDRowMap.insert("S5", 4);
        (void)m_StationIDRowMap.insert("S6", 5);
        (void)m_StationIDRowMap.insert("S7", 6);
        (void)m_StationIDRowMap.insert("S8", 7);
        (void)m_StationIDRowMap.insert("S9", 8);
        (void)m_StationIDRowMap.insert("S10", 9);
        (void)m_StationIDRowMap.insert("S11", 10);
        (void)m_StationIDRowMap.insert("S12", 11);
        (void)m_StationIDRowMap.insert("S13", 12);
        (void)m_StationIDRowMap.insert("P1", 13);
        (void)m_StationIDRowMap.insert("P2", 14);
        (void)m_StationIDRowMap.insert("P3", 15);


        (void)m_RowStationIDMap.insert(0, "S1");
        (void)m_RowStationIDMap.insert(1, "S2");
        (void)m_RowStationIDMap.insert(2, "S3");
        (void)m_RowStationIDMap.insert(3, "S4");
        (void)m_RowStationIDMap.insert(4, "S5");
        (void)m_RowStationIDMap.insert(5, "S6");
        (void)m_RowStationIDMap.insert(6, "S7");
        (void)m_RowStationIDMap.insert(7, "S8");
        (void)m_RowStationIDMap.insert(8, "S9");
        (void)m_RowStationIDMap.insert(9, "S10");
        (void)m_RowStationIDMap.insert(10, "S11");
        (void)m_RowStationIDMap.insert(11, "S12");
        (void)m_RowStationIDMap.insert(12, "S13");
        (void)m_RowStationIDMap.insert(13, "P1");
        (void)m_RowStationIDMap.insert(14, "P2");
        (void)m_RowStationIDMap.insert(15, "P3");

        Inilialize();
        ui->btnClose->setEnabled(false);

    }

    CBottleCheckStatusDlg::~CBottleCheckStatusDlg()
    {
        try {
            delete ui;
            delete mp_TableWidget;
        }
        catch (...) {
            // to please Lint.
        }
    }

    /****************************************************************************/
    /*!
     *  \brief Event handler for change events
     *
     *  \iparam p_Event = Change event
     */
    /****************************************************************************/

    void CBottleCheckStatusDlg::changeEvent(QEvent *p_Event)
    {
        QDialog::changeEvent(p_Event);
        switch (p_Event->type()) {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                RetranslateUI();
                break;
            default:
                break;
        }
    }
    /****************************************************************************/
    /*!
     *  \brief Event handler for key press events
     *
     *  \iparam e = keyboard event
     */
    /****************************************************************************/
    void CBottleCheckStatusDlg::keyPressEvent ( QKeyEvent * e )
    {
        Q_UNUSED(e);
    }
    /****************************************************************************/
    /*!
     *  \brief Translates the strings in UI to the selected language
     */
    /****************************************************************************/
    void CBottleCheckStatusDlg::RetranslateUI()
    {
        (void) m_BottleCheckStatusModel.setHeaderData(0, Qt::Horizontal,QApplication::translate("Settings::CBottleCheckStatusModel",
                                                                                     "Station", Q_NULLPTR, -1),0);

        (void) m_BottleCheckStatusModel.setHeaderData(1, Qt::Horizontal,QApplication::translate("Settings::CBottleCheckStatusModel",
                                                                                     "Reagent", Q_NULLPTR, -1),0);

        (void) m_BottleCheckStatusModel.setHeaderData(2, Qt::Horizontal,QApplication::translate("Settings::CBottleCheckStatusModel",
                                                                                     "Status", Q_NULLPTR, -1),0);
        m_Waiting = QApplication::translate("Settings::CBottleCheckStatusDlg", "Waiting", Q_NULLPTR, -1);
        m_Empty = QApplication::translate("Settings::CBottleCheckStatusDlg", "Empty", Q_NULLPTR, -1);
        m_Passed = QApplication::translate("Settings::CBottleCheckStatusDlg", "Passed", Q_NULLPTR, -1);
        m_Blockage = QApplication::translate("Settings::CBottleCheckStatusDlg", "Blockage", Q_NULLPTR, -1);
        m_Checking = QApplication::translate("Settings::CBottleCheckStatusDlg", "Checking", Q_NULLPTR, -1);
        m_BuildPressureFailed = QApplication::translate("Settings::CBottleCheckStatusDlg", "Build pressure failed", Q_NULLPTR, -1);
        m_Failed = QApplication::translate("Settings::CBottleCheckStatusDlg", "Failed", Q_NULLPTR, -1);
        m_strWarning = QApplication::translate("Settings::CBottleCheckStatusDlg", "Warning", Q_NULLPTR, -1);
        m_strOK = QApplication::translate("Settings::CBottleCheckStatusDlg", "OK", Q_NULLPTR, -1);
        m_strReportMsg = QApplication::translate("Settings::CBottleCheckStatusDlg", "Bottle Check failed! Please check the details in the Bottle Check status list.", Q_NULLPTR, -1);
        QString dlgTitle = QApplication::translate("Settings::CBottleCheckStatusDlg", "Bottle check", Q_NULLPTR, -1);

        SetDialogTitle(dlgTitle);
        Inilialize();
    }

    void CBottleCheckStatusDlg::OnClose()
    {
        (void)this->close();
    }

    void CBottleCheckStatusDlg::Inilialize()
    {
        m_BottleCheckStatusMap.clear();
        (void)m_BottleCheckStatusMap.insert(DataManager::BOTTLECHECK_WAITING, m_Waiting);
        (void)m_BottleCheckStatusMap.insert(DataManager::BOTTLECHECK_EMPTY, m_Empty);
        (void)m_BottleCheckStatusMap.insert(DataManager::BOTTLECHECK_PASSED, m_Passed);
        (void)m_BottleCheckStatusMap.insert(DataManager::BOTTLECHECK_BLOCKAGE, m_Blockage);
        (void)m_BottleCheckStatusMap.insert(DataManager::BOTTLECHECK_CHECKING, m_Checking);
        (void)m_BottleCheckStatusMap.insert(DataManager::BOTTLECHECK_BUILDPRESSUREFAILED, m_BuildPressureFailed);
        (void)m_BottleCheckStatusMap.insert(DataManager::BOTTLECHECK_FAILED, m_Failed);
        (void)m_BottleCheckStatusMap.insert(DataManager::BOTTLECHECK_WILLNOTPROCESS, " --- ");

        m_StationStatusMap.clear();
        (void)m_StationStatusMap.insert("S1", m_Waiting);
        (void)m_StationStatusMap.insert("S2", m_Waiting);
        (void)m_StationStatusMap.insert("S3", m_Waiting);
        (void)m_StationStatusMap.insert("S4", m_Waiting);
        (void)m_StationStatusMap.insert("S5", m_Waiting);
        (void)m_StationStatusMap.insert("S6", m_Waiting);
        (void)m_StationStatusMap.insert("S7", m_Waiting);
        (void)m_StationStatusMap.insert("S8", m_Waiting);

        (void)m_StationStatusMap.insert("S9", m_Waiting);
        (void)m_StationStatusMap.insert("S10", m_Waiting);
        (void)m_StationStatusMap.insert("S11", m_Waiting);
        (void)m_StationStatusMap.insert("S12", m_Waiting);
        (void)m_StationStatusMap.insert("S13", m_Waiting);
        (void)m_StationStatusMap.insert("P1", m_Waiting);
        (void)m_StationStatusMap.insert("P2", m_Waiting);
        (void)m_StationStatusMap.insert("P3", m_Waiting);

        m_BottleCheckStatusModel.SetBottleCheckStatusMap(m_StationStatusMap);
    }

    /****************************************************************************/
    /*!
     *  \brief Update current bottle check status for the specified station
     *  \param stationID = station ID
     *  \param bottleCheckStatusType = status type
     */
    /****************************************************************************/
    void CBottleCheckStatusDlg::UpdateCurrentStationStatus(const QString& stationID, DataManager::BottleCheckStatusType_t bottleCheckStatusType)
    {

        if (DataManager::BOTTLECHECK_ALLCOMPLETE == bottleCheckStatusType || DataManager::BOTTLECHECK_FAILED == bottleCheckStatusType)
        {
            ui->btnClose->setEnabled(true);
            if (DataManager::BOTTLECHECK_FAILED == bottleCheckStatusType)//failed
            {
                UpdateStationNotProcess(stationID);
                m_bNoneUpdated = true;
            }
            else if ((DataManager::BOTTLECHECK_ALLCOMPLETE == bottleCheckStatusType) && m_bNoneUpdated)//completed
            {
                UpdateStationNotProcess(stationID);
                m_bNoneUpdated = true;
                return;
            }
            else if (m_bShouldReportError)
            {
                MainMenu::CMessageDlg messageDlg(this);
                messageDlg.SetIcon(QMessageBox::Warning);
                messageDlg.SetTitle(m_strWarning);
                messageDlg.SetText(m_strReportMsg);
                messageDlg.SetButtonText(1, m_strOK);
                messageDlg.HideButtons();
                (void)messageDlg.exec();
                return;
            }
        }

        if (DataManager::BOTTLECHECK_BUILDPRESSUREFAILED == bottleCheckStatusType ||
            DataManager::BOTTLECHECK_EMPTY == bottleCheckStatusType ||
            DataManager::BOTTLECHECK_BLOCKAGE == bottleCheckStatusType )
        {
            m_bShouldReportError = true;
        }

        m_BottleCheckStatusModel.UpdateStatusData(stationID, m_BottleCheckStatusMap[bottleCheckStatusType],
                                                      m_StationIDRowMap[stationID]);
        if (!stationID.isEmpty())
           m_bNoneUpdated = false;

        QModelIndex index = m_BottleCheckStatusModel.index(m_StationIDRowMap[stationID], 0);
        mp_TableWidget->scrollTo(index);
        emit ui->bottleCheckStatusTable->ScrollRangeChanged();
    }

    void CBottleCheckStatusDlg::UpdateStationNotProcess(const QString& stationID)
    {
        int curRow = m_StationIDRowMap[stationID];
        curRow++;
        for (int r = curRow; r < 16; r++)
        {
            m_BottleCheckStatusModel.UpdateStatusData(m_RowStationIDMap[r], m_BottleCheckStatusMap[DataManager::BOTTLECHECK_WILLNOTPROCESS], r);
        }
    }

}
