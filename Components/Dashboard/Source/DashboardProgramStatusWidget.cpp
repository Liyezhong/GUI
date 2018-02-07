#include "Dashboard/Include/DashboardProgramStatusWidget.h"
#include "ui_DashboardProgramStatusWidget.h"
#include "Dashboard/Include/FavoriteProgramsPanelWidget.h"
#include "MainMenu/Include/BaseTable.h"
#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"
#include "Core/Include/DataConnector.h"
#include "Core/Include/GlobalHelper.h"


using namespace Dashboard;

CDashboardProgramStatusWidget::CDashboardProgramStatusWidget(QWidget *p) :
    MainMenu::CDialogFrame(p),
    ui(new Ui::CDashboardProgramStatusWidget)
{
    ui->setupUi(GetContentFrame());

    CONNECTSIGNALSLOT(ui->btnClose, clicked(), this, OnClose());
    CONNECTSIGNALSLOT(ui->btnAbort, clicked(), this, OnAbort());


    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->horizontalHeader()->show();
    ui->scrollTable_ProgramSteps->SetContent(mp_TableWidget);
    mp_TableWidget->SetVisibleRows(6);
}

CDashboardProgramStatusWidget::~CDashboardProgramStatusWidget()
{
    try
    {
        delete ui;
        delete mp_TableWidget;
    }catch (...)
    {}
}


void CDashboardProgramStatusWidget::InitDialog(DataManager::CProgram *p_Program,
                                               const Core::CDataConnector* pDataConnector,
                                               QList<QString>& StationNameList, int CurProgramStepIndex,
                                               int StepRemainingTime, int ProgramRemainingTime,
                                               const QString& endDateTime,
                                               bool bAbortButtonEnabled,
                                               const QString& strStation,
                                               const QString& strTemprature)
{
    if (!p_Program || !pDataConnector)
        return;

    SetDialogTitle(QString("%1").arg(CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME));

    m_StepModel.SetVisibleRowCount(6);
    m_StepModel.ShowStation(true, strStation, strTemprature);
    m_StepModel.SetCurSelectRowIndex(CurProgramStepIndex);
    m_StepModel.SetStationNameList(StationNameList);
    m_StepModel.SetProgram(p_Program, pDataConnector->SettingsInterface->GetUserSettings(), pDataConnector->ReagentGroupList,
                           pDataConnector->ReagentList, 6);
    mp_TableWidget->setModel(&m_StepModel);
    ResizeHorizontalSection();
    mp_TableWidget->selectRow(CurProgramStepIndex);
    ui->labelStepRemainingTime->setText(Core::CGlobalHelper::TimeToString(StepRemainingTime, false));
    ui->labelPrgmRemainingTime->setText(Core::CGlobalHelper::TimeToString(ProgramRemainingTime, false));
    ui->LabelPrgmEndTime->setText(endDateTime);
    ui->btnAbort->setEnabled(bAbortButtonEnabled);
}

/****************************************************************************/
/*!
 *  \brief Resizes the columns in the program step table
 */
/****************************************************************************/
void CDashboardProgramStatusWidget::ResizeHorizontalSection()
{
    mp_TableWidget->horizontalHeader()->resizeSection(0, 70);
    mp_TableWidget->horizontalHeader()->resizeSection(1, 180);
    mp_TableWidget->horizontalHeader()->resizeSection(2, 100);
    mp_TableWidget->horizontalHeader()->resizeSection(3, 75);
    mp_TableWidget->horizontalHeader()->resizeSection(4, 70);
    mp_TableWidget->horizontalHeader()->resizeSection(5, 45);
}

void CDashboardProgramStatusWidget::OnClose()
{
    (void)this->close();
}

void CDashboardProgramStatusWidget::OnAbort()
{
    (void)this->accept();
    emit AbortClicked(1);
}

void CDashboardProgramStatusWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void CDashboardProgramStatusWidget::DisableAbortButton()
{
    ui->btnAbort->setEnabled(false);
}


