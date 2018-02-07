/****************************************************************************/
/*! \file ModifyReagentRMSDlg.cpp
 *
 *  \brief ModifyReagentDlg Implementation.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2011-08-22, 2013-01-17
 *   $Author:  $ M.Scherer, C.Adaragunchi, Swati Tiwari
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
#include "Reagents/Include/ModifyReagentRMSDlg.h"
#include "ui_ModifyReagentDlg.h"
#include "Dashboard/Include/CommonString.h"
#include "Reagents/Include/ReagentRMSWidget.h"

//lint -e613

namespace Reagents {

const QString REGEXP_NUMERIC_VALIDATOR  = "^[0-9]{1,5}$"; //!< Reg expression for the validator
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 *  \iparam p_KeyBoard = Reference to Keyboard widget
 *  \iparam p_MainWindow = Reference to MainWindow
 *  \iparam p_DataConnector = Reference to DataConnector
 */
/****************************************************************************/
CModifyReagentRMSDlg::CModifyReagentRMSDlg(QWidget *p_Parent, KeyBoard::CKeyBoard *p_KeyBoard, MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector) :
    MainMenu::CDialogFrame(p_Parent, p_MainWindow), mp_Ui(new Ui::CModifyReagentRMSDlg), mp_DataConnector(p_DataConnector),
    m_strCassettesUntilChange(tr("Cassettes until change")),
    m_strCyclesUntilChange(tr("Cycles until change")),
    m_strDaysUntilChange(tr("Days until change")),
    m_strEnterValidData(tr("Please enter valid data")),
    m_strSelectReagentGroup(tr("Please Select reagent group")),
    m_strReagentAddFailed(tr("")),
    m_strReagentCopyFailed(tr("")),
    m_strEnterCassetteValue(tr("Please enter the number of cassettes")),
    m_strEnterCycleValue(tr("Enter Cycle Value")),
    m_strEnterDayValue(tr("Enter Day Value")),
    m_strLastReagentName(tr(""))
{
    mp_Ui->setupUi(GetContentFrame());
    m_ReagentNameBtnClicked = false;
    m_CassetteValueBtnClicked = false;
    m_ProcessRunning = false ;
    mp_ReagentGroupList = NULL;
    m_SelectionFlag = false;

    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->setModel(&m_ReagentGroupModel);

    mp_Ui->scrollTable->SetContent(mp_TableWidget);

    mp_TableWidget->SetVisibleRows(8);
    m_ReagentGroupModel.SetVisibleRowCount(8);
    mp_TableWidget->horizontalHeader()->show();
    mp_TableWidget->setFocusPolicy(Qt::ClickFocus);
    ResizeHorizontalSection();

    CONNECTSIGNALSLOT(mp_TableWidget, pressed(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_Ui->btnCancel, clicked(), this, OnCancel());
    CONNECTSIGNALSLOT(mp_Ui->btnOk, clicked(), this, OnOk());
    CONNECTSIGNALSLOT(mp_Ui->buttonReagentName, clicked(), this, OnEditName());
    CONNECTSIGNALSLOT(mp_Ui->buttonValue, clicked(), this, OnEditCassetteValue());
    mp_KeyBoardWidget = p_KeyBoard;

    // Init Message dialog
    mp_MessageDlg = new MainMenu::CMessageDlg(this);
    mp_MessageDlg->SetTitle(m_strInforMsg);
    mp_MessageDlg->SetIcon(QMessageBox::Information);
    mp_MessageDlg->SetButtonText(1, m_strOK);
    mp_MessageDlg->HideButtons();
    m_RMSOption = Global::RMS_OFF;
    if (mp_MainWindow)
    {
        CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
    }
}

/****************************************************************************/
/*!
 *  \brief Resizes the columns in the reagents table
 */
/****************************************************************************/
void CModifyReagentRMSDlg::ResizeHorizontalSection()
{
    if (mp_TableWidget == NULL)
    {
        return;
    }
    mp_TableWidget->horizontalHeader()->resizeSection(0, 170);
    mp_TableWidget->horizontalHeader()->resizeSection(1, 100);
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Currently selected cell
 */
/****************************************************************************/
void CModifyReagentRMSDlg::SelectionChanged(QModelIndex Index)
{
     QString Id = m_ReagentGroupModel.data(Index, (int)Qt::UserRole).toString();
     if (Id.isEmpty())
         return;

     mp_TableWidget->selectRow(Index.row());

     if (m_ButtonType == Reagents::NEW_BTN_CLICKED)
        m_SelectionFlag = true;

    mp_Ui->buttonValue->setEnabled(true);
    m_Reagent.SetGroupID(Id);

    Global::RMSOptions_t Option = Reagents::CReagentRMSWidget::RMSPROCESSINGOPTION;
    if (mp_DataConnector->ReagentGroupList->GetReagentGroup(Id)->IsCleaningReagentGroup())
    {
         Option = Reagents::CReagentRMSWidget::RMSCLEANINGOPTIONS;
    }
    UpdateRmsLabel(Option);
}

QString CModifyReagentRMSDlg::HandleEscapedChar(QString& str)
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
 *  \brief Destructor
 */
/****************************************************************************/
CModifyReagentRMSDlg::~CModifyReagentRMSDlg()
{
    try {        
        delete mp_Ui;
        delete mp_MessageDlg;
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
void CModifyReagentRMSDlg::changeEvent(QEvent *p_Event)
{
    QDialog::changeEvent(p_Event);
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
 *  \brief Sets the content of the dialog
 *
 *  \param p_Reagent = Reference of the reagent to be edited
 *  \param p_ReagentGroupList = all reagent groups
 *  \param Option = RMS option
 *  \param bShowCleaningReagent = whether show CleaningReagent in the table or not
 */
/****************************************************************************/
void CModifyReagentRMSDlg::InitDialog(DataManager::CReagent const *p_Reagent,
                                   DataManager::CDataReagentGroupList const *p_ReagentGroupList,
                                      Global::RMSOptions_t Option,
                                      bool bShowCleaningReagent)
{
    if (p_ReagentGroupList != NULL) {
        mp_ReagentGroupList = const_cast<DataManager::CDataReagentGroupList*>(p_ReagentGroupList);
        m_ReagentGroupModel.SetReagentGroupList(mp_ReagentGroupList, 2, bShowCleaningReagent);
        ResizeHorizontalSection();
        // always select the first one
        //mp_TableWidget->selectRow(0);
    }
    m_RMSOption = Option;
    mp_TableWidget->setEnabled(true);
    if (p_Reagent != NULL) {
        m_Reagent = *p_Reagent;
        // Check if Edit button was clicked in ReagentWidget
        if (m_ButtonType == Reagents::EDIT_BTN_CLICKED) {
            mp_Ui->buttonReagentName->setEnabled(m_Reagent.GetReagentID().at(0) != 'L');
            m_strLastReagentName = m_Reagent.GetReagentName();
            mp_Ui->buttonReagentName->setText(HandleEscapedChar(m_strLastReagentName));
            mp_Ui->buttonValue->setEnabled(true);
            mp_Ui->buttonValue->setVisible(true);
            mp_Ui->labelRMSStaticName->setVisible(true);
            UpdateRmsLabel(Option);
            ShowReagentValue(Option);
            mp_TableWidget->selectRow(mp_ReagentGroupList->GetReagentGroupIndex(m_Reagent.GetGroupID()));
            if (mp_DataConnector->ProgramList->GetReagentIDList().contains(m_Reagent.GetReagentID()))
            {
                mp_TableWidget->setEnabled(false);
            }
            else
            {
                mp_TableWidget->setEnabled(true);
            }

        }

    }
    else {//new reagent
        mp_Ui->buttonReagentName->setEnabled(true);
        UpdateRmsLabel(Option);
        mp_Ui->buttonValue->setEnabled(false);
        if(Option == Global::RMS_OFF){
            mp_Ui->buttonValue->setVisible(false);
            mp_Ui->labelRMSStaticName->setVisible(false);
            mp_Ui->buttonReagentName->setText("--");
        }
        else {
            mp_Ui->buttonReagentName->setText("--");
            mp_Ui->buttonValue->setVisible(true);
            mp_Ui->labelRMSStaticName->setVisible(true);
            mp_Ui->buttonValue->setText("--");
        }
    }    
}

void CModifyReagentRMSDlg::UpdateRmsLabel(Global::RMSOptions_t Option)
{
    switch (Option) {
        case Global::RMS_CASSETTES:
            mp_Ui->labelRMSStaticName->setText(m_strCassettesUntilChange);
            break;
        case Global::RMS_CYCLES:
            mp_Ui->labelRMSStaticName->setText(m_strCyclesUntilChange);
            break;
        case Global::RMS_DAYS:
            mp_Ui->labelRMSStaticName->setText(m_strDaysUntilChange);
            break;
        case Global::RMS_OFF:
            mp_Ui->buttonValue->setVisible(false);
            mp_Ui->labelRMSStaticName->setVisible(false);
            break;
        default:
            break;
    }
}

void CModifyReagentRMSDlg::ShowReagentValue(Global::RMSOptions_t Option)
{
    switch (Option) {
        case Global::RMS_CASSETTES:
            mp_Ui->buttonValue->setText(QString::number(m_Reagent.GetMaxCassettes()));
            break;
        case Global::RMS_CYCLES:
            mp_Ui->buttonValue->setText(QString::number(m_Reagent.GetMaxCycles()));
            break;
        case Global::RMS_DAYS:
            mp_Ui->buttonValue->setText(QString::number(m_Reagent.GetMaxDays()));
            break;
        case Global::RMS_OFF:
            mp_Ui->buttonValue->setVisible(false);
            break;
        default:
            break;
    }
}


/****************************************************************************/
/*!
 *  \brief This function is called when ESC button on keyboard is clicked
 */
/****************************************************************************/
void CModifyReagentRMSDlg::OnESCClicked()
{
    m_ReagentNameBtnClicked = false;
}

/****************************************************************************/
/*!
 *  \brief Connects signals and slots of keyboard.
 */
/****************************************************************************/
void CModifyReagentRMSDlg::ConnectKeyBoardSignalSlots()
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
void CModifyReagentRMSDlg::DisconnectKeyBoardSignalSlots()
{
    if (mp_KeyBoardWidget) {
        // Disconnect signals and slots connected to keyboard.
        (void) disconnect(mp_KeyBoardWidget, SIGNAL(OkButtonClicked(QString)), this, SLOT(OnOkClicked(QString)));
        (void) disconnect(mp_KeyBoardWidget, SIGNAL(EscButtonClicked()), this, SLOT(OnESCClicked()));
    }
}

/****************************************************************************/
/*!
 *  \brief Cancels the changes in the dialog
 */
/****************************************************************************/
void CModifyReagentRMSDlg::OnCancel()
{
    emit CancelPressed();
    mp_TableWidget->reset();
    reject();
}

/****************************************************************************/
/*!
 *  \brief Saves the changes in the dialog
 */
/****************************************************************************/
void CModifyReagentRMSDlg::OnOk()
{
    // Clone the DataManager ReagentList
    m_ReagentCloneList = *(mp_DataConnector->ReagentList);

    if (mp_Ui->buttonReagentName->text() == "--") {//as "--" is initialized when dialog is showing
        mp_MessageDlg->SetText(m_strEnterValidName);
        mp_MessageDlg->SetButtonText(1, m_strOK);
        (void) mp_MessageDlg->exec();
        return;
    }

    if (mp_Ui->buttonReagentName->text().contains("LEICA",Qt::CaseInsensitive)) {
        mp_MessageDlg->SetText(m_strReagentNameHasLaicaString);
        mp_MessageDlg->SetButtonText(1, m_strOK);
        (void) mp_MessageDlg->exec();
        return;
    }

    if ((m_RMSOption != Global::RMS_OFF) && (mp_Ui->buttonValue->text().toInt() == 0)) {
        mp_MessageDlg->SetText(m_strEnterValidData);
        mp_MessageDlg->SetButtonText(1, m_strOK);
        (void) mp_MessageDlg->exec();
        return;
    }

    // Check if Edit button was clicked in ReagentWidget
    if (m_ButtonType == Reagents::EDIT_BTN_CLICKED) {
        m_SelectionFlag = false;
        //m_Reagent.SetReagentName(mp_Ui->buttonReagentName->text());
        m_Reagent.SetReagentName(m_strLastReagentName);
        switch (m_RMSOption) {
            case Global::RMS_CASSETTES:
                m_Reagent.SetMaxCassettes(mp_Ui->buttonValue->text().toInt());
                break;
            case Global::RMS_CYCLES:
                m_Reagent.SetMaxCycles(mp_Ui->buttonValue->text().toInt());
                break;
            case Global::RMS_DAYS:
                m_Reagent.SetMaxDays(mp_Ui->buttonValue->text().toInt());
                break;
            default:
                break;
        }
        emit UpdateReagent(m_Reagent);
    }
    // Else New button is clicked in ReagentWidget
    else
    {
        if(!m_SelectionFlag)
        {
            mp_MessageDlg->SetText(m_strSelectReagentGroup);//Please Select reagent group
            mp_MessageDlg->SetButtonText(1, m_strOK);
            (void) mp_MessageDlg->exec();
            return;
        }

        QString Id = m_ReagentCloneList.GetNextFreeReagentID(true);
        m_Reagent.SetReagentID(Id);
        //m_Reagent.SetReagentName(mp_Ui->buttonReagentName->text());
        m_Reagent.SetReagentName(m_strLastReagentName);
        Global::RMSOptions_t option;
        if (mp_DataConnector->ReagentGroupList->GetReagentGroup(m_Reagent.GetGroupID())->IsCleaningReagentGroup())
        {
             option = Reagents::CReagentRMSWidget::RMSCLEANINGOPTIONS;
        }
        else
        {
            option = m_RMSOption;
        }
        switch (option) {
            case Global::RMS_CASSETTES:
                m_Reagent.SetMaxCassettes(mp_Ui->buttonValue->text().toInt());
                m_Reagent.SetMaxCycles(0);
                m_Reagent.SetMaxDays(0);
                break;
            case Global::RMS_CYCLES:
                m_Reagent.SetMaxCassettes(0);
                m_Reagent.SetMaxCycles(mp_Ui->buttonValue->text().toInt());
                m_Reagent.SetMaxDays(0);
                break;
            case Global::RMS_DAYS:
                m_Reagent.SetMaxCassettes(0);
                m_Reagent.SetMaxCycles(0);
                m_Reagent.SetMaxDays(mp_Ui->buttonValue->text().toInt());
                break;
            default:
                break;
        }
        emit AddReagent(m_Reagent);
    }
}

/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to edit the program name
 */
/****************************************************************************/
void CModifyReagentRMSDlg::OnEditName()
{
    m_ReagentNameBtnClicked = true;
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(tr("Enter Reagent Name"));
    mp_KeyBoardWidget->SetLineEditContent(tr(""));

    if (mp_Ui->buttonReagentName->text() != "--") {
        //mp_KeyBoardWidget->SetLineEditContent(mp_Ui->buttonReagentName->text());
        mp_KeyBoardWidget->SetLineEditContent(m_strLastReagentName);
    }

    mp_KeyBoardWidget->SetPasswordMode(false);
    mp_KeyBoardWidget->SetMaxCharLength(20);
    mp_KeyBoardWidget->SetMinCharLength(1);
    mp_KeyBoardWidget->show();
    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to edit the program name
 */
/****************************************************************************/
void CModifyReagentRMSDlg::OnEditCassetteValue()
{
    m_CassetteValueBtnClicked = true;
    switch (m_RMSOption) {
        case Global::RMS_CASSETTES:
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(m_strEnterCassetteValue);
            break;
        case Global::RMS_CYCLES:
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(m_strEnterCycleValue);
            break;
        case Global::RMS_DAYS:
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(m_strEnterDayValue);
            break;
        default:
            break;

    }

    if (mp_Ui->buttonValue->text() != "--") {
        mp_KeyBoardWidget->SetLineEditContent(mp_Ui->buttonValue->text());
    }

    mp_KeyBoardWidget->SetPasswordMode(false);
    mp_KeyBoardWidget->SetMaxCharLength(32);
    mp_KeyBoardWidget->SetMinCharLength(1);
	mp_KeyBoardWidget->SetLineEditValidatorExpression(REGEXP_NUMERIC_VALIDATOR);
    mp_KeyBoardWidget->DisplayNumericKeyBoard();
    mp_KeyBoardWidget->show();
    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();

}

/****************************************************************************/
/*!
 *  \brief This slot is called when OK button on Keyboard is clicked.
 *
 *  \param EnteredText = QString type parameter
 */
/****************************************************************************/
void CModifyReagentRMSDlg::OnOkClicked(QString EnteredText)
{
    Q_UNUSED(EnteredText);
    QString LineEditString;
    mp_KeyBoardWidget->hide();
    if (m_ReagentNameBtnClicked){
        m_ReagentNameBtnClicked = false;
        LineEditString = mp_KeyBoardWidget->GetLineEditString();

        m_strLastReagentName = LineEditString;

        if (LineEditString.contains('&')) {
            QString EscapedText = HandleEscapedChar(LineEditString);
            mp_Ui->buttonReagentName->setText(tr("%1").arg(EscapedText));
        }
        else {
            mp_Ui->buttonReagentName->setText(LineEditString);
        }
    }
    else if (m_CassetteValueBtnClicked){
        m_CassetteValueBtnClicked = false;
        LineEditString = mp_KeyBoardWidget->GetLineEditString();
        mp_Ui->buttonValue->setText(LineEditString);
    }

    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CModifyReagentRMSDlg::RetranslateUI()
{
    m_strInforMsg = QApplication::translate("Reagents::CModifyReagentRMSDlg", "Information Message", Q_NULLPTR, -1);
    m_strOK = QApplication::translate("Reagents::CModifyReagentRMSDlg", "OK", Q_NULLPTR, -1);

    mp_MessageDlg->SetTitle(m_strInforMsg);
    mp_MessageDlg->SetButtonText(1, m_strOK);

    m_strCassettesUntilChange = QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                          "Cassettes until change", Q_NULLPTR, -1);

    m_strCyclesUntilChange = QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                     "Cycles until change", Q_NULLPTR, -1);

    m_strDaysUntilChange = QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                     "Days until change", Q_NULLPTR, -1);

    m_strEnterValidName =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                           "Please enter the reagent Name", Q_NULLPTR, -1);

    m_strReagentNameHasLaicaString =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                   "Reagent name shall not contain 'leica'.", Q_NULLPTR, -1);

    m_strInputReagentSameName =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                         "The same reagent name is in use already. Please change the reagent name.", Q_NULLPTR, -1);

    m_strEnterValidData =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                               "Please enter valid data", Q_NULLPTR, -1);

    m_strSelectReagentGroup =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                               "Please select reagent group", Q_NULLPTR, -1);

    m_strReagentAddFailed =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                               "Failed to add the reagent. Please contact service.", Q_NULLPTR, -1);

    m_strReagentCopyFailed =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                               "Failed to copy the reagent. Please contact service.", Q_NULLPTR, -1);

    m_strEnterCassetteValue =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                       "Please enter the number of cassettes", Q_NULLPTR, -1);

    m_strEnterCycleValue =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                               "Enter Cycle Value", Q_NULLPTR, -1);

    m_strEnterDayValue =  QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                                               "Enter Day Value", Q_NULLPTR, -1);

    mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Reagents::CModifyReagentRMSDlg",
                                              "Enter Reagent Name", Q_NULLPTR, -1));
}

void CModifyReagentRMSDlg::OnUserRoleChanged()
{
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
    {
        mp_Ui->btnOk->setEnabled(false);
    }
    else
        mp_Ui->btnOk->setEnabled(true);
}

} // end namespace Reagents
