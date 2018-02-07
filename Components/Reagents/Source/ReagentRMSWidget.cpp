/****************************************************************************/
/*! \file ReagentRMSWidget.cpp
 *
 *  \brief ReagentWidget Implementation.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2013-01-15
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
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/SliderControl.h"
#include "Reagents/Include/ReagentRMSWidget.h"
#include "Reagents/Include/ModifyReagentRMSDlg.h"
#include "ui_ReagentsWidget.h"
#include "Dashboard/Include/CommonString.h"
#include "Core/Include/GlobalHelper.h"

//lint -e613

namespace Reagents {

Global::RMSOptions_t CReagentRMSWidget :: RMSPROCESSINGOPTION;
Global::RMSOptions_t CReagentRMSWidget :: RMSCLEANINGOPTIONS;

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CReagentRMSWidget::CReagentRMSWidget(QWidget *p_Parent):
    MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CReagentRMSWidget),
    mp_DataConnector(NULL),
    mp_ReagentList(NULL),
    mp_ModifiyReagentRMSDlg(NULL),
    mp_Reagent(NULL),
    m_lastSelectRMSMoldeID(-1),
    m_lastSelectCleaningRMSMoldeID(-1),
    m_IsCleaningReagentSelected(false)
{
    mp_Ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("RMS"));

    m_ProcessRunning = false;
    m_ShowMessageDialog = false;

    m_ReagentCleaningModel.SetReagentGroupType(true);
    mp_TableWidgetCleaning = new MainMenu::CBaseTable;
    mp_TableWidgetCleaning->setModel(&m_ReagentCleaningModel);
    mp_Ui->scrolltable_cleaning->SetContent(mp_TableWidgetCleaning);

    m_ReagentRMSModel.SetReagentGroupType(false);
    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->setModel(&m_ReagentRMSModel);
    mp_Ui->scrollTable->SetContent(mp_TableWidget);

    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();

    CONNECTSIGNALSLOT(mp_Ui->btnEdit, clicked(), this, OnEdit());
    CONNECTSIGNALSLOT(mp_Ui->btnNew, clicked(), this, OnNew());
    CONNECTSIGNALSLOT(mp_Ui->btnDelete, clicked(), this, OnDelete());

    CONNECTSIGNALSLOT(mp_Ui->radioOff, clicked(), this, OnRMSOFF());
    CONNECTSIGNALSLOT(mp_Ui->radioCassettes, clicked(), this, OnRMSCassettes());
    CONNECTSIGNALSLOT(mp_Ui->radioCycles, clicked(), this, OnRMSCycles());
    CONNECTSIGNALSLOT(mp_Ui->radioDays, clicked(), this, OnRMSDays());

    m_ButtonGroupRMSMode.addButton(mp_Ui->radioOff, 0);
    m_ButtonGroupRMSMode.addButton(mp_Ui->radioCassettes, 1);
    m_ButtonGroupRMSMode.addButton(mp_Ui->radioCycles, 2);
    m_ButtonGroupRMSMode.addButton(mp_Ui->radioDays, 3);


    CONNECTSIGNALSLOT(mp_Ui->radioOff_Cleaning, clicked(), this, OnCleaningRMSOFF());
    CONNECTSIGNALSLOT(mp_Ui->radioCycles_Cleaning, clicked(), this, OnCleaningRMSCycles());
    CONNECTSIGNALSLOT(mp_Ui->radioDays_Cleaning, clicked(), this, OnCleaningRMSDays());

    m_ButtonGroupCleaningRMSMode.addButton(mp_Ui->radioOff_Cleaning, 0);
    m_ButtonGroupCleaningRMSMode.addButton(mp_Ui->radioCycles_Cleaning, 1);
    m_ButtonGroupCleaningRMSMode.addButton(mp_Ui->radioDays_Cleaning, 2);


    CONNECTSIGNALSLOT(mp_TableWidget, pressed(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_TableWidgetCleaning, pressed(QModelIndex), this, SelectionChangedCleaningTable(QModelIndex));
    CONNECTSIGNALSLOT(this, UpdateReagentList(), &m_ReagentRMSModel, UpdateReagentList());
    CONNECTSIGNALSLOT(this, UpdateReagentList(), &m_ReagentCleaningModel, UpdateReagentList());
    PopulateReagentList();
}

/****************************************************************************/
/*!
     *  \brief Destructor
     */
/****************************************************************************/
CReagentRMSWidget::~CReagentRMSWidget()
{
    try {
        delete mp_TableWidget;
        delete mp_Ui;
        delete mp_ModifiyReagentRMSDlg;
        delete mp_TableWidgetCleaning;
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
void CReagentRMSWidget::changeEvent(QEvent *p_Event)
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
/****************************************************************************/
/*!
 *  \brief Resizes the columns in the reagents table
 */
/****************************************************************************/
void CReagentRMSWidget::ResizeHorizontalSection()
{
    if (mp_TableWidget)
    {
        mp_TableWidget->horizontalHeader()->resizeSection(0, 120);
        mp_TableWidget->horizontalHeader()->resizeSection(1, 150);
        mp_TableWidget->horizontalHeader()->resizeSection(2, 150);
    }

    if (mp_TableWidgetCleaning)
    {
        mp_TableWidgetCleaning->horizontalHeader()->resizeSection(0, 120);
        mp_TableWidgetCleaning->horizontalHeader()->resizeSection(1, 150);
        mp_TableWidgetCleaning->horizontalHeader()->resizeSection(2, 150);
    }

}
/****************************************************************************/
/*!
 *  \brief Fills the reagents list with content
 */
/****************************************************************************/
void CReagentRMSWidget::PopulateReagentList()
{
    if (mp_TableWidget && mp_TableWidgetCleaning)
    {
        mp_TableWidget->SetVisibleRows(2,28,26);
        m_ReagentRMSModel.SetVisibleRowCount(2);
        mp_TableWidget->horizontalHeader()->show();
        mp_TableWidget->verticalHeader()->setDefaultSectionSize(26);

        mp_TableWidgetCleaning->SetVisibleRows(2,28,26);
        m_ReagentCleaningModel.SetVisibleRowCount(2);
        mp_TableWidgetCleaning->horizontalHeader()->show();
        mp_TableWidgetCleaning->verticalHeader()->setDefaultSectionSize(26);

        ResizeHorizontalSection();
    }
}
/****************************************************************************/
/*!
 *  \brief This slot is used to close all open dialogs of this widget
 */
/****************************************************************************/
void CReagentRMSWidget::CloseDialogs()
{
    ResetButtons();
    if (mp_ModifiyReagentRMSDlg) {
        mp_ModifiyReagentRMSDlg->accept();
    }
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when processing RMS mode changed to Off
 */
/****************************************************************************/
void CReagentRMSWidget::OnRMSOFF()
{
    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Warning);
    m_MessageDlg.SetButtonText(1, m_strYes);
    m_MessageDlg.SetButtonText(3, m_strCancel);
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(m_strConfirmChangeRMSMode);

    if (m_MessageDlg.exec() == (int)QDialog::Accepted) {
        m_UserSettings.SetModeRMSProcessing(Global::RMS_OFF);
        emit RMSSettingChanged(m_UserSettings);
        m_lastSelectRMSMoldeID = m_ButtonGroupRMSMode.checkedId();
    }
    else
    {
        if (m_lastSelectCleaningRMSMoldeID != -1)
        {
            m_ButtonGroupRMSMode.button(m_lastSelectRMSMoldeID)->setChecked(true);
        }
    }

}

/****************************************************************************/
/*!
 *  \brief Slot will be called when processing RMS mode changed to Cassettes
 */
/****************************************************************************/
void CReagentRMSWidget::OnRMSCassettes()
{
    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Warning);
    m_MessageDlg.SetButtonText(1, m_strYes);
    m_MessageDlg.SetButtonText(3, m_strCancel);
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(m_strConfirmChangeRMSMode);

    if (m_MessageDlg.exec() == (int)QDialog::Accepted) {
        m_UserSettings.SetModeRMSProcessing(Global::RMS_CASSETTES);
        emit RMSSettingChanged(m_UserSettings);
        m_lastSelectRMSMoldeID = m_ButtonGroupRMSMode.checkedId();
    }
    else
    {
        if (m_lastSelectRMSMoldeID != -1)
        {
            m_ButtonGroupRMSMode.button(m_lastSelectRMSMoldeID)->setChecked(true);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when processing RMS mode changed to Cycles
 */
/****************************************************************************/
void CReagentRMSWidget::OnRMSCycles()
{
    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Warning);
    m_MessageDlg.SetButtonText(1, m_strYes);
    m_MessageDlg.SetButtonText(3, m_strCancel);
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(m_strConfirmChangeRMSMode);

    if (m_MessageDlg.exec() == (int)QDialog::Accepted) {
        m_UserSettings.SetModeRMSProcessing(Global::RMS_CYCLES);
        emit RMSSettingChanged(m_UserSettings);
        m_lastSelectRMSMoldeID = m_ButtonGroupRMSMode.checkedId();
    }
    else
    {
        if (m_lastSelectRMSMoldeID != -1)
        {
            m_ButtonGroupRMSMode.button(m_lastSelectRMSMoldeID)->setChecked(true);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when processing RMS mode changed to Days
 */
/****************************************************************************/
void CReagentRMSWidget::OnRMSDays()
{
    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Warning);
    m_MessageDlg.SetButtonText(1, m_strYes);
    m_MessageDlg.SetButtonText(3, m_strCancel);
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(m_strConfirmChangeRMSMode);

    if (m_MessageDlg.exec() == (int)QDialog::Accepted) {
        m_UserSettings.SetModeRMSProcessing(Global::RMS_DAYS);
        emit RMSSettingChanged(m_UserSettings);
        m_lastSelectRMSMoldeID = m_ButtonGroupRMSMode.checkedId();
    }
    else
    {
        if (m_lastSelectRMSMoldeID != -1)
        {
            m_ButtonGroupRMSMode.button(m_lastSelectRMSMoldeID)->setChecked(true);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when Cleaning RMS mode changed to Off
 */
/****************************************************************************/
void CReagentRMSWidget::OnCleaningRMSOFF()
{
    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Warning);
    m_MessageDlg.SetButtonText(1, m_strYes);
    m_MessageDlg.SetButtonText(3, m_strCancel);
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(m_strConfirmRMSOffMode);

    if (m_MessageDlg.exec() == (int)QDialog::Accepted) {
        m_UserSettings.SetModeRMSCleaning(Global::RMS_OFF);
        emit RMSSettingChanged(m_UserSettings);
        m_lastSelectCleaningRMSMoldeID = m_ButtonGroupCleaningRMSMode.checkedId();
    }
    else
    {
        if (m_lastSelectCleaningRMSMoldeID != -1)
        {
            m_ButtonGroupCleaningRMSMode.button(m_lastSelectCleaningRMSMoldeID)->setChecked(true);
        }
    }
}
/****************************************************************************/
/*!
 *  \brief Slot will be called when Cleaning RMS mode changed to Cycles
 */
/****************************************************************************/
void CReagentRMSWidget::OnCleaningRMSCycles()
{
    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Warning);
    m_MessageDlg.SetButtonText(1, m_strYes);
    m_MessageDlg.SetButtonText(3, m_strCancel);
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(m_strConfirmChangeRMSMode);

    if (m_MessageDlg.exec() == (int)QDialog::Accepted) {
        m_UserSettings.SetModeRMSCleaning(Global::RMS_CYCLES);
        emit RMSSettingChanged(m_UserSettings);
        m_lastSelectCleaningRMSMoldeID = m_ButtonGroupCleaningRMSMode.checkedId();
    }
    else
    {
        if (m_lastSelectCleaningRMSMoldeID != -1)
        {
            m_ButtonGroupCleaningRMSMode.button(m_lastSelectCleaningRMSMoldeID)->setChecked(true);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when Cleaning RMS mode changed to Days
 */
/****************************************************************************/
void CReagentRMSWidget::OnCleaningRMSDays()
{
    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Warning);
    m_MessageDlg.SetButtonText(1, m_strYes);
    m_MessageDlg.SetButtonText(3, m_strCancel);
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(m_strConfirmChangeRMSMode);

    if (m_MessageDlg.exec() == (int)QDialog::Accepted) {
        m_UserSettings.SetModeRMSCleaning(Global::RMS_DAYS);
        emit RMSSettingChanged(m_UserSettings);
        m_lastSelectCleaningRMSMoldeID = m_ButtonGroupCleaningRMSMode.checkedId();
    }
    else
    {
        if (m_lastSelectCleaningRMSMoldeID != -1)
        {
            m_ButtonGroupCleaningRMSMode.button(m_lastSelectCleaningRMSMoldeID)->setChecked(true);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of a reagent
 */
/****************************************************************************/
void CReagentRMSWidget::OnEdit()
{
    if (NULL == mp_Reagent)
        return;

    if (mp_ModifiyReagentRMSDlg)
    {
        mp_ModifiyReagentRMSDlg->SetDialogTitle(m_strEditReagent);
        mp_ModifiyReagentRMSDlg->SetButtonType(EDIT_BTN_CLICKED);

        bool bShowCleaningReagent = false;
        Global::RMSOptions_t rmsOption = Reagents::CReagentRMSWidget::RMSPROCESSINGOPTION;
        if (mp_DataConnector->ReagentGroupList->GetReagentGroup(mp_Reagent->GetGroupID())->IsCleaningReagentGroup())
        {
            rmsOption = RMSCLEANINGOPTIONS;
            bShowCleaningReagent = true;
        }

        mp_ModifiyReagentRMSDlg->InitDialog(mp_Reagent, mp_DataConnector->ReagentGroupList, rmsOption, bShowCleaningReagent);
        mp_ModifiyReagentRMSDlg->show();
    }
}

/****************************************************************************/
/*!
 *  \brief Creates a new reagent and opens its edit dialog
 */
/****************************************************************************/
void CReagentRMSWidget::OnNew()
{
    if (mp_ModifiyReagentRMSDlg)
    {
        mp_ModifiyReagentRMSDlg->SetDialogTitle(m_strNewReagent);
        mp_ModifiyReagentRMSDlg->SetButtonType(NEW_BTN_CLICKED);

        Global::RMSOptions_t rmsOption = Reagents::CReagentRMSWidget::RMSPROCESSINGOPTION;
        if (mp_Reagent) {
            if (mp_DataConnector->ReagentGroupList->GetReagentGroup(mp_Reagent->GetGroupID())->IsCleaningReagentGroup()) {
                rmsOption = RMSCLEANINGOPTIONS;
            }
        }

        mp_Reagent = NULL;
        mp_ModifiyReagentRMSDlg->InitDialog(mp_Reagent, mp_DataConnector->ReagentGroupList, rmsOption, false);
        mp_ModifiyReagentRMSDlg->show();
        ResetButtons();
    }
}

/****************************************************************************/
/*!
 *  \brief Deletes the currently selected reagent
 */
/****************************************************************************/
void CReagentRMSWidget::OnDelete()
{
    if (NULL == mp_Reagent)
        return;

    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Warning);
    m_MessageDlg.SetButtonText(1, m_strYes);
    m_MessageDlg.SetButtonText(3, m_strCancel);
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(m_strConfirmDelReagent);

    if (m_MessageDlg.exec() == (int)QDialog::Accepted) {
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->btnEdit->setEnabled(false);
        emit RemoveReagent(mp_Reagent->GetReagentID());
		mp_Reagent = NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Cancel button in ModifyReagentDlg is pressed.
 */
/****************************************************************************/
void CReagentRMSWidget::OnCancelPressed()
{
    //mp_TableWidget->clearSelection();
    //mp_TableWidgetCleaning->clearSelection();
    mp_Ui->btnEdit->setEnabled(false);
    m_IsCleaningReagentSelected = false;
    ResetButtons();
}


/****************************************************************************/
/*!
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CReagentRMSWidget::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{
    Q_UNUSED(p_UserSettings)
}


/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Model Index
 */
/****************************************************************************/
void CReagentRMSWidget::SelectionChanged(QModelIndex Index)
{
    if (mp_TableWidgetCleaning)
    {
        mp_TableWidgetCleaning->clearSelection();
    }
    if (!Index.isValid()) {
        Index = mp_TableWidget->currentIndex();
        if (!Index.isValid()) {
            mp_TableWidget->clearSelection();
            ResetButtons();
            return;
        }
    }
    m_IsCleaningReagentSelected = false;
    QString Id = m_ReagentRMSModel.data(Index, (int)Qt::UserRole).toString();
    UpdateButtons(Id);
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Model Index
 */
/****************************************************************************/
void CReagentRMSWidget::SelectionChangedCleaningTable(QModelIndex Index)
{
    mp_TableWidget->clearSelection();
    if (!Index.isValid()) {
        Index = mp_TableWidgetCleaning->currentIndex();
        if (!Index.isValid()) {
            mp_TableWidgetCleaning->clearSelection();
            ResetButtons();
            return;
        }
    }
    m_IsCleaningReagentSelected = true;
    QString Id = m_ReagentCleaningModel.data(Index, (int)Qt::UserRole).toString();
    UpdateButtons(Id);

    mp_Ui->btnNew->setEnabled(false);
    mp_Ui->btnDelete->setEnabled(false);
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Id = Reagent ID
 */
/****************************************************************************/
void CReagentRMSWidget::UpdateButtons(QString& Id)
{
    mp_Reagent = mp_ReagentList->GetReagent(Id);
    if (mp_Reagent) {
        if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
             m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
                (!m_ProcessRunning)) {

            if (Core::CGlobalHelper::CheckIfCanEdit(Id, 2) == false) {
                mp_Ui->btnEdit->setEnabled(false);
                mp_Ui->btnDelete->setEnabled(false);
            }
            else {
                //Edit Mode
                mp_Ui->btnNew->setEnabled(true);
                mp_Ui->btnEdit->setEnabled(true);
                mp_Ui->btnDelete->setEnabled(true);
            }

        }
        else {
            mp_Ui->btnNew->setEnabled(false);
            mp_Ui->btnDelete->setEnabled(false);
        }

        if (mp_DataConnector->ProgramList->GetReagentIDList().contains(Id))
        {
            mp_Ui->btnDelete->setEnabled(false);
        }
    }
}
/****************************************************************************/
/*!
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CReagentRMSWidget::OnUserRoleChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {

        if (Core::CGlobalHelper::CheckIfCanEdit() == false ) {
            mp_Ui->groupRMS->setEnabled(false);
            mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);
            mp_TableWidget->clearSelection();
            mp_TableWidgetCleaning->clearSelection();
        }
        else {
            //Edit Mode
            mp_Ui->btnNew->setEnabled(true);
            mp_Ui->groupRMS->setEnabled(true);
            mp_Ui->CleaningReagent_GroupRMS->setEnabled(true);
        }
    }
    else {
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->groupRMS->setEnabled(false);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CReagentRMSWidget::OnProcessStateChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {

        if (Core::CGlobalHelper::CheckIfCanEdit() == false ) {
            mp_Ui->btnNew->setEnabled(true);
            mp_Ui->groupRMS->setEnabled(false);
            mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);
            mp_TableWidget->clearSelection();
            mp_TableWidgetCleaning->clearSelection();
        }
        else {
            mp_Ui->btnNew->setEnabled(true);
            mp_Ui->groupRMS->setEnabled(true);
            mp_Ui->CleaningReagent_GroupRMS->setEnabled(true);
        }

    }
    else {
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->btnEdit->setEnabled(false);
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->groupRMS->setEnabled(false);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This resets the buttons on the right of the screen
 */
/****************************************************************************/
void CReagentRMSWidget::ResetButtons()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) && (!m_ProcessRunning)) {
        if (Core::CGlobalHelper::CheckIfCanEdit() == false ) {
            mp_Ui->groupRMS->setEnabled(false);
            mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);
            mp_Ui->btnNew->setEnabled(true);
        }
        else {
            //Edit Mode
            if (!m_IsCleaningReagentSelected)
            {
                mp_Ui->btnNew->setEnabled(true);
            }
            mp_Ui->btnEdit->setEnabled(false);
            mp_Ui->btnDelete->setEnabled(false);
            mp_Ui->groupRMS->setEnabled(true);
            mp_Ui->CleaningReagent_GroupRMS->setEnabled(true);
        }
    }
    else {
        //View Mode
        mp_Ui->btnEdit->setEnabled(false);
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->groupRMS->setEnabled(false);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);
    }
}

/*****************************************************************************/
/*!
  *  \brief Translates the strings in UI to the selected language
  */
/****************************************************************************/
void CReagentRMSWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Reagents::CReagentRMSWidget",
                                                                 "RMS", Q_NULLPTR, -1));

    m_strEditReagent = QApplication::translate("Reagents::CReagentRMSWidget",
                                           "Edit Reagent", Q_NULLPTR, -1);

    m_strNewReagent = QApplication::translate("Reagents::CReagentRMSWidget",
                                           "New Reagent", Q_NULLPTR, -1);

    m_strConfirmDelReagent = QApplication::translate("Reagents::CReagentRMSWidget",
                                           "Do you really want to delete the selected reagent?", Q_NULLPTR, -1);

    m_strConfirmMsg = QApplication::translate("Reagents::CReagentRMSWidget", "Confirmation Message", Q_NULLPTR, -1);
    m_strYes = QApplication::translate("Reagents::CReagentRMSWidget", "Yes", Q_NULLPTR, -1);
    m_strCancel = QApplication::translate("Reagents::CReagentRMSWidget", "Cancel", Q_NULLPTR, -1);
    m_strConfirmChangeRMSMode =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                                       "Do you really want to change the RMS controlling mode?  If reagents are being replaced, please reset data for those reagents on the Status page.", Q_NULLPTR, -1);
    m_strConfirmRMSOffMode =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
        "Do you really want to turn off RMS mode? Longer than the recommended use of cleaning reagents can cause paraffin blockages in reagent lines. The cleaning program can be run as usual. However, the reagent quality must be controlled carefully.", Q_NULLPTR, -1);

    (void) m_ReagentRMSModel.setHeaderData(0,Qt::Horizontal,QApplication::translate("Core::CReagentRMSModel",
                                                                                 "Reagent", Q_NULLPTR, -1),0);
    (void) m_ReagentRMSModel.setHeaderData(1,Qt::Horizontal,QApplication::translate("Core::CReagentRMSModel",
                                                                                 "ReagentGroup", Q_NULLPTR, -1),0);
    QString SecondColumnName("");
    switch(Reagents:: CReagentRMSWidget::RMSPROCESSINGOPTION) {
        case Global::RMS_CASSETTES:
            SecondColumnName = QApplication::translate("Core::CReagentRMSModel", "Cassettes until change", Q_NULLPTR, -1);
            break;
        case Global::RMS_CYCLES:
            SecondColumnName = QApplication::translate("Core::CReagentRMSModel", "Cycles until change", Q_NULLPTR, -1);
            break;
        case Global::RMS_DAYS:
            SecondColumnName = QApplication::translate("Core::CReagentRMSModel", "Days until change", Q_NULLPTR, -1);
            break;
        default:
            break;
    }

    (void) m_ReagentRMSModel.setHeaderData(2,Qt::Horizontal, QApplication::translate("Core::CReagentRMSModel",
                                                                                 SecondColumnName.toUtf8(), Q_NULLPTR, -1),0);

    (void) m_ReagentCleaningModel.setHeaderData(0,Qt::Horizontal,QApplication::translate("Core::CReagentRMSModel",
                                                                                 "Reagent", Q_NULLPTR, -1),0);
    (void) m_ReagentCleaningModel.setHeaderData(1,Qt::Horizontal,QApplication::translate("Core::CReagentRMSModel",
                                                                                 "Reagent Group", Q_NULLPTR, -1),0);

    switch(Reagents:: CReagentRMSWidget::RMSCLEANINGOPTIONS) {
        case Global::RMS_CYCLES:
            SecondColumnName = QApplication::translate("Core::CReagentRMSModel", "Cycles until change", Q_NULLPTR, -1);
            break;
        case Global::RMS_DAYS:
            SecondColumnName = QApplication::translate("Core::CReagentRMSModel", "Days until change", Q_NULLPTR, -1);
            break;
        default:
         break;

    }
    (void) m_ReagentCleaningModel.setHeaderData(2,Qt::Horizontal,QApplication::translate("Core::CReagentRMSModel",
                                                                                 SecondColumnName.toUtf8(), Q_NULLPTR, -1),0);
}

/****************************************************************************/
/*!
 *  \brief This event is called whenever UserSetting Changed
 */
/****************************************************************************/
void CReagentRMSWidget ::UpdateUserSetting()
{
    if(mp_DataConnector->SettingsInterface->GetUserSettings()) {
        m_UserSettings = *(mp_DataConnector->SettingsInterface->GetUserSettings());
        m_ReagentRMSModel.SetUserSettings(&m_UserSettings);
        m_ReagentCleaningModel.SetUserSettings(&m_UserSettings);

        if(RMSPROCESSINGOPTION != m_UserSettings.GetModeRMSProcessing())
            RMSPROCESSINGOPTION = m_UserSettings.GetModeRMSProcessing();
        switch(RMSPROCESSINGOPTION) {
        case Global::RMS_CASSETTES:
            mp_Ui->radioCassettes->setChecked(true);
            if (mp_TableWidget)
            {
                mp_TableWidget->setColumnHidden(2, false);
            }
            ResizeHorizontalSection();
            emit RMSChanged(Global::RMS_CASSETTES);
            break;
        case Global::RMS_CYCLES:
            mp_Ui->radioCycles->setChecked(true);
            if (mp_TableWidget)
            {
                mp_TableWidget->setColumnHidden(2, false);
            }
            ResizeHorizontalSection();
            emit RMSChanged(Global::RMS_CYCLES);
            break;
        case Global::RMS_DAYS:
            mp_Ui->radioDays->setChecked(true);
            if (mp_TableWidget)
            {
                mp_TableWidget->setColumnHidden(2, false);
            }
            ResizeHorizontalSection();
            emit RMSChanged(Global::RMS_DAYS);
            break;
        case Global::RMS_OFF:
            mp_Ui->radioOff->setChecked(true);
            if (mp_TableWidget)
            {
                mp_TableWidget->setColumnHidden(2, true);
            }
            emit RMSChanged(Global::RMS_OFF);
            break;
        default:
            break;

        }
        m_lastSelectRMSMoldeID = m_ButtonGroupRMSMode.checkedId();

        if(RMSCLEANINGOPTIONS != m_UserSettings.GetModeRMSCleaning())
            RMSCLEANINGOPTIONS = m_UserSettings.GetModeRMSCleaning();
        switch(RMSCLEANINGOPTIONS) {
        case Global::RMS_CYCLES:
            mp_Ui->radioCycles_Cleaning->setChecked(true);
            if (mp_TableWidgetCleaning)
            {
                mp_TableWidgetCleaning->setColumnHidden(2, false);
            }
            ResizeHorizontalSection();
            emit RMSCleaningChanged(Global::RMS_CYCLES);
            break;
        case Global::RMS_DAYS:
            mp_Ui->radioDays_Cleaning->setChecked(true);
            if (mp_TableWidgetCleaning)
            {
                mp_TableWidgetCleaning->setColumnHidden(2, false);
            }
            ResizeHorizontalSection();
            emit RMSCleaningChanged(Global::RMS_DAYS);
            break;
        case Global::RMS_OFF:
            mp_Ui->radioOff_Cleaning->setChecked(true);
            if (mp_TableWidgetCleaning)
            {
                mp_TableWidgetCleaning->setColumnHidden(2, true);
            }
            emit RMSCleaningChanged(Global::RMS_OFF);
            break;
        default:
            break;
        }
        m_lastSelectCleaningRMSMoldeID = m_ButtonGroupCleaningRMSMode.checkedId();
    }
}


void CReagentRMSWidget::showEvent(QShowEvent *)
{
    /*m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."));*/
    m_ShowMessageDialog = true ;
    if (mp_TableWidget)
    {
        mp_TableWidget->clearSelection();
    }
    mp_Ui->scrollTable->Reset();
    m_ReagentRMSModel.UpdateReagentList();

    if (mp_TableWidgetCleaning)
    {
        mp_TableWidgetCleaning->clearSelection();
    }
    mp_Ui->btnEdit->setEnabled(false);
    m_ReagentCleaningModel.UpdateReagentList();
    m_IsCleaningReagentSelected = false;
    ResetButtons();
}


/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *  \param p_DataConnector = pointer to DataConnector
 *  \param p_ReagentList = pointer to reagent list
 *  \param p_MainWindow = application's main window
 *  \param p_KeyBoard =  pointer to keyboard instance
 */
/****************************************************************************/
void CReagentRMSWidget::SetPtrToMainWindow(Core::CDataConnector *p_DataConnector,
                                         DataManager::CDataReagentList *p_ReagentList,
                                         MainMenu::CMainWindow *p_MainWindow,
                                         KeyBoard::CKeyBoard *p_KeyBoard)
{
    mp_DataConnector = p_DataConnector;
    mp_ReagentList = p_ReagentList;
    // set the reagent ist to the model
    m_ReagentRMSModel.SetReagentList(mp_ReagentList, 3);
    m_ReagentRMSModel.SetReagentGroupList(mp_DataConnector->ReagentGroupList);

    m_ReagentCleaningModel.SetReagentList(mp_ReagentList, 3);
    m_ReagentCleaningModel.SetReagentGroupList(mp_DataConnector->ReagentGroupList);
    ResizeHorizontalSection();

    mp_ModifiyReagentRMSDlg = new CModifyReagentRMSDlg(this, p_KeyBoard, p_MainWindow, mp_DataConnector);
    mp_ModifiyReagentRMSDlg->setModal(true);

    CONNECTSIGNALSLOT(this,RMSSettingChanged(DataManager::CUserSettings&),mp_DataConnector,SendUpdatedSettings(DataManager::CUserSettings&));

    CONNECTSIGNALSLOT(mp_ModifiyReagentRMSDlg, UpdateReagent(DataManager::CReagent &), mp_DataConnector,
                        SendReagentUpdate(DataManager::CReagent &));
    CONNECTSIGNALSLOT(mp_ModifiyReagentRMSDlg, AddReagent(DataManager::CReagent &) ,
                        mp_DataConnector, SendReagentAdd(DataManager::CReagent &));
    CONNECTSIGNALSLOT(this, RemoveReagent(QString) ,
                      mp_DataConnector, SendReagentRemove(QString));
    CONNECTSIGNALSLOT(mp_DataConnector,UserSettingsUpdated(),this,UpdateUserSetting());

    CONNECTSIGNALSLOT(mp_DataConnector, ReagentUpdated(), this, OnReagentUpdated());
    CONNECTSIGNALSLOT(mp_DataConnector, ReagentAdded(), this, OnReagentAdded());

    CONNECTSIGNALSLOT(mp_ModifiyReagentRMSDlg, CancelPressed(), this, OnCancelPressed());
}

void CReagentRMSWidget::OnReagentUpdated()
{
    if (mp_ModifiyReagentRMSDlg) {
        mp_ModifiyReagentRMSDlg->accept();
        this->ResetButtons();
    }
}

void CReagentRMSWidget::OnReagentAdded()
{
    m_ReagentRMSModel.UpdateReagentList();
    if (mp_ModifiyReagentRMSDlg) {
        mp_ModifiyReagentRMSDlg->accept();
    }
}

} // end namespace Reagents
