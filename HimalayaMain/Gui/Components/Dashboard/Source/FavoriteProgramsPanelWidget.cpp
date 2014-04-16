#include "Dashboard/Include/FavoriteProgramsPanelWidget.h"
#include "ui_FavoriteProgramsPanelWidget.h"
#include "Core/Include/DataConnector.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "Global/Include/Utils.h"
#include "Dashboard/Include/DashboardDateTimeWidget.h"
#include "Core/Include/Startup.h"

#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"


using namespace Dashboard;

QString CFavoriteProgramsPanelWidget::SELECTED_PROGRAM_NAME = tr("");

CFavoriteProgramsPanelWidget::CFavoriteProgramsPanelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFavoriteProgramsPanelWidget),
    mp_DataConnector(NULL),
    m_ProcessRunning(false),
    m_LastSelectedButtonId(-1),
    m_LastCanBeSelectedButtonId(-1)
{
    ui->setupUi(this);
    SetButtonGroup();

    mp_wdgtDateTime = new Dashboard::CDashboardDateTimeWidget();
    mp_wdgtDateTime->setWindowFlags(Qt::CustomizeWindowHint);

    CONNECTSIGNALSLOT(mp_wdgtDateTime, OnSelectDateTime(const QDateTime &), this, OnSelectDateTime(const QDateTime&));
    CONNECTSIGNALSIGNAL(mp_wdgtDateTime, OnSelectDateTime(const QDateTime &), this, OnSelectEndDateTime(const QDateTime &));

    CONNECTSIGNALSLOT(ui->BtnProgram1, clicked(bool), this, OnEndTimeButtonClicked());
    CONNECTSIGNALSLOT(ui->BtnProgram2, clicked(bool), this, OnEndTimeButtonClicked());
    CONNECTSIGNALSLOT(ui->BtnProgram3, clicked(bool), this, OnEndTimeButtonClicked());
    CONNECTSIGNALSLOT(ui->BtnProgram4, clicked(bool), this, OnEndTimeButtonClicked());
    CONNECTSIGNALSLOT(ui->BtnProgram5, clicked(bool), this, OnEndTimeButtonClicked());
    AddItemsToFavoritePanel(false);
}

CFavoriteProgramsPanelWidget::~CFavoriteProgramsPanelWidget()
{
    try {
            delete ui;
            delete mp_wdgtDateTime;
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
    m_mapLabel.insert(0, ui->lblPrg1);
    m_mapLabel.insert(1, ui->lblPrg2);
    m_mapLabel.insert(2, ui->lblPrg3);
    m_mapLabel.insert(3, ui->lblPrg4);
    m_mapLabel.insert(4, ui->lblPrg5);
}

void CFavoriteProgramsPanelWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector)
{
    mp_wdgtDateTime->SetMainWindow(p_MainWindow);
    mp_DataConnector = p_DataConnector;
	mp_wdgtDateTime->SetUserSettings(p_DataConnector->SettingsInterface->GetUserSettings());
}

void CFavoriteProgramsPanelWidget::UpdateProgram(DataManager::CProgram &Program)
{
    for ( int j = 0; j < m_FavProgramIDs.count(); j++)
    {
        QString ProgramId = m_FavProgramIDs.at(j);
        QString ProgramName = mp_ProgramList->GetProgram(ProgramId)->GetName();
        if ( ProgramName == Program.GetName())
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
    }
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

            QLabel* label = m_mapLabel.value(i);
            label->setText("");
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
        qDebug()<<"CFavoriteProgramsPanelWidget::AddItemsToFavoritePanel  ---iconname="<<strIconName;

        //enable this button and use it
        m_ButtonGroup.button(j)->setEnabled(true);
        m_ButtonGroup.button(j)->setIcon(QIcon(strIconName));
        QLabel* label = m_mapLabel.value(j);
        label->setText(ProgramName);
    }
}


void CFavoriteProgramsPanelWidget::OnProcessStateChanged()
{
   m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
}

void CFavoriteProgramsPanelWidget::ProgramSelected(QString& programId, int asapEndTime, bool bProgramStartReady)
{
    Q_UNUSED(programId);
    Q_UNUSED(bProgramStartReady);
    m_ProgramEndDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(asapEndTime);
}

void CFavoriteProgramsPanelWidget::UndoProgramSelection()
{
    m_ButtonGroup.setExclusive(false);
    int checkedID = m_ButtonGroup.checkedId();
    if (checkedID != -1)
    {
        m_ButtonGroup.button(checkedID)->setChecked(false);
    }
    m_ButtonGroup.setExclusive(true);
    m_LastSelectedButtonId = -1;
    m_LastCanBeSelectedButtonId = -1;
}

void CFavoriteProgramsPanelWidget::OnEndTimeButtonClicked()
{
    if ((m_LastSelectedButtonId == m_ButtonGroup.checkedId()) && (m_FavProgramIDs.at(0) != "C01"))
    {   //show Datetime dialog
        mp_wdgtDateTime->UpdateProgramName();
        mp_wdgtDateTime->SetASAPDateTime(m_ProgramEndDateTime);
        mp_wdgtDateTime->show();
    }
    else
    {//on selected a program
        m_LastSelectedButtonId = m_ButtonGroup.checkedId();
        m_NewSelectedProgramId = m_FavProgramIDs.at(m_LastSelectedButtonId);
        SELECTED_PROGRAM_NAME = mp_ProgramList->GetProgram(m_NewSelectedProgramId)->GetName();

        emit PrepareSelectedProgramChecking(m_NewSelectedProgramId);
    }
}

void CFavoriteProgramsPanelWidget::OnSelectDateTime(const QDateTime& dateTime)
{
    m_EndDateTime = dateTime;
}

void CFavoriteProgramsPanelWidget::OnResetFocus(bool reset)
{
    if (reset) {
        m_ButtonGroup.setExclusive(false);
        int checkedID = m_ButtonGroup.checkedId();
        if (checkedID != -1)
        {
            m_ButtonGroup.button(checkedID)->setChecked(false);
        }
        m_ButtonGroup.setExclusive(true);

        if (m_LastCanBeSelectedButtonId != -1) {
            m_ButtonGroup.setExclusive(false);
            m_ButtonGroup.button(m_LastCanBeSelectedButtonId)->setChecked(true);
            m_ButtonGroup.setExclusive(true);
            m_LastSelectedButtonId = m_ButtonGroup.checkedId();
            m_NewSelectedProgramId = m_FavProgramIDs.at(m_LastSelectedButtonId);
            SELECTED_PROGRAM_NAME = mp_ProgramList->GetProgram(m_NewSelectedProgramId)->GetName();
        }
        else {
            m_LastSelectedButtonId = -1;
            m_LastCanBeSelectedButtonId = -1;
        }
    }
    else {
        m_LastCanBeSelectedButtonId = m_ButtonGroup.checkedId();
    }
}

