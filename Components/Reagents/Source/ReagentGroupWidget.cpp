/****************************************************************************/
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/SliderControl.h"
#include "Reagents/Include/ReagentGroupWidget.h"
#include "ui_ReagentGroupWidget.h"
#include "Dashboard/Include/CommonString.h"
#include "Core/Include/GlobalHelper.h"

//lint -e613

namespace Reagents {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \param p_Parent
 *
 *  \return return from CReagentGroupWidget
 */
/****************************************************************************/
CReagentGroupWidget::CReagentGroupWidget(QWidget *p_Parent):
                     MainMenu::CPanelFrame(p_Parent),
                     mp_Ui(new Ui::CReagentGroupWidget),
                     mp_ReagentGroupList(NULL),
                     m_ProcessRunning(false),
                     mp_ReagentGroup(NULL),
                     mp_CReagentGroupColorList(NULL),
                     mp_ModifyReagentColorDlg(NULL),
                     m_strSelectColor(tr("Select Color"))
{
    mp_Ui->setupUi(GetContentFrame());

    m_ShowMessageDialog = false;

    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->setModel(&m_ReagentGroupModel);

    mp_Ui->scrollTable->SetContent(mp_TableWidget);
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();

    mp_TableWidget->SetVisibleRows(8);
    m_ReagentGroupModel.SetVisibleRowCount(8);
    mp_TableWidget->horizontalHeader()->show();
    mp_TableWidget->setFocusPolicy(Qt::ClickFocus);
    ResizeHorizontalSection();

    CONNECTSIGNALSLOT(this, UpdateReagentGroupList(), &m_ReagentGroupModel, UpdateReagentGroupList());
    CONNECTSIGNALSLOT(mp_TableWidget, pressed(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_Ui->btnEdit, clicked(), this, OnEdit());

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CReagentGroupWidget::~CReagentGroupWidget()
{
    try {
        delete mp_TableWidget;
        delete mp_ModifyReagentColorDlg;
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint.
    }
}
/****************************************************************************/
/*!
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CReagentGroupWidget::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{
    Q_UNUSED(p_UserSettings);
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *  \param p_ReagentGroupColorList = the list of reagent group color
 *  \param p_ReagentGroupList = the reagent group list
 *  \param p_MainWindow = pointer to main window
 */
/****************************************************************************/
void CReagentGroupWidget::SetPtrToMainWindow(DataManager::CReagentGroupColorList *p_ReagentGroupColorList,
                                             DataManager::CDataReagentGroupList *p_ReagentGroupList,
                                    MainMenu::CMainWindow *p_MainWindow)
{
    Q_UNUSED(p_MainWindow);
    mp_CReagentGroupColorList = p_ReagentGroupColorList;
    mp_ReagentGroupList = p_ReagentGroupList;
    m_ReagentGroupModel.SetReagentGroupList(mp_ReagentGroupList, 2);
    ResizeHorizontalSection();
    mp_ModifyReagentColorDlg = new CModifyReagentGroupColorDlg(this, p_MainWindow);
    mp_ModifyReagentColorDlg->SetReagentGroupColorListptr(mp_CReagentGroupColorList);
    mp_ModifyReagentColorDlg->setModal(true);
    CONNECTSIGNALSLOT(mp_ModifyReagentColorDlg, UpdateReagentGroup(DataManager::CReagentGroup&),
                        this, OnUpdateReagentGroup(DataManager::CReagentGroup&));
    CONNECTSIGNALSLOT(this,ReagentGroupColorUpdated(),mp_ModifyReagentColorDlg,UpdateReagentGroupColor());

}

/****************************************************************************/
/*!
 *  \brief This event is called whenever widget is shown
 */
/****************************************************************************/
void CReagentGroupWidget::showEvent(QShowEvent *)
{
    //m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."));
    m_ShowMessageDialog = true ;
    mp_TableWidget->clearSelection();
    mp_Ui->scrollTable->Reset();
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CReagentGroupWidget::changeEvent(QEvent *p_Event)
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
void CReagentGroupWidget::ResizeHorizontalSection()
{
    if (mp_TableWidget == NULL)
    {
        return;
    }
    mp_TableWidget->horizontalHeader()->resizeSection(0, 159);
    mp_TableWidget->horizontalHeader()->resizeSection(1, 170);
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Currently selected cell
 */
/****************************************************************************/
void CReagentGroupWidget::SelectionChanged(QModelIndex Index)
{
    QString Id = m_ReagentGroupModel.data(Index, (int)Qt::UserRole).toString();
    mp_ReagentGroup = mp_ReagentGroupList->GetReagentGroup(Id);
    if (mp_ReagentGroup) {
        mp_TableWidget->selectRow(Index.row());
        if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
             m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
                (!m_ProcessRunning)) {
            if (Core::CGlobalHelper::CheckIfCanEdit(Id, 3) == false) {
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

/*****************************************************************************/
 /*!
  *  \brief Translates the strings in UI to the selected language
  */
/****************************************************************************/
void CReagentGroupWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Reagents::CReagentRMSWidget",
                                                        "Groups", Q_NULLPTR, -1));
   (void) m_ReagentGroupModel.setHeaderData(0,Qt::Horizontal,QApplication::translate("Core::CReagentGroupModel",
                                                        "Reagent Group", Q_NULLPTR, -1),0);
   (void) m_ReagentGroupModel.setHeaderData(1,Qt::Horizontal,QApplication::translate("Core::CReagentGroupModel",
                                                          "Color", Q_NULLPTR, -1),0);
   m_strSelectColor = QApplication::translate("Reagents::CReagentGroupWidget", "Select Color", Q_NULLPTR, -1);
}

/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of a reagent
 */
/****************************************************************************/
void CReagentGroupWidget::OnEdit()
{
    if (!mp_ModifyReagentColorDlg)
        return;
    /*m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."
                            "\nPlease close the dialog with \"Close\""));*/
    mp_ModifyReagentColorDlg->SetDialogTitle(m_strSelectColor);
    mp_ModifyReagentColorDlg->SetReagentGroupList(*mp_ReagentGroupList, *mp_ReagentGroup);
    mp_ModifyReagentColorDlg->show();

}

/****************************************************************************/
/*!
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CReagentGroupWidget::OnUserRoleChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {
        //Edit Mode
        mp_Ui->btnEdit->setEnabled(true);
    }
    else {
        mp_Ui->btnEdit->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CReagentGroupWidget::OnProcessStateChanged()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {
        //Edit Mode
        mp_Ui->btnEdit->setEnabled(true);
    }
    else {
        mp_Ui->btnEdit->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief This resets the buttons on the right of the screen
 */
/****************************************************************************/
void CReagentGroupWidget::ResetButtons()
{
    mp_Ui->btnEdit->setEnabled(false);
}

/**************************************************************************/
/*!
 * \brief Disable Edit button and Emit UpdateReagentGroup signal
 */
/**************************************************************************/
    void CReagentGroupWidget :: OnUpdateReagentGroup(DataManager::CReagentGroup& ObjCReagentGroup)
    {
        mp_Ui->btnEdit->setEnabled(false);
        emit UpdateReagentGroup(ObjCReagentGroup);
    }

} // end namespace Reagents



