/****************************************************************************/
/*! \file LanguageWidget.cpp
 *
 *  \brief LanguageWidget implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-29
 *   $Author:  $ M.Scherer, C.Adaragunchi
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "Settings/Include/LanguageWidget.h"
#include "ui_LanguageWidget.h"
#include "Core/Include/GlobalHelper.h"


namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CLanguageWidget::CLanguageWidget(QWidget *p_Parent) : MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CLanguageWidget),
    mp_UserSettings(NULL),
    mp_MainWindow(NULL),
    m_ProcessRunning(false),
    m_CurrentUserRole(MainMenu::CMainWindow::Operator),
    mp_DataConnector(NULL)
{
    mp_Ui->setupUi(GetContentFrame());
    mp_TableWidget = new MainMenu::CBaseTable;
    mp_UserSettings = new DataManager::CHimalayaUserSettings();
    m_LanguageModel.SetVisibleRowCount(8);
    QFont Font;
    Font.setPointSize(11);
    Font.setFamily("NanumGothic");
    setFont(Font);
    mp_TableWidget->setModel(&m_LanguageModel);
    mp_Ui->tableWidget->SetContent(mp_TableWidget);
    PopulateLanguageList();
    CONNECTSIGNALSLOT(mp_TableWidget, pressed(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_Ui->btnApply, clicked(), this, OnBtnApplyClicked());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CLanguageWidget::~CLanguageWidget()
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

void CLanguageWidget::changeEvent(QEvent *p_Event)
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
 *  \brief Sets the LangauageList to the Langauge table.
 *  \iparam LanguageList = List of Langauges.
 */
/****************************************************************************/
void CLanguageWidget::SetLanguages(QStringList LanguageList)
{
    m_LanguageNativeNameList.clear();
    m_LanguageList = LanguageList;
    for (int i = 0; i < m_LanguageList.count(); i++) {
        QString LanguageName = m_LanguageList.at(i);

        QLocale Locale(Global::StringToLanguage(LanguageName));
        QString LangaugeNativeName;
        if (LanguageName == "en")
        {
            LangaugeNativeName = "English";
        }
        else
            LangaugeNativeName = Locale.nativeLanguageName();
        if (LangaugeNativeName.isEmpty())
        {
            continue;
        }

        m_LanguageNativeNameList.append("   "+ LangaugeNativeName);
    }
    m_LanguageModel.SetLanguageList(m_LanguageNativeNameList);
    InitDialog();
}

/****************************************************************************/
/*!
 *  \brief Fills the language selection table
 */
/****************************************************************************/
void CLanguageWidget::PopulateLanguageList()
{
    mp_TableWidget->horizontalHeader()->show();
    mp_TableWidget->SetVisibleRows(8);
    m_LanguageModel.SetVisibleRowCount(8);
    mp_TableWidget->horizontalHeader()->resizeSection(0, 300);
}

/****************************************************************************/
/*!
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CLanguageWidget::SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings)
{
    *mp_UserSettings = *p_UserSettings;
}

/****************************************************************************/
/*!
 *  \brief Called when the apply button is clicked
 */
/****************************************************************************/
void CLanguageWidget::OnBtnApplyClicked()
{
    mp_UserSettings->SetLanguage(Global::StringToLanguage(m_SelectedLanguage));
    emit UserSettingsChanged(*mp_UserSettings);
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Currently selected cell
 */
/****************************************************************************/
void CLanguageWidget::SelectionChanged(QModelIndex Index)
{
    int SelectedIndex = Index.row();
    if (SelectedIndex + 1 > m_LanguageList.count())  {
        mp_TableWidget->clearSelection();
        mp_Ui->btnApply->setEnabled(false);
    }
    else {
        m_SelectedLanguage = (m_LanguageList.at(SelectedIndex));
        ResetButtons();
    }
}
/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CLanguageWidget::OnProcessStateChanged()
{
    ResetButtons();
}

void CLanguageWidget::OnUserRoleChanged()
{
    if (MainMenu::CMainWindow::GetCurrentUserRole() == MainMenu::CMainWindow::Operator)
    {
        mp_Ui->btnApply->setEnabled(false);
    }
}
/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CLanguageWidget::ResetButtons()
{
    if (m_LanguageNativeNameList.size()==0) {
        mp_Ui->btnApply->setEnabled(false);
        return ;
    }

    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    bool bCanEdit = Core::CGlobalHelper::CheckIfCanEdit();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) && (!m_ProcessRunning) && bCanEdit) {
            //Edit Mode
            mp_Ui->btnApply->setEnabled(true);
    }
    else {
        mp_Ui->btnApply->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CLanguageWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CLanguageWidget", "Languages", Q_NULLPTR, -1));
    (void) m_LanguageModel.setHeaderData(0,Qt::Horizontal,QApplication::translate("Settings::CLanguageModel", "Language", Q_NULLPTR, -1),0);
}

/****************************************************************************/
/*!
 *  \brief Called when Language widget is show
 */
/****************************************************************************/
void CLanguageWidget::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);    
    InitDialog();
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Initialises the dialog with the language list
 */
/****************************************************************************/
void CLanguageWidget::InitDialog()
{
    QString CurrentLanguage;
    if (mp_UserSettings) {
        CurrentLanguage = QLocale(mp_UserSettings->GetLanguage()).name();
    }
    if (m_LanguageList.count() <= 0) {
        mp_Ui->btnApply->setEnabled(false);
    }
    else {
        mp_Ui->btnApply->setEnabled(true);
    }
    (void) CurrentLanguage.remove(2, 4);
    for (int i = 0; i < m_LanguageList.count();i++) {
        if ( CurrentLanguage == m_LanguageList.at(i)) {
            mp_TableWidget->selectRow(i);
            m_SelectedLanguage = CurrentLanguage;
            break;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *  \iparam p_MainWindow = Reference to Mainwindow
 *  \iparam p_DataConnector = Reference to DataConnector
 */
/****************************************************************************/
void CLanguageWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector)
{
    mp_MainWindow = p_MainWindow;
    mp_DataConnector = p_DataConnector;
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
}

} // end namespace Settings
