/****************************************************************************/
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/SliderControl.h"
#include "Reagents/Include/ReagentStationWidget.h"
#include "Reagents/Include/ModifyReagentStationDlg.h"
#include "Reagents/Build/ui_ReagentStationWidget.h"
#include "Core/Include/GlobalHelper.h"


namespace Reagents {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Reference to Parent object
 *
 *  \return return from CReagentStationWidget
 */
/****************************************************************************/
CReagentStationWidget::CReagentStationWidget(QWidget *p_Parent):
                                           MainMenu::CPanelFrame(p_Parent),
                                           mp_Ui(new Ui::CReagentStationWidget),
                                           mp_DataConnector(NULL),
                                           mp_ReagentList(NULL),
                                           m_CurrentUserRole(MainMenu::CMainWindow::GetCurrentUserRole()),
                                           mp_Reagent(NULL),
                                           mp_DashStation(NULL),
                                           mp_ModifiyReagentStationDlg(NULL),
                                            m_strSelectReagent(tr("Select Reagent"))
{
    mp_Ui->setupUi(GetContentFrame());

    m_ShowMessageDialog = false;
    m_ProcessRunning = false;

    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->setModel(&m_ReagentStationModel);


    mp_Ui->scrollTable->SetContent(mp_TableWidget);

    PopulateReagentList();
    CONNECTSIGNALSLOT(mp_TableWidget, pressed(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_Ui->btnEdit, clicked(), this, OnEdit());
    CONNECTSIGNALSLOT(this, UpdateReagentList(), &m_ReagentStationModel, UpdateReagentList());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CReagentStationWidget::~CReagentStationWidget()
{
    try {
        delete mp_TableWidget;
        delete mp_Ui;
        delete mp_ModifiyReagentStationDlg;
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
void CReagentStationWidget::changeEvent(QEvent *p_Event)
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
void CReagentStationWidget::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{

    Q_UNUSED(p_UserSettings);
}


/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of a reagent
 */
/****************************************************************************/
void CReagentStationWidget::OnEdit()
{
    m_MessageDlg.SetText(m_strSelectReagent);
    mp_ModifiyReagentStationDlg->SetDialogTitle(m_strSelectReagent);
    mp_ModifiyReagentStationDlg->SetEditedDashboardStation(mp_DashStation);
    mp_ModifiyReagentStationDlg->show();
}

/****************************************************************************/
/*!
 *  \brief Fills the reagents list with content
 */
/****************************************************************************/
void CReagentStationWidget::PopulateReagentList()
{
    mp_TableWidget->SetVisibleRows(8);
    m_ReagentStationModel.SetVisibleRowCount(8);
    mp_TableWidget->horizontalHeader()->show();
    ResizeHorizontalSection();
}

/****************************************************************************/
/*!
 *  \brief Resizes the columns in the reagents table
 */
/****************************************************************************/
void CReagentStationWidget::ResizeHorizontalSection()
{
    if (mp_TableWidget == NULL)
    {
        return;
    }
    mp_TableWidget->horizontalHeader()->resizeSection(0, 150);
    mp_TableWidget->horizontalHeader()->resizeSection(1, 150);
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Currently selected cell
 */
/****************************************************************************/
void CReagentStationWidget::SelectionChanged(QModelIndex Index)
{
    QString Id = m_ReagentStationModel.data(Index, (int)Qt::UserRole).toString();
    mp_DashStation = const_cast<DataManager::CDashboardStation*>(mp_DataConnector->DashboardStationList->GetDashboardStation(Id));

    if (mp_DashStation) {
        if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
             m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
                (!m_ProcessRunning)) {
            if (Core::CGlobalHelper::CheckIfCanEdit(Id, 4) == false) {
                mp_Ui->btnEdit->setEnabled(false);
            }
            else {
                //Edit Mode
                mp_Ui->btnEdit->setEnabled(true);
            }
        }
        else {
            mp_Ui->btnEdit->setEnabled(false);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CReagentStationWidget::OnUserRoleChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_UserRoleChanged = true;
    if (m_CurrentUserRole == MainMenu::CMainWindow::Operator)
    {
        mp_Ui->btnEdit->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CReagentStationWidget::OnProcessStateChanged()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {
        //Edit Mode
        bool m_IsLeicaReagent = m_SelectedReagent.IsLeicaReagent();
        if(m_IsLeicaReagent) {
            mp_Ui->btnEdit->setEnabled(false);
        }
        else {
            mp_Ui->btnEdit->setEnabled(true);
        }
    }
    else {
        mp_Ui->btnEdit->setEnabled(false);

    }
}

/*****************************************************************************/
/*!
  *  \brief Translates the strings in UI to the selected language
  */
/****************************************************************************/
void CReagentStationWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Reagent Station::CReagentStatusWidget",
                                                                 "Stations", Q_NULLPTR, -1));
    (void) m_ReagentStationModel.setHeaderData(0, Qt::Horizontal,QApplication::translate("Core::CReagentStationModel",
                                                                                 "Station", Q_NULLPTR, -1),0);

    (void) m_ReagentStationModel.setHeaderData(1, Qt::Horizontal,QApplication::translate("Core::CReagentStationModel",
                                                                                 "Reagent", Q_NULLPTR, -1),0);

    m_strSelectReagent = QApplication::translate("Reagents::CReagentStationWidget", "Select Reagent", Q_NULLPTR, -1);

}

/****************************************************************************/
/*!
 *  \brief This event is called whenever widget is shown
 */
/****************************************************************************/
void CReagentStationWidget::showEvent(QShowEvent *)
{
    //m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."));
    m_ShowMessageDialog = true ;
    mp_TableWidget->clearSelection();
    mp_Ui->scrollTable->Reset();
    ResetButtons();
}

void CReagentStationWidget::SetPtrToMainWindow(Core::CDataConnector *p_DataConnector,
                                         DataManager::CDataReagentList *p_ReagentList,
                                         MainMenu::CMainWindow *p_MainWindow,
                                         KeyBoard::CKeyBoard *p_KeyBoard)
{
    CONNECTSIGNALSLOT(p_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
    Q_UNUSED(p_KeyBoard);
    mp_DataConnector = p_DataConnector;
    mp_ReagentList = p_ReagentList;
    // set the reagent ist to the model
    m_ReagentStationModel.SetRequiredContainers(mp_ReagentList,
                                               mp_DataConnector->DashboardStationList,
                                                2);
    ResizeHorizontalSection();
    mp_ModifiyReagentStationDlg = new CModifyReagentStationDlg(this, p_MainWindow, p_DataConnector);
    mp_ModifiyReagentStationDlg->setModal(true);
    CONNECTSIGNALSIGNAL(mp_ModifiyReagentStationDlg, UpdateStationChangeReagent(QString,QString),
                        this, UpdateStationChangeReagent(QString,QString));

    CONNECTSIGNALSLOT(mp_ModifiyReagentStationDlg, UpdateStationChangeReagent(QString,QString),
                        this, HandleStationChangeReagent(QString,QString));

    CONNECTSIGNALSIGNAL(mp_ModifiyReagentStationDlg, UpdateStationSetAsEmpty(QString),
                        this, UpdateStationSetAsEmpty(QString));

    CONNECTSIGNALSIGNAL(mp_ModifiyReagentStationDlg, UnselectProgram(),
                                this, UnselectProgram());


}


/****************************************************************************/
/*!
 *  \brief This resets the buttons on the right of the screen
 */
/****************************************************************************/
void CReagentStationWidget::ResetButtons()
{
    mp_Ui->btnEdit->setEnabled(false);
}

/****************************************************************************/
/*!
 *  \brief This Slot Called when Reagent is updated in station
 */
/****************************************************************************/
void CReagentStationWidget:: StationReagentUpdated(const QString& StationId)
{
    m_ReagentStationModel.UpdateReagent(StationId);
}

void CReagentStationWidget::UpdateSelectedStationList(QList<QString>& stationList)
{
    m_StationList = stationList;
}

void CReagentStationWidget::HandleStationChangeReagent(const QString& stationid, const QString& reagentid)
{
    Q_UNUSED(stationid);
    Q_UNUSED(reagentid);
    this->ResetButtons();
}

}
