/****************************************************************************/
/*! \file ModifyReagentStationDlg.cpp
 *
 *  \brief ModifyReagentStatusDlg Implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-29
 *   $Author:  $ Swati Tiwari
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
#include "Global/Include/Exception.h"
#include "Reagents/Include/ModifyReagentStationDlg.h"
#include "ui_ModifyReagentStatusDlg.h"
#include <Dashboard/Include/CommonString.h>
#include "Core/Include/GlobalHelper.h"

namespace Reagents {
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 *  \param p_MainWindow =  MainMenu::CMainWindow type parameter
 *  \param p_DataConnector =  Core::CDataConnector type parameter
 */
/****************************************************************************/
CModifyReagentStationDlg::CModifyReagentStationDlg(QWidget *p_Parent, MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector) :
    MainMenu::CDialogFrame(p_Parent, p_MainWindow), mp_Ui(new Ui::CModifyReagentStationDlg), mp_DataConnector(p_DataConnector)
{
    mp_Ui->setupUi(GetContentFrame());
    m_ProcessRunning = false ;

    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->setModel(&m_ReagentsDataModel);

    mp_Ui->scrollTable->SetContent(mp_TableWidget);

    mp_TableWidget->SetVisibleRows(8);
    m_ReagentsDataModel.SetVisibleRowCount(8);
    m_ReagentsDataModel.SetRequiredContainers(mp_DataConnector->ReagentList,mp_DataConnector->ReagentGroupList,mp_DataConnector->DashboardStationList, 2);
    mp_TableWidget->horizontalHeader()->show();
    ResizeHorizontalSection();

    CONNECTSIGNALSLOT(mp_TableWidget, pressed(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_Ui->btnCancel, clicked(), this, OnCancel());
    CONNECTSIGNALSLOT(mp_Ui->btnOk, clicked(), this, OnOk());
    CONNECTSIGNALSLOT(p_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
    CONNECTSIGNALSLOT(p_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
    // Init Message dialog
    m_MessageDlg.SetTitle(m_strInforMsg);
    m_MessageDlg.SetIcon(QMessageBox::Information);
    m_MessageDlg.SetButtonText(1, m_strOK);
    m_MessageDlg.HideButtons();
}

/****************************************************************************/
/*!
     *  \brief Destructor
     */
/****************************************************************************/
CModifyReagentStationDlg::~CModifyReagentStationDlg()
{
    try {
        delete mp_TableWidget;
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint.
    }
}

void CModifyReagentStationDlg::SetEditedDashboardStation(DataManager::CDashboardStation* p_Station)
{
    m_EditedStation = *p_Station;

    if (m_EditedStation.IsParaffinBath())
    {
        m_ReagentsDataModel.SetReagentType(ONLY_PARAFFIN_REAGENT);
    }
    else
    {
        m_ReagentsDataModel.SetReagentType(NON_PARAFFINBATH_REAGENT);
    }

    m_ReagentsDataModel.UpdateReagentList();
    //Check if it is a valid
    const DataManager::CReagent* pReagent = mp_DataConnector->ReagentList->GetReagent(m_EditedStation.GetDashboardReagentID());
    if (pReagent){
        mp_TableWidget->selectRow(m_ReagentsDataModel.GetReagentPositionOfReagent(pReagent->GetReagentID()));
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CModifyReagentStationDlg::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        mp_Ui->retranslateUi(this);
        RetranslateUI();
        break;
    default:
        break;
    }
}

void CModifyReagentStationDlg::showEvent(QShowEvent * p_Event)
{
    Q_UNUSED(p_Event);
    mp_Ui->btnOk->setEnabled(true);
}

/****************************************************************************/
/*!
 *  \brief Resizes the columns in the reagents table
 */
/****************************************************************************/
void CModifyReagentStationDlg::ResizeHorizontalSection()
{
    if (mp_TableWidget == NULL)
    {
        return;
    }
    mp_TableWidget->horizontalHeader()->resizeSection(0, 120);
    mp_TableWidget->horizontalHeader()->resizeSection(1, 150);

}


/****************************************************************************/
/*!
 *  \brief This slot is called when Cancel button in ModifyReagentDlg is pressed.
 */
/****************************************************************************/
void CModifyReagentStationDlg::OnCancel()
{
    emit CancelPressed();
    reject();
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Model Index
 */
/****************************************************************************/
void CModifyReagentStationDlg::SelectionChanged(QModelIndex Index)
{
    QString Id = m_ReagentsDataModel.data(Index, (int)Qt::UserRole).toString();
    m_EditedStation.SetDashboardReagentID(Id);
}
/****************************************************************************/
/*!
 *  \brief Saves the changes in the dialog
 */
/************************************************************************/
void CModifyReagentStationDlg::OnOk()
{
	bool bRevertSelectedProgram = false;
    if (!Core::CGlobalHelper::CheckSelectedProgram(bRevertSelectedProgram))
        return;

    if (bRevertSelectedProgram)
        emit UnselectProgram();
		
    m_EditedStation.SetDashboardReagentStatus("Empty");
    accept();
    emit UpdateStationChangeReagent(m_EditedStation.GetDashboardStationID(),
                                    m_EditedStation.GetDashboardReagentID());

    // It is disabled for the station has been set empty when calling "emit UpdateStationChangeReagent"
    //emit UpdateStationSetAsEmpty(m_EditedStation.GetDashboardStationID());
}

void CModifyReagentStationDlg::OnUserRoleChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    if (m_CurrentUserRole == MainMenu::CMainWindow::Operator)
    {
        mp_Ui->btnOk->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CModifyReagentStationDlg::OnProcessStateChanged()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if (!m_ProcessRunning) {
        //Edit Mode
        mp_Ui->btnOk->setEnabled(true);
        mp_Ui->btnCancel->setText(m_strCancel);
    }
    else {
        //View Mode
        mp_Ui->btnOk->setEnabled(false);
        mp_Ui->btnCancel->setText(m_strClose);
    }
}

/*****************************************************************************/
/*!
  *  \brief Translates the strings in UI to the selected language
  */
/****************************************************************************/
void CModifyReagentStationDlg::RetranslateUI()
{
    m_strCancel = QApplication::translate("Reagents::CModifyReagentStationDlg", "Cancel", Q_NULLPTR, -1);
    m_strClose = QApplication::translate("Reagents::CModifyReagentStationDlg", "Close", Q_NULLPTR, -1);
    m_strInforMsg = QApplication::translate("Reagents::CModifyReagentStationDlg", "Information Message", Q_NULLPTR, -1);
    m_strOK = QApplication::translate("Reagents::CModifyReagentStationDlg", "OK", Q_NULLPTR, -1);
    (void)m_ReagentsDataModel.setHeaderData(0,Qt::Horizontal, QApplication::translate("Core::CReagentStationEditModel",
                                                                                 "Reagent", Q_NULLPTR, -1), 0);
    (void)m_ReagentsDataModel.setHeaderData(1,Qt::Horizontal, QApplication::translate("Core::CReagentStationEditModel",
                                                                                 "Group", Q_NULLPTR, -1),0);
}


}
