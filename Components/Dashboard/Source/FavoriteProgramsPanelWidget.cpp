#include "Dashboard/Include/FavoriteProgramsPanelWidget.h"
#include "ui_FavoriteProgramsPanelWidget.h"
#include "Core/Include/DataConnector.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "Global/Include/Utils.h"
#include "Dashboard/Include/DashboardDateTimeWidget.h"
#include "Core/Include/Startup.h"
#include "Core/Include/GlobalHelper.h"

#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"


using namespace Dashboard;

QString CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME = tr("");

CFavoriteProgramsPanelWidget::CFavoriteProgramsPanelWidget(QWidget *p) :
    QWidget(p),
    ui(new Ui::CFavoriteProgramsPanelWidget),
    mp_DataConnector(NULL),
    m_LastCanBeSelectedButtonId(-1),
    m_ProcessRunning(false),
    m_LastSelectedButtonId(-1),
    m_OnlyAddCleaningProgram(false),
    m_IsInFavProgramButtonClicked(false)
{
    ui->setupUi(this);
    SetButtonGroup();
   
    AddItemsToFavoritePanel(false);
}

CFavoriteProgramsPanelWidget::~CFavoriteProgramsPanelWidget()
{
    try {
            delete ui;
        }
        catch (...) {
            // to please Lint.
        }
}

void CFavoriteProgramsPanelWidget ::SetButtonGroup()
{
    m_ButtonGroup.addButton(ui->BtnProgram1, 0);
    m_ButtonGroup.addButton(ui->BtnProgram2, 1);
    m_ButtonGroup.addButton(ui->BtnProgram3, 2);
    m_ButtonGroup.addButton(ui->BtnProgram4, 3);
    m_ButtonGroup.addButton(ui->BtnProgram5, 4);

    m_mapLabel.clear();
    (void)m_mapLabel.insert(0, ui->stackedWidget1);
    (void)m_mapLabel.insert(1, ui->stackedWidget2);
    (void)m_mapLabel.insert(2, ui->stackedWidget3);
    (void)m_mapLabel.insert(3, ui->stackedWidget4);
    (void)m_mapLabel.insert(4, ui->stackedWidget5);
    CONNECTSIGNALSLOT(&m_ButtonGroup, buttonClicked(int), this, OnFavProgramButtonClicked(int));
    CONNECTSIGNALSLOT(ui->BtnProgram1, toggled(bool), ui->stackedWidget1, setHighlight(bool));
    CONNECTSIGNALSLOT(ui->BtnProgram2, toggled(bool), ui->stackedWidget2, setHighlight(bool));
    CONNECTSIGNALSLOT(ui->BtnProgram3, toggled(bool), ui->stackedWidget3, setHighlight(bool));
    CONNECTSIGNALSLOT(ui->BtnProgram4, toggled(bool), ui->stackedWidget4, setHighlight(bool));
    CONNECTSIGNALSLOT(ui->BtnProgram5, toggled(bool), ui->stackedWidget5, setHighlight(bool));
}


void CFavoriteProgramsPanelWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector)
{
    Q_UNUSED(p_MainWindow);
    mp_DataConnector = p_DataConnector;
}

void CFavoriteProgramsPanelWidget::UpdateProgram(DataManager::CProgram &Program)
{
    qDebug() << "CFavoriteProgramsPanelWidget::UpdateProgram";

    for ( int j = 0; j < m_FavProgramIDs.count(); j++)
    {
        QString ProgramId = m_FavProgramIDs.at(j);

        if (ProgramId == Program.GetID()) {
            const QString name = Program.GetName();
            DataManager::CProgram *prog = const_cast<DataManager::CProgram *>(mp_ProgramList->GetProgram(ProgramId));
            prog->SetName(name);
            CStackedLabel* label = m_mapLabel.value(j);
            label->setText(name);
            label->setHighlight(false);
            label->setVisible(true);
        }

        QString ProgramName = mp_ProgramList->GetProgram(ProgramId)->GetName();
        if (ProgramName == Program.GetName())
        {
            QString strIconName;
            if (Program.GetIcon().isEmpty())
            {
                strIconName = "";
            }
            else
            {
                strIconName = ":/HimalayaImages/Icons/Program/"+ Program.GetIcon() + ".png";
            }

            m_ButtonGroup.button(j)->setIcon(QIcon(strIconName));
            break;
        }
        else {
            Core::CGlobalHelper::UnselectProgram();
            emit UpdateFavProgram();
        }
    }
}

void CFavoriteProgramsPanelWidget::AddItemsToFavoritePanel()
{
    AddItemsToFavoritePanel(m_OnlyAddCleaningProgram);
}

void CFavoriteProgramsPanelWidget::AddItemsToFavoritePanel(bool bOnlyAddCleaningProgram)
{
    UndoProgramSelection();
    //loop all program buttons
    for(int i = 0; i < 5; i++)
    {
        QAbstractButton* btn = m_ButtonGroup.button(i);
        if (btn->isEnabled())
        {
            btn->setEnabled(false);
            btn->setIcon(QIcon(""));

            CStackedLabel* label = m_mapLabel.value(i);
            label->setVisible(false);
        }
    }

    if (!mp_DataConnector)
    return;

    m_FavProgramIDs.clear();
    mp_ProgramList = mp_DataConnector->ProgramList;
    if (bOnlyAddCleaningProgram)
    {
        m_FavProgramIDs.append("C01");
    }
    else
    {
        m_FavProgramIDs = mp_ProgramList->GetFavoriteProgramIDs(); // get five favorite Programs' ID
    }



    for ( int j = 0; j < m_FavProgramIDs.count(); j++)
    {
        QString ProgramId = m_FavProgramIDs.at(j);
        QString ProgramName = mp_ProgramList->GetProgram(ProgramId)->GetName();
        QString strIconName;
        if (mp_ProgramList->GetProgram(ProgramId)->GetIcon().isEmpty())
        {
            strIconName = "";
        }
        else
            strIconName = ":/HimalayaImages/Icons/Program/"+ mp_ProgramList->GetProgram(ProgramId)->GetIcon() + ".png";

        //enable this button and use it
        m_ButtonGroup.button(j)->setEnabled(true);
        m_ButtonGroup.button(j)->setIcon(QIcon(strIconName));
        CStackedLabel* label = m_mapLabel.value(j);
        label->setText(ProgramName);
        label->setHighlight(false);
        label->setVisible(true);
    }
  
    m_OnlyAddCleaningProgram = bOnlyAddCleaningProgram;
}


void CFavoriteProgramsPanelWidget::OnProcessStateChanged()
{
   m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
}

void CFavoriteProgramsPanelWidget::UndoProgramSelection()
{
    int checkedID = m_ButtonGroup.checkedId();
    if (-1 == checkedID)
        return;

    m_ButtonGroup.setExclusive(false);
    if (checkedID != -1)
    {
        m_ButtonGroup.button(checkedID)->setChecked(false);
    }
    m_ButtonGroup.setExclusive(true);
    m_LastSelectedButtonId = -1;
    m_LastCanBeSelectedButtonId = -1;
    m_NewSelectedProgramId = "";
    SELECTED_PROGRAM_NAME = "";
    Core::CGlobalHelper::UnselectProgram();
    emit UpdateFavProgram();
}

void CFavoriteProgramsPanelWidget::OnFavProgramButtonClicked(int btnIndex)
{
    Q_UNUSED(btnIndex);
    if (m_IsInFavProgramButtonClicked)
        return;
    m_IsInFavProgramButtonClicked = true;

	m_LastSelectedButtonId = m_ButtonGroup.checkedId();
	m_NewSelectedProgramId = m_FavProgramIDs.at(m_LastSelectedButtonId);
	SELECTED_PROGRAM_NAME = mp_ProgramList->GetProgram(m_NewSelectedProgramId)->GetName();

	emit PrepareSelectedProgramChecking(m_NewSelectedProgramId);
}

void CFavoriteProgramsPanelWidget::OnResetFocus(bool rst)
{
    if (rst) {
        int checkedID = m_ButtonGroup.checkedId();
        if (checkedID == m_LastCanBeSelectedButtonId)
        return;
        m_ButtonGroup.setExclusive(false);
        if (checkedID != -1)
        {
            m_ButtonGroup.button(checkedID)->setChecked(false);
        }
        m_ButtonGroup.setExclusive(true);

        if (m_LastCanBeSelectedButtonId != -1) {
            m_ButtonGroup.setExclusive(false);
            m_ButtonGroup.button(m_LastCanBeSelectedButtonId)->setChecked(true);
            m_ButtonGroup.setExclusive(true);
            OnFavProgramButtonClicked(0);//select last one again
        }
        else {
            m_LastSelectedButtonId = -1;
            m_LastCanBeSelectedButtonId = -1;
            m_NewSelectedProgramId = "";
            SELECTED_PROGRAM_NAME = "";
        }
    }
    else {
        m_LastCanBeSelectedButtonId = m_ButtonGroup.checkedId();
    }
}


void CFavoriteProgramsPanelWidget::ResetInFavProgramButtonClicked()
{
    m_IsInFavProgramButtonClicked = false;
}

void CFavoriteProgramsPanelWidget::SetInFavProgramButtonClicked()
{
    m_IsInFavProgramButtonClicked = true;
}

void CFavoriteProgramsPanelWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

void CFavoriteProgramsPanelWidget::RetranslateUI()
{
    for ( int j = 0; j < m_FavProgramIDs.count(); j++)
    {
        QString ProgramId = m_FavProgramIDs.at(j);
        QString ProgramName = mp_ProgramList->GetProgram(ProgramId)->GetName();

        CStackedLabel* label = m_mapLabel.value(j);
        label->setText(ProgramName, label->IsTextHighlight());
        label->setHighlight(label->IsTextHighlight());
    }

    if (!m_NewSelectedProgramId.isEmpty())
    {
        const DataManager::CProgram* pProgram = mp_ProgramList->GetProgram(m_NewSelectedProgramId);
        if (NULL != pProgram){
            SELECTED_PROGRAM_NAME = pProgram->GetName();
        }
    }
}
