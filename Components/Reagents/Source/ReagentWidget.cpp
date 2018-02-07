
/****************************************************************************/
/*! \file ReagentWidget.cpp
 *
 *  \brief ReagentWidget Implementation.
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
#include "Reagents/Include/ReagentWidget.h"
#include "Reagents/Include/ModifyReagentRMSDlg.h"
#include "ui_ReagentWidget.h"

namespace Reagents {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p = pointer to the main window
 *  \iparam p_KeyBoard = Reference to Keyboard widget
 *  \iparam p_DataConnector = Reference to DataConnector
 *
 *  \return return from CReagentWidget
 */
/****************************************************************************/
CReagentWidget::CReagentWidget(Core::CDataConnector *p_DataConnector,
                                 MainMenu::CMainWindow *p,
                                 KeyBoard::CKeyBoard *p_KeyBoard):
                                 mp_Ui(new Ui::CReagentWidget),
                                 mp_DataConnector(p_DataConnector),                                 
                                 mp_MainWindow(p),
                                 mp_KeyBoardWidget(p_KeyBoard)



{
    mp_Ui->setupUi(this);
    m_ProcessRunning = false;
    m_ShowMessageDialog = false;

    DataManager::CUserSettings *p_Settings = mp_DataConnector->SettingsInterface->GetUserSettings();
    if (p_Settings) {
        mp_Ui->pageReagentStatus->SetUserSettings(p_Settings);
        mp_Ui->pageReagentGroup->SetUserSettings(p_Settings);
        mp_Ui->pageReagentStation->SetUserSettings(p_Settings);
        //mp_Ui->pageReagents->SetUserSettings(p_Settings);
        mp_Ui->pageReagentStatus->SetPtrToMainWindow(mp_DataConnector, mp_DataConnector->ReagentList, mp_KeyBoardWidget);
        mp_Ui->pageReagentStation->SetPtrToMainWindow(mp_DataConnector, mp_DataConnector->ReagentList, mp_MainWindow, mp_KeyBoardWidget);
        mp_Ui->pageReagentGroup->SetPtrToMainWindow(mp_DataConnector->ReagentGroupColorList,mp_DataConnector->ReagentGroupList, mp_MainWindow);
        mp_Ui->pageReagents->SetPtrToMainWindow(mp_DataConnector, mp_DataConnector->ReagentList, mp_MainWindow, mp_KeyBoardWidget);
    }
    // call when reagents are updated
    CONNECTSIGNALSIGNAL(mp_DataConnector, ReagentUpdated(), mp_Ui->pageReagents, UpdateReagentList());
    CONNECTSIGNALSIGNAL(mp_DataConnector, ReagentUpdated(), mp_Ui->pageReagentStatus, UpdateReagentList());
    CONNECTSIGNALSIGNAL(mp_DataConnector, ReagentUpdated(), mp_Ui->pageReagentStation, UpdateReagentList());

    // call when Reagent group is updated, since the color is changed
    CONNECTSIGNALSIGNAL(mp_DataConnector, ReagentGroupUpdated(), mp_Ui->pageReagents, UpdateReagentList());
    // for the reagent group
    CONNECTSIGNALSIGNAL(mp_DataConnector, ReagentGroupUpdated(), mp_Ui->pageReagentGroup, UpdateReagentGroupList());
    // by calling reagent list it updates all the stations

    CONNECTSIGNALSIGNAL(mp_DataConnector, ReagentGroupUpdated(), mp_Ui->pageReagentStatus, UpdateReagentList());
    CONNECTSIGNALSIGNAL(mp_DataConnector, DashboardStationsUpdated(), mp_Ui->pageReagentStatus, UpdateReagentList());


    CONNECTSIGNALSIGNAL(mp_DataConnector, ReagentGroupUpdated(), mp_Ui->pageReagentStation, UpdateReagentList());
    CONNECTSIGNALSIGNAL(mp_DataConnector, DashboardStationsUpdated(), mp_Ui->pageReagentStation, UpdateReagentList());


    CONNECTSIGNALSLOT(mp_Ui->pageReagentGroup, UpdateReagentGroup(DataManager::CReagentGroup &), mp_DataConnector,
                        SendReagentGroupUpdate(DataManager::CReagentGroup &));
    CONNECTSIGNALSIGNAL(mp_DataConnector,ReagentGroupColorUpdated(),mp_Ui->pageReagentGroup,ReagentGroupColorUpdated());

    CONNECTSIGNALSLOT(mp_Ui->pageReagentStation, UpdateStationChangeReagent(QString,QString),
                      mp_DataConnector, SendStationChangeReagent(QString,QString));
    CONNECTSIGNALSLOT(mp_Ui->pageReagentStation, UpdateStationSetAsEmpty(QString),
                      mp_DataConnector, SendStationSetAsEmpty(QString));

    CONNECTSIGNALSLOT(mp_Ui->pageReagentStatus, UpdateStationResetData(QString),
                      mp_DataConnector, SendStationResetData(QString));
    CONNECTSIGNALSLOT(mp_Ui->pageReagentStatus, UpdateStationSetAsEmpty(QString),
                      mp_DataConnector, SendStationSetAsEmpty(QString));

    CONNECTSIGNALSIGNAL(mp_Ui->pageReagentStation, UnselectProgram(),
                        this, UnselectProgram());

    CONNECTSIGNALSIGNAL(mp_Ui->pageReagentStatus, UnselectProgram(),
                      this, UnselectProgram());

    CONNECTSIGNALSIGNAL(mp_Ui->pageReagentStatus, BottleCheck(),
                      this, BottleCheck());

    CONNECTSIGNALSLOT(this, UpdateSelectedStationList(QList<QString>&),
                      mp_Ui->pageReagentStatus, UpdateSelectedStationList(QList<QString>&));

    CONNECTSIGNALSLOT(this, UpdateSelectedStationList(QList<QString>&),
                      mp_Ui->pageReagentStation, UpdateSelectedStationList(QList<QString>&));


    CONNECTSIGNALSLOT(mp_Ui->pageReagentStatus, UpdateStationSetAsFull(QString),
                      mp_DataConnector, SendStationSetAsFull(QString));

    CONNECTSIGNALSLOT(mp_Ui->reagentsubmenuPanel, CurrentRowChanged(int), mp_Ui->reagentsStack, setCurrentIndex(int));
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), mp_Ui->pageReagents, OnUserRoleChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), mp_Ui->pageReagents, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), mp_Ui->pageReagentGroup, OnUserRoleChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), mp_Ui->pageReagentGroup, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), mp_Ui->pageReagentStatus, OnUserRoleChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), mp_Ui->pageReagentStatus, OnProcessStateChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), mp_Ui->pageReagentStation, OnUserRoleChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), mp_Ui->pageReagentStation, OnProcessStateChanged());

    CONNECTSIGNALSLOT(mp_Ui->pageReagents, RMSChanged(const Global::RMSOptions_t), mp_Ui->pageReagentStatus, RMSChanged(const Global::RMSOptions_t));
    CONNECTSIGNALSLOT(mp_Ui->pageReagents, RMSCleaningChanged(const Global::RMSOptions_t), mp_Ui->pageReagentStatus, RMSCleaningChanged(const Global::RMSOptions_t));
    CONNECTSIGNALSLOT(mp_DataConnector, DashboardStationChangeReagent(const QString&), mp_Ui->pageReagentStatus, StationReagentUpdated(const QString&));
    CONNECTSIGNALSLOT(mp_DataConnector, DashboardStationChangeReagent(const QString&), mp_Ui->pageReagentStation, StationReagentUpdated(const QString&));

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CReagentWidget::~CReagentWidget()
{
    try {
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

void CReagentWidget::changeEvent(QEvent *p_Event)
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
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CReagentWidget::OnUserRoleChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service)) //&&

    m_UserRoleChanged = true;
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CReagentWidget::RetranslateUI()
{
    QWidget::setWindowTitle(QApplication::translate("Reagents::CReagentWidget", "", Q_NULLPTR, -1));
}

/****************************************************************************/
/*!
 *  \brief Slot for the selected panel
 *  \param[in] Index = Index of the panel
 */
/****************************************************************************/
void CReagentWidget::PanelSelected(int Index)
{
    // check for the whether panel is Reagents Widget or not
    int PanelIndex = mp_Ui->reagentsStack->indexOf(mp_Ui->pageReagents);
    // if the panel index matches then update it
    if (PanelIndex == Index) {
        mp_DataConnector->RequestDayRunLogFileNames();
    }
}

} // end namespace Reagents
