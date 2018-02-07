/****************************************************************************/
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/SliderControl.h"
#include "Reagents/Include/ReagentStatusWidget.h"
#include "Reagents/Include/ModifyReagentStationDlg.h"
#include "ui_ReagentStatusWidget.h"
#include "Core/Include/GlobalHelper.h"


namespace Reagents {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Reference to Parent object
 *
 *  \return return from CReagentStatusWidget
 */
/****************************************************************************/
CReagentStatusWidget::CReagentStatusWidget(QWidget *p_Parent):
                                           MainMenu::CPanelFrame(p_Parent),
                                           mp_Ui(new Ui::CReagentStatusWidget),
                                           mp_DataConnector(NULL),
                                           mp_ReagentList(NULL),
                                           mp_Reagent(NULL),
                                           mp_DashStation(NULL),
                                           m_RMSOptions(Global::RMS_UNDEFINED),
                                           m_RMSCleaningOptions(Global::RMS_UNDEFINED),
                                           m_EnableBottleCheck(false)

{
    mp_Ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("Status"));

    m_ShowMessageDialog = false;
    m_ProcessRunning = false;

    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->setModel(&m_ReagentStatusModel);
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();

    mp_Ui->scrollTable->SetContent(mp_TableWidget);

    PopulateReagentList();
    CONNECTSIGNALSLOT(mp_TableWidget, pressed(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_Ui->btnEmpty, clicked(), this, OnSetAsEmpty());
    CONNECTSIGNALSLOT(mp_Ui->btnFull, clicked(), this, OnSetAsFull());
    CONNECTSIGNALSLOT(mp_Ui->btnReset, clicked(), this, OnResetData());
    CONNECTSIGNALSLOT(this, UpdateReagentList(), &m_ReagentStatusModel, UpdateReagentList());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CReagentStatusWidget::~CReagentStatusWidget()
{
    try {
        delete mp_TableWidget;
        delete mp_Ui;
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
void CReagentStatusWidget::changeEvent(QEvent *p_Event)
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
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CReagentStatusWidget::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{
    m_ReagentStatusModel.SetUserSettings(p_UserSettings);
}

/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of a reagent
 */
/****************************************************************************/
void CReagentStatusWidget::OnSetAsEmpty()
{
        MainMenu::CMessageDlg ConfirmationMessageDlg;

        ConfirmationMessageDlg.SetTitle(m_strConfirmMsg);
        QString strTemp = m_strSetAsEmpty.arg(m_CurrentStationName);
        ConfirmationMessageDlg.SetText(strTemp);
        ConfirmationMessageDlg.SetIcon(QMessageBox::Warning);
        ConfirmationMessageDlg.SetButtonText(1, m_strYes);
        ConfirmationMessageDlg.SetButtonText(3, m_strNo);
        ConfirmationMessageDlg.HideCenterButton();
        if(ConfirmationMessageDlg.exec() == (int)QDialog::Accepted)
        {
            bool bRevertSelectedProgram = false;
            if (!Core::CGlobalHelper::CheckSelectedProgram(bRevertSelectedProgram))
                return;

            if (bRevertSelectedProgram)
                emit UnselectProgram();

            emit UpdateStationSetAsEmpty(mp_DashStation->GetDashboardStationID());
            ResetButtons();
            mp_Ui->btnFull->setEnabled(true);
        }
}

/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of a reagent
 */
/****************************************************************************/
void CReagentStatusWidget::OnResetData()
{
    MainMenu::CMessageDlg ConfirmationMessageDlg;

    ConfirmationMessageDlg.SetTitle(m_strConfirmMsg);
    ConfirmationMessageDlg.SetText(m_strResetData.arg(m_CurrentStationName));
    ConfirmationMessageDlg.SetIcon(QMessageBox::Warning);
    ConfirmationMessageDlg.SetButtonText(1, m_strYes);
    ConfirmationMessageDlg.SetButtonText(3, m_strNo);
    ConfirmationMessageDlg.HideCenterButton();
    if(ConfirmationMessageDlg.exec() == (int)QDialog::Accepted)
    {
        bool bRevertSelectedProgram = false;
        if (!Core::CGlobalHelper::CheckSelectedProgram(bRevertSelectedProgram))
            return;

        if (bRevertSelectedProgram)
            emit UnselectProgram();

        emit UpdateStationResetData(mp_DashStation->GetDashboardStationID());
        if (m_EnableBottleCheck && !Core::CGlobalHelper::GetSystemErrorStatus())
        {
            MainMenu::CMessageDlg ConfirmationMsgDlg;
            ConfirmationMsgDlg.SetText(m_strBottleCheckConfirm);
            ConfirmationMsgDlg.SetIcon(QMessageBox::Warning);
            ConfirmationMsgDlg.SetButtonText(1, m_strYes);
            ConfirmationMsgDlg.SetButtonText(3, m_strNo);
            ConfirmationMsgDlg.HideCenterButton();
            if(ConfirmationMsgDlg.exec() == (int)QDialog::Accepted)
            {
                emit BottleCheck();
            }
        }
    }


}

/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of a reagent
 */
/****************************************************************************/
void CReagentStatusWidget::OnSetAsFull()
{
    MainMenu::CMessageDlg ConfirmationMessageDlg;

    ConfirmationMessageDlg.SetTitle(m_strConfirmMsg);
    ConfirmationMessageDlg.SetText(m_strSetAsFull.arg(m_CurrentStationName));
    ConfirmationMessageDlg.SetIcon(QMessageBox::Warning);
    ConfirmationMessageDlg.SetButtonText(1, m_strYes);
    ConfirmationMessageDlg.SetButtonText(3, m_strNo);
    ConfirmationMessageDlg.HideCenterButton();
    if(ConfirmationMessageDlg.exec() == (int)QDialog::Accepted)
    {
        bool bRevertSelectedProgram = false;
        if (!Core::CGlobalHelper::CheckSelectedProgram(bRevertSelectedProgram))
            return;

        if (bRevertSelectedProgram)
            emit UnselectProgram();

        emit UpdateStationSetAsFull(mp_DashStation->GetDashboardStationID());
        ResetButtons();
        mp_Ui->btnEmpty->setEnabled(true);
        mp_Ui->btnReset->setEnabled(true);
        if (m_EnableBottleCheck && !Core::CGlobalHelper::GetSystemErrorStatus())
        {
            ConfirmationMessageDlg.SetText(m_strBottleCheckConfirm);
            ConfirmationMessageDlg.SetIcon(QMessageBox::Warning);
            ConfirmationMessageDlg.SetButtonText(1, m_strYes);
            ConfirmationMessageDlg.SetButtonText(3, m_strNo);
            ConfirmationMessageDlg.HideCenterButton();
            if(ConfirmationMessageDlg.exec() == (int)QDialog::Accepted)
            {
                emit BottleCheck();
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of a reagent
 *
 *  \param RMSValue =  Global::RMSOptions_t type parameter
 */
/****************************************************************************/
void CReagentStatusWidget::RMSChanged(const Global::RMSOptions_t RMSValue)
{
    m_ReagentStatusModel.SetRMSOption(RMSValue);
    m_RMSOptions = RMSValue;
    ControlColumnShow();
}

void CReagentStatusWidget::RMSCleaningChanged(const Global::RMSOptions_t RMSValue)
{
    m_ReagentStatusModel.SetRMSCleaningOption(RMSValue);
    m_RMSCleaningOptions = RMSValue;
    ControlColumnShow();
}

void CReagentStatusWidget::ControlColumnShow()
{
    if (Global::RMS_OFF == m_RMSOptions && Global::RMS_OFF == m_RMSCleaningOptions)
    {
        mp_TableWidget->setColumnHidden(2, true);
        mp_TableWidget->setColumnHidden(3, true);
        return;
    }

    if ((m_RMSOptions == m_RMSCleaningOptions) || (Global::RMS_OFF == m_RMSCleaningOptions))
    {
        mp_TableWidget->setColumnHidden(2, false);//show
        mp_TableWidget->setColumnHidden(3, true);//hide
        return;
    }

    if (Global::RMS_OFF == m_RMSOptions)
    {
        mp_TableWidget->setColumnHidden(2, true);//hide
        mp_TableWidget->setColumnHidden(3, false);//show
        return;
    }

    if (m_RMSOptions != m_RMSCleaningOptions)
    {
        mp_TableWidget->setColumnHidden(2, false);//show
        mp_TableWidget->setColumnHidden(3, false);//show
    }
}

/****************************************************************************/
/*!
 *  \brief Fills the reagents list with content
 */
/****************************************************************************/
void CReagentStatusWidget::PopulateReagentList()
{
    mp_TableWidget->SetVisibleRows(7, 64, 40);
    m_ReagentStatusModel.SetVisibleRowCount(7);
    mp_TableWidget->horizontalHeader()->show();
    ResizeHorizontalSection();
}

/****************************************************************************/
/*!
 *  \brief Resizes the columns in the reagents table
 */
/****************************************************************************/
void CReagentStatusWidget::ResizeHorizontalSection()
{
    if (mp_TableWidget == NULL)
    {
        return;
    }
    mp_TableWidget->horizontalHeader()->resizeSection(0, 70);
    mp_TableWidget->horizontalHeader()->resizeSection(1, 120);
    mp_TableWidget->horizontalHeader()->resizeSection(2, 100);
    mp_TableWidget->horizontalHeader()->resizeSection(3, 100);
    mp_TableWidget->horizontalHeader()->resizeSection(4, 95);
    mp_TableWidget->horizontalHeader()->resizeSection(5, 60);
}

bool CReagentStatusWidget::IsReagentExpired()
{
    if (!mp_DataConnector)
        return false;

    if (!mp_Reagent)
        return false;
    Global::RMSOptions_t rmsMode;
    bool bExpireReagent = false;
    bool isCleaningReagentGroup = mp_DataConnector->ReagentGroupList->GetReagentGroup(mp_Reagent->GetGroupID())->IsCleaningReagentGroup();
    if (isCleaningReagentGroup)
    {
       rmsMode = m_RMSCleaningOptions;
    }
    else
    {
        rmsMode = m_RMSOptions;
    }

    DataManager::ReagentStatusType_t ReagentStatus = mp_DashStation->GetReagentStatus(*mp_Reagent, rmsMode);
    if ( ReagentStatus == DataManager::REAGENT_STATUS_EXPIRED )
        bExpireReagent = true;
    else
        bExpireReagent = false;

    return bExpireReagent;
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Currently selected cell
 */
/****************************************************************************/
void CReagentStatusWidget::SelectionChanged(QModelIndex Index)
{
    m_CurrentIndex = Index;
    QString Id = m_ReagentStatusModel.data(Index, (int)Qt::UserRole).toString();
    mp_DashStation = const_cast<DataManager::CDashboardStation*>(mp_DataConnector->DashboardStationList->GetDashboardStation(Id));

    if (Core::CGlobalHelper::CheckIfCanEdit(Id, 4)==false) {
        mp_Ui->btnFull->setEnabled(false);
        mp_Ui->btnEmpty->setEnabled(false);
        mp_Ui->btnReset->setEnabled(false);
        return;
    }

    if (mp_DashStation) {
        m_CurrentStationName = mp_DashStation->GetDashboardStationName();
        mp_Reagent = const_cast<DataManager::CReagent*>(mp_ReagentList->GetReagent(mp_DashStation->GetDashboardReagentID()));

        if (Global::RMS_OFF == m_RMSOptions && Global::RMS_OFF == m_RMSCleaningOptions)
        {
            mp_TableWidget->setColumnHidden(2, true);
            mp_TableWidget->setColumnHidden(3, true);
        }
        else
       {
            if(IsReagentExpired())
                mp_TableWidget->setStyleSheet("QTableView::item:selected {background-color:#D43032;""border-style:default;color:yellow}""QHeaderView {color:black;}");
            else
                mp_TableWidget->setStyleSheet("QHeaderView {color:black;}");
        }

        if (!m_ProcessRunning) {
            mp_Ui->btnFull->setEnabled(true);
            mp_Ui->btnEmpty->setEnabled(true);
            mp_Ui->btnReset->setEnabled(true);

            if(mp_DashStation->GetDashboardReagentID().compare("",Qt::CaseInsensitive) == 0) {
                mp_Ui->btnFull->setEnabled(false);
                mp_Ui->btnEmpty->setEnabled(false);
                mp_Ui->btnReset->setEnabled(false);
            }

            if (mp_DashStation->GetDashboardReagentStatus().compare("Full", Qt::CaseInsensitive) == 0) {
                mp_Ui->btnFull->setEnabled(false);
            }
            else if (mp_DashStation->GetDashboardReagentStatus().compare("Empty", Qt::CaseInsensitive) == 0) {
                mp_Ui->btnEmpty->setEnabled(false);
                mp_Ui->btnReset->setEnabled(false);
            }
        }
        else {
            mp_Ui->btnFull->setEnabled(false);
            mp_Ui->btnEmpty->setEnabled(false);
            mp_Ui->btnReset->setEnabled(false);
        }
    }
    else
    {
        mp_Ui->btnFull->setEnabled(false);
        mp_Ui->btnEmpty->setEnabled(false);
        mp_Ui->btnReset->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CReagentStatusWidget::OnUserRoleChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CReagentStatusWidget::OnProcessStateChanged()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if (!m_ProcessRunning) {
        //Edit Mode
        if(mp_DashStation) {
            ResetButtons();
            if ("Full" == mp_DashStation->GetDashboardReagentStatus())
            {
                mp_Ui->btnEmpty->setEnabled(true);
                mp_Ui->btnReset->setEnabled(true);
            }
            else if ("Empty" == mp_DashStation->GetDashboardReagentStatus())
            {
                mp_Ui->btnFull->setEnabled(true);
                mp_Ui->btnReset->setEnabled(true);
            }
        }
    }
    else {
        mp_Ui->btnFull->setEnabled(false);
        mp_Ui->btnEmpty->setEnabled(false);
        mp_Ui->btnReset->setEnabled(false);
    }
}

/*****************************************************************************/
/*!
  *  \brief Translates the strings in UI to the selected language
  */
/****************************************************************************/
void CReagentStatusWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Reagent::CReagentStatusWidget",
                                                                 "Status", Q_NULLPTR, -1));

    m_strSetAsEmpty = QApplication::translate("Reagent::CReagentStatusWidget",
                                              "Would you like to set Station %1 as empty?", Q_NULLPTR, -1);

    m_strResetData = QApplication::translate("Reagent::CReagentStatusWidget",
                                             "Would you like to reset data for Station %1?", Q_NULLPTR, -1);

    m_strSetAsFull = QApplication::translate("Reagent::CReagentStatusWidget",
                                             "Would you like to set Station %1 as full?", Q_NULLPTR, -1);

    m_strBottleCheckConfirm = QApplication::translate("Reagent::CReagentStatusWidget",
                                             "Please check that the retort is empty and clean prior starting bottle check and then press \"yes\". Bottle check function will require approximately 7 minutes.", Q_NULLPTR, -1);

    m_strConfirmMsg = QApplication::translate("Reagents::CReagentStatusWidget", "Confirmation Message", Q_NULLPTR, -1);
    m_strYes = QApplication::translate("Reagents::CReagentStatusWidget", "Yes", Q_NULLPTR, -1);
    m_strNo = QApplication::translate("Reagents::CReagentStatusWidget", "No", Q_NULLPTR, -1);


    (void) m_ReagentStatusModel.setHeaderData(0, Qt::Horizontal,QApplication::translate("Core::CReagentStatusModel",
                                                                                 "Station", Q_NULLPTR, -1),0);

    (void) m_ReagentStatusModel.setHeaderData(1, Qt::Horizontal,QApplication::translate("Core::CReagentStatusModel",
                                                                                 "Reagent", Q_NULLPTR, -1),0);
    QString SecondColumnName("");
    switch(Core::CReagentStatusModel::RMSPROCESSINGOPTION) {
        default:
            SecondColumnName = "";
            break;
        case Global::RMS_CASSETTES:
            SecondColumnName = QApplication::translate("Core::CReagentStatusModel", "Cassettes\nsince\nchange", Q_NULLPTR, -1);
            break;
        case Global::RMS_CYCLES:
            SecondColumnName = QApplication::translate("Core::CReagentStatusModel", "Cycles\nsince\nchange", Q_NULLPTR, -1);
            break;
        case Global::RMS_DAYS:
            SecondColumnName = QApplication::translate("Core::CReagentStatusModel", "Expiry\nDate", Q_NULLPTR, -1);
            break;
    }

    QString ThirdColumnName("");
    switch (Core::CReagentStatusModel::RMSCLEANINGOPTIONS) {
        default:
            ThirdColumnName = "";
            break;
        case Global::RMS_CYCLES:
             ThirdColumnName = QApplication::translate("Core::CReagentStatusModel", "Cycles\nsince\nchange", Q_NULLPTR, -1);
             break;
        case Global::RMS_DAYS:
             ThirdColumnName = QApplication::translate("Core::CReagentStatusModel", "Expiry\nDate", Q_NULLPTR, -1);
             break;
    }

    (void) m_ReagentStatusModel.setHeaderData(2, Qt::Horizontal, SecondColumnName, 0);
    (void) m_ReagentStatusModel.setHeaderData(3, Qt::Horizontal, ThirdColumnName, 0);
    (void) m_ReagentStatusModel.setHeaderData(4, Qt::Horizontal,QApplication::translate("Core::CReagentStatusModel",
                                                                                 "Exchange\nDate", Q_NULLPTR, -1),0);
    (void) m_ReagentStatusModel.setHeaderData(5, Qt::Horizontal,QApplication::translate("Core::CReagentStatusModel",
                                                                                 "Status", Q_NULLPTR, -1),0);

}

/****************************************************************************/
/*!
 *  \brief This event is called whenever widget is shown
 */
/****************************************************************************/
void CReagentStatusWidget::showEvent(QShowEvent *)
{
    //m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."));
    m_ShowMessageDialog = true ;
    mp_TableWidget->clearSelection();
    mp_Ui->scrollTable->Reset();
    ResetButtons();
}


/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *  \param p_DataConnector = pointer to DataConnector
 *  \param p_ReagentList = pointer to reagent list
 *  \param p_KeyBoard = the keyboard
 */
/****************************************************************************/
void CReagentStatusWidget::SetPtrToMainWindow(Core::CDataConnector *p_DataConnector,
                                         DataManager::CDataReagentList *p_ReagentList,
                                         KeyBoard::CKeyBoard *p_KeyBoard)
{
    Q_UNUSED(p_KeyBoard);
    mp_DataConnector = p_DataConnector;
    CONNECTSIGNALSLOT(mp_DataConnector, WaitRotaryValveHeatingPrompt(),
                      this, DisableBottleCheckFlag());

    CONNECTSIGNALSLOT(mp_DataConnector, EnableBottleCheck(bool),
                              this, EnableBottleCheck(bool));

    mp_ReagentList = p_ReagentList;
    // set the reagent ist to the model
    m_ReagentStatusModel.SetRequiredContainers(mp_ReagentList, mp_DataConnector->ReagentGroupList,
                                               mp_DataConnector->DashboardStationList, 6);
    ResizeHorizontalSection();
}


/****************************************************************************/
/*!
 *  \brief This resets the buttons on the right of the screen
 */
/****************************************************************************/
void CReagentStatusWidget::ResetButtons()
{
    mp_Ui->btnFull->setEnabled(false);
    mp_Ui->btnEmpty->setEnabled(false);
    mp_Ui->btnReset->setEnabled(false);
}

/****************************************************************************/
/*!
 *  \brief This Slot Called when StationList updated
 */
/****************************************************************************/

void CReagentStatusWidget::StationReagentUpdated(const QString& StationId)
{
    Q_UNUSED(StationId)
    m_ReagentStatusModel.UpdateReagentList();
    mp_TableWidget->selectRow(m_CurrentIndex.row());
}

void CReagentStatusWidget::UpdateSelectedStationList(QList<QString>& stationList)
{
    m_StationList = stationList;
}

void CReagentStatusWidget::EnableBottleCheckFlag()
{
    m_EnableBottleCheck = true;
}

void CReagentStatusWidget::DisableBottleCheckFlag()
{
    m_EnableBottleCheck = false;
}

void CReagentStatusWidget::EnableBottleCheck(bool bSet)
{
    m_EnableBottleCheck = bSet;
}

}
