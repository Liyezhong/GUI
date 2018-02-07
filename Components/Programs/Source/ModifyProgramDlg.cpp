/****************************************************************************/
/*! \file ModifyProgramDlg.cpp
 *
 *  \brief Implementation of file for class CModifyProgramDlg.
 *
 *  \b Description:
 *          This class implements a widget for displaying program step list of
 *          selected program in a table and provides the user interfaces to
 *          Edit program name and abbreviation, Add, Copy, Assigning color
 *          to the program and Delete a program step.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-28
 *   $Author:  $ M.Scherer, N.Kamath, C.Adaragunchi1234, Swati Tiwari
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Core/Include/GlobalHelper.h"

#include "Programs/Include/ModifyProgramDlg.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Helper/Include/Types.h"
#include "ui_ModifyProgramDlg.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"



namespace Programs {

const QString UNLOADER_STEP_ID = "S7";  //!< Unloader step id
const QString TRANSFER_STEP_ID = "S8";  //!< Transfer step id
const int USER_PROGRAM_COUNT =    10;   //!< Number of User Program in a table
const int NAME_LENGTH_MIN = 1;          //!< name length Min
const int NAME_LENGTH_MAX = 20;         //!< name length Max
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Reference to Parent widget
 *  \iparam p_KeyBoard = Reference to Keyboard widget
 *  \iparam p_MainWindow = Reference to MainWindow widget
 *  \iparam p_DataConnector = Reference to DataConnector
 */
/****************************************************************************/
CModifyProgramDlg::CModifyProgramDlg(QWidget *p_Parent,
                                     KeyBoard::CKeyBoard *p_KeyBoard,
                                     MainMenu::CMainWindow *p_MainWindow,
                                     Core::CDataConnector *p_DataConnector) :
    MainMenu::CDialogFrame(p_Parent, p_MainWindow),
    mp_Ui(new Ui::CModifyProgramDlg),
    mp_DataConnector(p_DataConnector),
    m_ProgNameBtnClicked(false),
    m_ProgShortNameBtnClicked(false),
    m_ProcessRunning(false),
    m_TempColor(" "),
    mp_MessageDlg(NULL),
    mp_NewProgram(NULL),
    m_ColorReplaced(false),
    m_strSelectIcon(tr("Select Icon")),
    m_strConfirmMsg(tr("Confirmation Message")),
    m_strYes(tr("Yes")),
    m_strOK(tr("OK")),
    m_strClose(tr("Close")),
    m_strCancel(tr("Cancel")),
    m_strDelProgramStep(tr("Do you really want to delete the selected program step?")),
    m_strEnterValidName(tr("Please enter a valid Program Name")),
    m_strSeclectIcon(tr("Please select a Program Icon")),
    m_strLastProgName(tr("")),
    m_bIconSelected(false),
    m_isInSaving(false)

{
    mp_Ui->setupUi(GetContentFrame());
    mp_ModifyProgramIconDlg = new Programs::CModifyProgramIconDlg(this, p_MainWindow);
    mp_KeyBoardWidget = p_KeyBoard;
    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->horizontalHeader()->show();
    mp_Ui->scrollTable->SetContent(mp_TableWidget);
    mp_TableWidget->SetVisibleRows(6);
    mp_ModifyProgStepDlg = new Programs::CModifyProgramStepDlg(this, p_MainWindow, p_DataConnector);
    mp_ModifyProgStepDlg->setModal(true);
    CONNECTSIGNALSLOT(mp_Ui->btnEdit, clicked(), this, OnEdit());
    CONNECTSIGNALSLOT(mp_Ui->btnNew, clicked(), this, OnNewStep());
    CONNECTSIGNALSLOT(mp_Ui->btnCopy, clicked(), this, OnCopy());
    CONNECTSIGNALSLOT(mp_Ui->btnDelete, clicked(), this, OnDelete());
    CONNECTSIGNALSLOT(mp_Ui->btnCancel, clicked(), this, OnCancel());
    CONNECTSIGNALSLOT(mp_Ui->btnSave, clicked(), this, OnSave());
    CONNECTSIGNALSLOT(mp_TableWidget,clicked(QModelIndex), this, OnSelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_Ui->btnPrgName, clicked(), this, OnEditName());
    CONNECTSIGNALSLOT(mp_Ui->btnPrgIcon, clicked(), this, OnIconClicked());
    CONNECTSIGNALSLOT(p_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
    CONNECTSIGNALSLOT(mp_ModifyProgStepDlg, AddProgramStep(DataManager::CProgramStep*,bool),
                      this, UpdateProgramStepTable(DataManager::CProgramStep*,bool));
    CONNECTSIGNALSLOT(mp_ModifyProgramIconDlg, UpdateProgramIcon(DataManager::CProgram *), this, UpdateProgramIcon(DataManager::CProgram *));
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    OnProcessStateChanged();
    mp_Ui->btnPrgIcon->setIconSize(QSize(45, 30));
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CModifyProgramDlg::~CModifyProgramDlg()
{
    try {
        delete mp_ModifyProgStepDlg;
        delete mp_TableWidget;
        delete mp_Ui;
        delete mp_NewProgram;
        delete mp_ModifyProgramIconDlg;
    }
    catch (...) {
        // to please Lint.
    }
}

void CModifyProgramDlg::UpdateProgramIcon(DataManager::CProgram *Program)
{
    m_bIconSelected = true;
    m_Icon = Program->GetIcon();
    m_Program.SetIcon(m_Icon);
    mp_Ui->btnPrgIcon->setIcon(QIcon(":/HimalayaImages/Icons/Program/"+m_Icon+".png"));
}

/****************************************************************************/
/*!
 *  \brief Closes ModifyProgramStep dialog
 *
 */
/****************************************************************************/
void CModifyProgramDlg::CloseDialogModifyStepDlg() { mp_ModifyProgStepDlg->accept(); }
/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CModifyProgramDlg::changeEvent(QEvent *p_Event)
{
    MainMenu::CDialogFrame::changeEvent(p_Event);
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
 *  \brief Closes opened dialogs when master sends updated programs xml data.
 */
/****************************************************************************/
void CModifyProgramDlg::CloseDialogs()
{
    //      ResetButtons();
}
/****************************************************************************/
/*!
 *  \brief Resizes the columns in the program step table
 */
/****************************************************************************/
void CModifyProgramDlg::ResizeHorizontalSection()
{
    mp_TableWidget->horizontalHeader()->resizeSection(0, 70);
    mp_TableWidget->horizontalHeader()->resizeSection(1, 180);
    mp_TableWidget->horizontalHeader()->resizeSection(2, 70);
    mp_TableWidget->horizontalHeader()->resizeSection(3, 100);
    mp_TableWidget->horizontalHeader()->resizeSection(4, 10);
}

/****************************************************************************/
/*!
 *  \brief Sets the content displayed in the dialog
 *
 *  \iparam p_Program = Reference to the Program displayed in the dialog
 */
/****************************************************************************/
void CModifyProgramDlg::InitDialog(DataManager::CProgram const *p_Program)
{
    m_bIconSelected = false;
    if (p_Program != NULL) {
        m_Program = *(p_Program);
        m_strLastProgName = m_Program.GetName();
    }
    m_ProgramListClone = *(mp_DataConnector->ProgramList);
    if (m_StepModel.rowCount(QModelIndex()) > 0) {
        mp_TableWidget->selectRow(0);
    }
    m_StepModel.SetModifyProgramDlgPtr(this);
    ResizeHorizontalSection();
    QString str = m_Program.GetName();
    QString LongName = HandleEscapedChar(str);

    if (m_ButtonType == NEW_BTN_CLICKED)
    {
        mp_Ui->btnPrgIcon->setIcon(QIcon(""));
    }
    else if((m_ButtonType != COPY_BTN_CLICKED) && (m_Program.IsLeicaProgram() || m_Program.GetID().at(0) == 'C' ||
                                                   Core::CGlobalHelper::CheckIfCanEdit(m_Program.GetID(), 1) == false )) {
        if (Core::CGlobalHelper::CheckIfCanEdit(m_Program.GetID(), 1) == false) {
            mp_Ui->btnPrgIcon->setIcon(QIcon(QString(":/HimalayaImages/Icons/Program/"+m_Program.GetIcon()+".png")));
        }
        else {
            mp_Ui->btnPrgIcon->setIcon(QIcon(":/HimalayaImages/Icons/Program/Icon_Leica.png"));
        }
        mp_Ui->btnEdit->setEnabled(false);
        mp_Ui->btnCopy->setEnabled(false);
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->btnPrgName->setEnabled(false);
        mp_Ui->btnSave->setEnabled(false);
        ButtonPrgIconEnable(false);
    } else {
        mp_Ui->btnPrgIcon->setIcon(QIcon(QString(":/HimalayaImages/Icons/Program/"+m_Program.GetIcon()+".png")));
    }

    if (m_ButtonType == EDIT_BTN_CLICKED) {
        if (m_Program.GetNumberOfSteps() >= MAX_PROGRAM_STEPS){
             mp_Ui->btnNew->setEnabled(false);
        }

        mp_Ui->btnPrgName->setText(tr("%1").arg(LongName));
        // Pass a value same as the one passed to SetVisibleRows()

        m_StepModel.SetVisibleRowCount(6);
        m_StepModel.SetProgram(&m_Program, mp_DataConnector->SettingsInterface->GetUserSettings(), mp_DataConnector->ReagentGroupList,mp_DataConnector->ReagentList, 5);
        mp_TableWidget->setModel(&m_StepModel);
        m_RowIndex = m_Program.GetNumberOfSteps();
    }
    else if (m_ButtonType == COPY_BTN_CLICKED) {
        if (m_Program.IsLeicaProgram())
        {
            int idx = LongName.indexOf("leica", 0, Qt::CaseInsensitive);
            if (-1 != idx)
            {
                (void)LongName.remove(idx, 5);
            }
            LongName = LongName.trimmed();
            m_Program.SetLeicaProgram(false);
        }
        mp_Ui->btnPrgIcon->setIcon(QIcon(""));
        m_Program.SetIcon("");

        switch (LongName.length()) {
        case 18:
        case 19:
        case 20:
            LongName.replace(17, 3, "_cp");
            m_strLastProgName.replace(17, 3, "_cp");
            break;
        default:
            LongName.append("_cp");
            m_strLastProgName.append("_cp");
            break;
        }
        mp_Ui->btnPrgName->setText(tr("%1").arg(LongName));

        // Pass a value same as the one passed to SetVisibleRows()
        m_StepModel.SetVisibleRowCount(6);
        m_StepModel.SetProgram(&m_Program,mp_DataConnector->SettingsInterface->GetUserSettings(), mp_DataConnector->ReagentGroupList, mp_DataConnector->ReagentList, 5);
        mp_TableWidget->setModel(&m_StepModel);
        m_RowIndex = m_Program.GetNumberOfSteps();
    }
    else {
        NewProgram();
    }
}

/****************************************************************************/
/*!
 *  \brief Sets up the dialog for New Program
 */
/****************************************************************************/
void CModifyProgramDlg::NewProgram()
{
    if (mp_NewProgram)
        delete mp_NewProgram;

    mp_NewProgram = new DataManager::CProgram();
    m_Icon.clear();
    //Pass a value same as the one passed to SetVisibleRows()
    m_StepModel.SetVisibleRowCount(6);
    m_StepModel.SetProgram(NULL,NULL, NULL, NULL, 5);
    mp_TableWidget->setModel(&m_StepModel);
    m_StepModel.SetModifyProgramDlgPtr(this);
    mp_Ui->btnPrgName->setText("--");
}

/****************************************************************************/
/*!
 *  \brief Returns the data of the currently selected program step
 *
 *  \return Currently selected program step
 */
/****************************************************************************/
DataManager::CProgramStep *CModifyProgramDlg::SelectedStep()
{
    QModelIndex Index = mp_TableWidget->selectionModel()->selectedRows()[0];
    QString Id = m_StepModel.data(Index, (int)Qt::UserRole).toString();
    if (m_ButtonType == NEW_BTN_CLICKED) {
        return const_cast<DataManager::CProgramStep*>(mp_NewProgram->GetProgramStep(Id));
    }
    else {
        return const_cast<DataManager::CProgramStep*>(m_Program.GetProgramStep(Id));
    }
}

/****************************************************************************/
/*!
 *  \brief This function focuses the required row
 *
 *  \iparam CurrentRow - Row to be focused.
 */
/****************************************************************************/
void CModifyProgramDlg::SetRowFocus(qint32 CurrentRow)
{
    mp_TableWidget->selectRow(CurrentRow);
}

/****************************************************************************/
/*!
 *  \brief Connects signals and slots of keyboard.
 */
/****************************************************************************/
void CModifyProgramDlg::ConnectKeyBoardSignalSlots()
{
    if (mp_KeyBoardWidget) {
        // Connect signals and slots to keyboard.
        CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, OkButtonClicked(QString), this, OnOkClicked(QString));
        CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, EscButtonClicked(), this, OnESCClicked());
    }
}

/****************************************************************************/
/*!
 *  \brief Disconnects signals and slots of keyboard.
 */
/****************************************************************************/
void CModifyProgramDlg::DisconnectKeyBoardSignalSlots()
{
    if (mp_KeyBoardWidget) {
        // Disconnect signals and slots connected to keyboard.
        (void) disconnect(mp_KeyBoardWidget, SIGNAL(OkButtonClicked(QString)), this, SLOT(OnOkClicked(QString)));
        (void) disconnect(mp_KeyBoardWidget, SIGNAL(EscButtonClicked()), this, SLOT(OnESCClicked()));
    }
}


/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to edit the program name
 */
/****************************************************************************/
void CModifyProgramDlg::OnEditName()
{
    //    mp_KeyBoardWidget->Attach(this);
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(tr("Enter Program Name"));
    mp_KeyBoardWidget->SetPasswordMode(false);
    if (!(m_ButtonType == NEW_BTN_CLICKED)) {
        //mp_KeyBoardWidget->SetLineEditContent(mp_Ui->btnPrgName->text());
        mp_KeyBoardWidget->SetLineEditContent(m_strLastProgName);
    }
    else {
        if ((mp_Ui->btnPrgName->text() != "--")) {
            //mp_KeyBoardWidget->SetLineEditContent(mp_Ui->btnPrgName->text());
            mp_KeyBoardWidget->SetLineEditContent(m_strLastProgName);
        }
    }
    mp_KeyBoardWidget->SetMaxCharLength(MAX_LONGNAME_LENGTH);
    mp_KeyBoardWidget->SetMinCharLength(MIN_LONGNAME_LENGTH);
    mp_KeyBoardWidget->show();
    m_ProgNameBtnClicked  = true;
    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();
}

///****************************************************************************/
/*!
 *  \brief Shows the edit program step dialog
 */
/****************************************************************************/
void CModifyProgramDlg::OnEdit()
{
    DataManager::CProgramStep *p_ProgramStep = SelectedStep();

    mp_ModifyProgStepDlg->SetDialogTitle(tr("Edit Program Step"));
    mp_ModifyProgStepDlg->SetProgramStep(p_ProgramStep);

    if (p_ProgramStep) {
        QString Pressure = p_ProgramStep->GetPressure();
        QString Vacuum   = p_ProgramStep->GetVacuum();
        mp_ModifyProgStepDlg->SetRadioButtonStatus(Pressure, Vacuum);
    }

    mp_ModifyProgStepDlg ->SetButtonType(EDIT_BTN_CLICKED);
    mp_ModifyProgStepDlg->show();
}

/****************************************************************************/
/*!
 *  \brief Inserts a new program step and shows the edit dialog
 */
/****************************************************************************/
void CModifyProgramDlg::OnNewStep()
{
    mp_ModifyProgStepDlg->SetDialogTitle(tr("New Program Step"));
    mp_ModifyProgStepDlg->NewProgramStep();
    mp_ModifyProgStepDlg ->SetButtonType(NEW_BTN_CLICKED);
    mp_ModifyProgStepDlg->show();
}

/****************************************************************************/
/*!
 *  \brief Copies the currently selected program step
 */
/****************************************************************************/
void CModifyProgramDlg::OnCopy()
{
    DataManager::CProgramStep *p_ProgramStep = SelectedStep();

    mp_ModifyProgStepDlg->SetDialogTitle(tr("Edit Program Step"));
    mp_ModifyProgStepDlg->SetButtonType(COPY_BTN_CLICKED);
    mp_ModifyProgStepDlg->SetProgramStep(p_ProgramStep);

    if (p_ProgramStep) {
        QString Pressure = p_ProgramStep->GetPressure();
        QString Vacuum   = p_ProgramStep->GetVacuum();
        mp_ModifyProgStepDlg->SetRadioButtonStatus(Pressure, Vacuum);
    }

    mp_ModifyProgStepDlg->show();
}

/****************************************************************************/
/*!
 *  \brief Deletes the currently selected program step
 */
/****************************************************************************/
void CModifyProgramDlg::OnDelete()
{
    if (mp_MessageDlg) {
        delete mp_MessageDlg;
    }
    mp_MessageDlg = new MainMenu::CMessageDlg();
    mp_MessageDlg->SetTitle(m_strConfirmMsg);
    mp_MessageDlg->SetIcon(QMessageBox::Warning);
    mp_MessageDlg->SetButtonText(1, m_strYes);
    mp_MessageDlg->SetButtonText(3, m_strCancel);
    mp_MessageDlg->HideCenterButton();
    mp_MessageDlg->SetText(m_strDelProgramStep);

    if (mp_MessageDlg->exec() == (int)QDialog::Accepted) {
        if (m_ButtonType == NEW_BTN_CLICKED) {
            DeleteSelectedStep(mp_NewProgram);
            if(mp_NewProgram->GetNumberOfSteps()== 0){
                mp_Ui->btnDelete->setEnabled(false);
                mp_Ui->btnCopy->setEnabled(false);
                mp_Ui->btnEdit->setEnabled(false);
            }
        }
        else {
            DeleteSelectedStep(&m_Program);
            if (m_Program.GetNumberOfSteps()== 0) {
                mp_Ui->btnDelete->setEnabled(false);
                mp_Ui->btnCopy->setEnabled(false);
                mp_Ui->btnEdit->setEnabled(false);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Deletes the changes made in the dialog
 *  \iparam p_CurrentProgram = Reference to the selected program
 */
/****************************************************************************/
void CModifyProgramDlg::DeleteSelectedStep(DataManager::CProgram* p_CurrentProgram)
{
    int Index = mp_TableWidget->currentIndex().row();
    DataManager::CProgram *p_Program = p_CurrentProgram;

    // Added void to please lint
    (void) p_Program->DeleteProgramStep(Index);
    ResetButtons(*p_Program, false);
    mp_TableWidget->clearSelection();
}

/****************************************************************************/
/*!
 *  \brief Saves the changes made in the dialog
 */
/****************************************************************************/
void CModifyProgramDlg::OnSave()
{
    if (m_isInSaving)
        return;

    m_isInSaving = true;

    if (mp_MessageDlg) {
        delete mp_MessageDlg;
    }

    mp_MessageDlg = new MainMenu::CMessageDlg();
    mp_MessageDlg->SetTitle(m_strConfirmMsg);
    mp_MessageDlg->SetIcon(QMessageBox::Information);
    mp_MessageDlg->SetButtonText(1, m_strOK);
    mp_MessageDlg->HideButtons();
    if(mp_Ui->btnPrgName->text() == "--"){
        mp_MessageDlg->SetText(m_strEnterValidName);
        (void) mp_MessageDlg->exec();
        m_isInSaving = false;
        return;
    }

    if (m_strLastProgName.contains('&') && mp_Ui->btnPrgName->text().contains('&')) {
        m_Program.SetName(m_strLastProgName);
    }
    else {
        m_Program.SetName(mp_Ui->btnPrgName->text());
    }

    if (m_ButtonType == EDIT_BTN_CLICKED) {
        emit UpdateProgram(m_Program);
    }
    else
    {
        DataManager::CProgram* pProgram = NULL;
        if (m_ButtonType == COPY_BTN_CLICKED) {
            pProgram = &m_Program;
        }
        else //new program
        {
            mp_NewProgram->SetIcon(m_Icon);
            pProgram = mp_NewProgram;
        }

        //Check it has an icon?
        if (pProgram->GetIcon().isEmpty()) {
            mp_MessageDlg->SetText(m_strSeclectIcon);
            (void) mp_MessageDlg->exec();
            m_isInSaving = false;
            return;
        }

        if (m_ButtonType == COPY_BTN_CLICKED) {
            m_Program.SetFavorite(false);
            m_Program.SetNameID("");
            m_Program.SetBottleCheck(true);
            emit AddProgram(m_Program);
        }
        else {
            if (m_strLastProgName.contains('&') && mp_Ui->btnPrgName->text().contains('&')) {
                mp_NewProgram->SetName(m_strLastProgName);
            }
            else {
                mp_NewProgram->SetName(mp_Ui->btnPrgName->text());
            }

            emit AddProgram(*mp_NewProgram);
        }
    }
    m_isInSaving = false;
}

/****************************************************************************/
/*!
 *  \brief Cancels the changes made in the dialog
 */
/****************************************************************************/
void CModifyProgramDlg::OnCancel()
{
    emit CancelClicked();
    reject();
}


/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Currently selected cell
 */
/****************************************************************************/
void CModifyProgramDlg::OnSelectionChanged(QModelIndex Index)
{
    if (m_ButtonType == EDIT_BTN_CLICKED && Core::CGlobalHelper::CheckIfCanEdit(m_Program.GetID(), 1) == false)
        return;

    m_RowIndex = Index.row();

    QString Id = m_StepModel.data(Index, (int)Qt::UserRole).toString();
    //Inform the model about the current row.
    m_StepModel.SetIndex(Index);
    int SelectedIndex = Index.row();
    //Retrieving only process running state and not user role since
    //user role can't change behind the scene.
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {
        if (m_ButtonType == NEW_BTN_CLICKED){

            if ((SelectedIndex+1) > mp_NewProgram->GetNumberOfSteps()) {
                ResetButtons(*mp_NewProgram, false);
                mp_TableWidget->clearSelection();
            }
            else {
                ResetButtons(*mp_NewProgram, true);
            }
        }
        else {
            //Edit Mode
            if ((SelectedIndex+1) > m_Program.GetNumberOfSteps()) {
                if(!m_Program.IsLeicaProgram())
                    ResetButtons(m_Program, false);
                mp_TableWidget->clearSelection();
            }
            else {
                if(!m_Program.IsLeicaProgram())
                    ResetButtons(m_Program, true);
            }
        }
    }
}

void CModifyProgramDlg::OnUserRoleChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    if (m_CurrentUserRole == MainMenu::CMainWindow::Operator)
    {
        mp_Ui->btnSave->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process Running state changes
 */
/****************************************************************************/
void CModifyProgramDlg::OnProcessStateChanged()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {
        if (m_ButtonType == EDIT_BTN_CLICKED && Core::CGlobalHelper::CheckIfCanEdit(m_Program.GetID(), 1) == false) {
            // View Mode
            mp_Ui->btnPrgName->setEnabled(false);
            ButtonPrgIconEnable(false);
            mp_Ui->btnCancel->setEnabled(true);
            mp_Ui->btnDelete->setEnabled(false);
            mp_Ui->btnNew->setEnabled(false);
            mp_Ui->btnEdit->setEnabled(false);
            mp_Ui->btnCopy->setEnabled(false);
            mp_Ui->btnSave->setEnabled(false);
        }
        else {
            //Edit Mode
            mp_Ui->btnPrgName->setEnabled(true);
            ButtonPrgIconEnable(true);
            mp_Ui->btnNew->setEnabled(true);
            mp_Ui->btnCancel->setEnabled(true);
            mp_Ui->btnSave->setEnabled(true);
        }
    }
    else {
        if (m_ProcessRunning) {
            mp_Ui->btnSave->setEnabled(false);
        }
        else {
            //View Mode
            mp_Ui->btnPrgName->setEnabled(false);
            ButtonPrgIconEnable(false);
            mp_Ui->btnCancel->setEnabled(true);
            mp_Ui->btnDelete->setEnabled(false);
            mp_Ui->btnNew->setEnabled(false);
            mp_Ui->btnEdit->setEnabled(false);
            mp_Ui->btnCopy->setEnabled(false);
            mp_Ui->btnSave->setEnabled(false);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Esc button on Keyboard is clicked.
 */
/****************************************************************************/
void CModifyProgramDlg::OnESCClicked()
{

    m_ProgNameBtnClicked = false;
    m_ProgShortNameBtnClicked = false;
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
}

QString CModifyProgramDlg::HandleEscapedChar(QString& str)
{
    QString EscapedText;
    if (str.contains('&')) {
        int len = str.length();
        for (int i = 0; i < len; i ++ ) {
            QChar c = str.at(i);
            if (c == '&') {
                EscapedText.push_back("&&");
            }
            else {
                EscapedText.push_back(c);
            }
        }
        return EscapedText;
    }
    else {
        return str;
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when OK button on Keyboard is clicked.
 *
 *  \param EnteredText = QString type parameter
 */
/****************************************************************************/
void CModifyProgramDlg::OnOkClicked(QString EnteredText)
{
    mp_KeyBoardWidget->hide();
    if (m_ProgNameBtnClicked) {
        m_ProgNameBtnClicked = false;

        if (mp_MessageDlg) {
            delete mp_MessageDlg;
        }
        mp_MessageDlg = new MainMenu::CMessageDlg();
        mp_MessageDlg->SetTitle(QApplication::translate("Programs::CModifyProgramDlg",
                                                        "Program Name", Q_NULLPTR, -1));
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->SetButtonText(1, m_strOK);
        mp_MessageDlg->HideButtons();

        if (EnteredText.trimmed().isEmpty()){
            mp_MessageDlg->SetText(QApplication::translate("Programs::CModifyProgramDlg",
                                                           "The Program name cannot be empty!",
                                                           Q_NULLPTR, -1));
            (void) mp_MessageDlg->exec();
            return;
        } else if (EnteredText.contains("Leica", Qt::CaseInsensitive)) {
            mp_MessageDlg->SetText(QApplication::translate("Programs::CModifyProgramDlg",
                                "The Program name cannot contain \"leica\"", Q_NULLPTR, -1));
            (void) mp_MessageDlg->exec();
            return;
        }
        m_strLastProgName = EnteredText;
        if (EnteredText.contains('&')) {
            QString EscapedText = HandleEscapedChar(EnteredText);
            mp_Ui->btnPrgName->setText(tr("%1").arg(EscapedText));
        }
        else {
            mp_Ui->btnPrgName->setText(tr("%1").arg(EnteredText));
        }
    }
    else if (m_ProgShortNameBtnClicked) {
        m_ProgShortNameBtnClicked = false;
    }
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
}

void CModifyProgramDlg::ButtonPrgIconEnable(bool enable)
{
    if (!mp_Ui)
        return;

    QString enableIcon;
    if (enable)
    {
        enableIcon = "IconPushButton-Enabled.png";
    }
    else
    {
        enableIcon = "IconPushButton-Disabled.png";
    }
    mp_Ui->btnPrgIcon->setStyleSheet(mp_Ui->btnPrgIcon->property("defaultStyleSheet").toString()
                                     + "QToolButton { border-image: url(:HimalayaImages/IconPushButton/"
                                     + enableIcon + ");}");
    mp_Ui->btnPrgIcon->setEnabled(enable);
}

/****************************************************************************/
/*!
 *  \brief This event is called whenever widget is shown.Sets the states of
 *         various buttons as required for View and Edit Mode.
 *
 *   \iparam p_Event = Ref
 */
/****************************************************************************/
void CModifyProgramDlg::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);
    ResizeHorizontalSection();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {
        //Edit Mode
        if (m_ButtonType == NEW_BTN_CLICKED) {
            mp_Ui->btnPrgName->setEnabled(true);
            ButtonPrgIconEnable(true);
            mp_Ui->btnNew->setEnabled(true);
            mp_Ui->btnCancel->setEnabled(true);
            mp_Ui->btnSave->setEnabled(true);
            mp_Ui->btnDelete->setEnabled(false);
            mp_Ui->btnCopy->setEnabled(false);
            mp_Ui->btnEdit->setEnabled(false);
            mp_Ui->btnCancel->setText(m_strCancel);
        }
        else {
            if(!m_Program.IsLeicaProgram())
            {
                if (m_ButtonType == EDIT_BTN_CLICKED && Core::CGlobalHelper::CheckIfCanEdit(m_Program.GetID(), 1) == false) {
                    //View Mode
                    mp_Ui->btnPrgName->setEnabled(false);
                    ButtonPrgIconEnable(false);
                    mp_Ui->btnCancel->setEnabled(true);
                    mp_Ui->btnCopy->setEnabled(false);
                    mp_Ui->btnDelete->setEnabled(false);
                    mp_Ui->btnNew->setEnabled(false);
                    mp_Ui->btnEdit->setEnabled(false);
                    mp_Ui->btnSave->setEnabled(false);
                    mp_Ui->btnCancel->setText(m_strClose);
                }
                else {
                    int NumOfSteps = m_Program.GetNumberOfSteps();
                    if (NumOfSteps < MAX_PROGRAM_STEPS) {
                        mp_Ui->btnNew->setEnabled(true);
                    }
                    else
                        mp_Ui->btnNew->setEnabled(false);
                    mp_Ui->btnCancel->setEnabled(true);
                    mp_Ui->btnSave->setEnabled(true);
                    mp_Ui->btnDelete->setEnabled(false);
                    mp_Ui->btnCopy->setEnabled(false);
                    mp_Ui->btnEdit->setEnabled(false);
                    mp_Ui->btnPrgName->setEnabled(true);
                    mp_Ui->btnCancel->setText(m_strCancel);
                    ButtonPrgIconEnable(true);
                }
            }
            else
            {
                mp_Ui->btnCancel->setText(m_strClose);
            }
        }
    }
    else {
        //View Mode
        mp_Ui->btnPrgName->setEnabled(false);
        ButtonPrgIconEnable(false);
        mp_Ui->btnCancel->setEnabled(true);
        mp_Ui->btnCopy->setEnabled(false);
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->btnEdit->setEnabled(false);
        mp_Ui->btnSave->setEnabled(false);
        mp_Ui->btnCancel->setText(m_strClose);
    }
    mp_Ui->scrollTable->Reset();

}

/****************************************************************************/
/*!
 *  \brief This resets the buttons on the right of the screen.
 *
 *  \iparam CurrentProgram = Currently selected program
 *  \iparam SelectionChanged = True if selection is changed else False.
 */
/****************************************************************************/
void CModifyProgramDlg::ResetButtons(DataManager::CProgram &CurrentProgram, bool SelectionChanged)
{
    if (SelectionChanged) {
        if (CurrentProgram.GetNumberOfSteps() > 0 )
        {
            mp_Ui->btnDelete->setEnabled(true);
            mp_Ui->btnCopy->setEnabled(true);
            mp_Ui->btnEdit->setEnabled(true);
            mp_Ui->btnNew->setEnabled(true);
        }
        else {
            if (CurrentProgram.GetNumberOfSteps()>= MAX_PROGRAM_STEPS) {
                mp_Ui->btnNew->setEnabled(false);
                mp_Ui->btnCopy->setEnabled(false);
            }
            else {
                mp_Ui->btnDelete->setEnabled(false);
                mp_Ui->btnCopy->setEnabled(false);
                mp_Ui->btnEdit->setEnabled(false);
                mp_Ui->btnNew->setEnabled(true);
            }
        }
    }
    else {
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->btnCopy->setEnabled(false);
        mp_Ui->btnEdit->setEnabled(false);
        mp_Ui->btnNew->setEnabled(true);
        int NumOfSteps = CurrentProgram.GetNumberOfSteps();
        if (NumOfSteps >= MAX_PROGRAM_STEPS) {
            mp_Ui->btnNew->setEnabled(false);
            mp_Ui->btnCopy->setEnabled(false);
        }
    }

    if (CurrentProgram.GetNumberOfSteps()>= MAX_PROGRAM_STEPS) {
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->btnCopy->setEnabled(false);
    }
}
/********************************************************************************/
/*!
 *  \brief This slot is called when Ok button in ModifyProgramStep dialog is clicked.
 *
 *  \iparam p_PrgmStep = Reference to the program step
 *  \iparam AddNewProgramStep = True if New step is added  else False
 */
/********************************************************************************/
void CModifyProgramDlg::UpdateProgramStepTable(DataManager::CProgramStep *p_PrgmStep, bool AddNewProgramStep)
{
    //Add program Step expects the user to allocate memory for the object. It doesnt expect
    // an object in Stack.
    DataManager::CProgramStep *p_ProgramStep = new DataManager::CProgramStep();
    // Store the pointer in container , so that Destructor can delete them later
    m_ListOfProgramSteps << p_ProgramStep;
    *p_ProgramStep = *p_PrgmStep;
    static bool setNextFreeStepId = true;
    if (m_ButtonType == NEW_BTN_CLICKED) {
        if(setNextFreeStepId) {
            mp_NewProgram->SetNextFreeStepID("0");
            setNextFreeStepId = false;
        }
        if (!AddNewProgramStep) {
            (void) mp_NewProgram->UpdateProgramStep(p_ProgramStep);
        }
        else {
            p_ProgramStep->SetStepID(mp_NewProgram->GetNextFreeStepID(true));
            (void) mp_NewProgram->AddProgramStep(m_RowIndex, p_ProgramStep);
            m_StepModel.SetProgram(mp_NewProgram,mp_DataConnector->SettingsInterface->GetUserSettings(), mp_DataConnector->ReagentGroupList, mp_DataConnector->ReagentList, 5);
            m_StepModel.SetModifyProgramDlgPtr(this);
            mp_TableWidget->setModel(&m_StepModel);
            ResetButtons(*mp_NewProgram, false);
            m_RowIndex = mp_NewProgram->GetNumberOfSteps();
            qDebug() << "After new prgram button clicked. m_RowIndex = " << m_RowIndex << "\n";
        }
    }
    else {
           //User clicked edit button in program dialog.
           // Check if new Program step is being added to the selected Program else update the program step
           if (AddNewProgramStep) {
               p_ProgramStep->SetStepID(m_Program.GetNextFreeStepID(true));
               (void) m_Program.AddProgramStep(m_RowIndex, p_ProgramStep);

               m_StepModel.SetProgram(&m_Program,mp_DataConnector->SettingsInterface->GetUserSettings(), mp_DataConnector->ReagentGroupList, mp_DataConnector->ReagentList, 5);
               m_StepModel.SetModifyProgramDlgPtr(this);
               mp_TableWidget->setModel(&m_StepModel);
               ResetButtons(m_Program, false);
               m_RowIndex = m_Program.GetNumberOfSteps();
               qDebug() << "After new step button clicked. m_RowIndex = " << m_RowIndex << "\n";
           }
           else {
               (void) m_Program.UpdateProgramStep(p_ProgramStep);
           }
       }
}

/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of Program Icon
 */
/****************************************************************************/
void CModifyProgramDlg::OnIconClicked()
{
    /*m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."
                            "\nPlease close the dialog with \"Close\""));*/

    m_bIconSelected = false;
    mp_ModifyProgramIconDlg->SetDialogTitle(m_strSelectIcon);
    mp_ModifyProgramIconDlg->EnableAvailableIcon(&m_ProgramListClone);
    (void)mp_ModifyProgramIconDlg->exec();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CModifyProgramDlg::RetranslateUI()
{
    mp_ModifyProgStepDlg->SetDialogTitle(QApplication::translate("Programs::CModifyProgramDlg", "Edit Program Step", Q_NULLPTR, -1));
    mp_ModifyProgStepDlg->SetDialogTitle(QApplication::translate("Programs::CModifyProgramDlg", "New Program Step", Q_NULLPTR, -1));
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Programs::CModifyProgramDlg", "Enter Program Name", Q_NULLPTR, -1));

    // Added void to please lint
    (void) m_StepModel.setHeaderData(0,Qt::Horizontal,QApplication::translate("Programs::CStepModel", "Step", Q_NULLPTR, -1),0);
    (void) m_StepModel.setHeaderData(1,Qt::Horizontal,QApplication::translate("Programs::CStepModel", "Reagent", Q_NULLPTR, -1),0);
    (void) m_StepModel.setHeaderData(2,Qt::Horizontal,QApplication::translate("Programs::CStepModel", "Duration", Q_NULLPTR, -1),0);
    (void) m_StepModel.setHeaderData(3,Qt::Horizontal,QApplication::translate("Programs::CStepModel", "Temperature", Q_NULLPTR, -1),0);
    (void) m_StepModel.setHeaderData(4,Qt::Horizontal,QApplication::translate("Programs::CStepModel", "P/V", Q_NULLPTR, -1),0);

    m_strSelectIcon = QApplication::translate("Programs::CModifyProgramDlg", "Select Icon", Q_NULLPTR, -1);
    m_strConfirmMsg = QApplication::translate("Programs::CModifyProgramDlg", "Confirmation Message", Q_NULLPTR, -1);
    m_strYes = QApplication::translate("Programs::CModifyProgramDlg", "Yes", Q_NULLPTR, -1);
    m_strCancel = QApplication::translate("Programs::CModifyProgramDlg", "Cancel", Q_NULLPTR, -1);
    m_strDelProgramStep = QApplication::translate("Programs::CModifyProgramDlg", "Do you really want to delete the selected program step?", Q_NULLPTR, -1);
    m_strEnterValidName = QApplication::translate("Programs::CModifyProgramDlg", "Please enter a valid Program Name", Q_NULLPTR, -1);
    m_strSeclectIcon = QApplication::translate("Programs::CModifyProgramDlg", "Please select a Program Icon", Q_NULLPTR, -1);
    m_strClose = QApplication::translate("Programs::CModifyProgramDlg", "Close", Q_NULLPTR, -1);
    m_strOK = QApplication::translate("Programs::CModifyProgramDlg", "OK", Q_NULLPTR, -1);

}

void CModifyProgramDlg::EnablebtnSave()
{
    mp_Ui->btnSave->setEnabled(true);
}

} // end namespace Programs
