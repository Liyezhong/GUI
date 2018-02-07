/****************************************************************************/
/*! \file ReagentWidget.h
 *
 *  \brief ReagentWidget definition.
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

#ifndef REAGENTS_REAGENTWIDGET_H
#define REAGENTS_REAGENTWIDGET_H

#include "Core/Include/ReagentRMSModel.h"
#include "Core/Include/DataConnector.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/SliderControl.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"

#include "KeyBoard/Include/KeyBoard.h"

namespace Reagents {

namespace Ui {
    class CReagentWidget;
}

/****************************************************************************/
/**
 * \brief Main  reagents widget listing all reagents available
 */
/****************************************************************************/
class CReagentWidget : public QWidget
{
    Q_OBJECT

    friend class CTestReagents;

private:
    Ui::CReagentWidget *mp_Ui;                     //!< User interface
    Core::CDataConnector *mp_DataConnector;          //!< Data object
    DataManager::CDataReagentList *mp_ReagentList;  //!< Reagent list
    DataManager::CReagent m_SelectedReagent;        //!< Currently selected reagent
    MainMenu::CMainWindow *mp_MainWindow;           //!< Reference to main window.
    MainMenu::CMessageDlg m_MessageDlg;             //!< Information Message Dialog
    bool m_UserRoleChanged;                         //!< True if user Role changed else false
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole; //!< Current user role
    bool m_ProcessRunning;                          //!< Process running state
    bool m_ShowMessageDialog;                       //!< To show Information Message Dialog
    DataManager::CReagent const *mp_Reagent;        //!< Reagent object
    DataManager::CUserSettings *mp_UserSettings;    //!< Data object
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;         //!< Keyboard object
    MainMenu::CMainWindow *p_Parent;       ///<  Definition/Declaration of variable p_Parent

protected:
      void changeEvent(QEvent *p_Event);

public:
      explicit CReagentWidget(Core::CDataConnector *p_DataConnector,
                               MainMenu::CMainWindow *p_Parent = NULL,
                               KeyBoard::CKeyBoard *p_KeyBoard = NULL);
    ~CReagentWidget();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetUserSettings
     *
     *  \param p_UserSettings = DataManager::CUserSettings type parameter
     *
     *  \return from SetUserSettings
     */
    /****************************************************************************/
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);


private:
    void RetranslateUI();

signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when UserSettings are changed.
     *  \param Settings = the changed user setting
     *
     */
    /****************************************************************************/
    void SettingsChanged(DataManager::CUserSettings &Settings);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when RMS Mode is Changed.
     *
     */
    /****************************************************************************/
    void RMSValueChanged(Global::RMSOptions_t);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UnselectProgram
     */
    /****************************************************************************/
    void UnselectProgram();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal BottleCheck
     */
    /****************************************************************************/
    void BottleCheck();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateSelectedStationList
     */
    /****************************************************************************/
    void UpdateSelectedStationList(QList<QString>&);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnUserRoleChanged
     */
    /****************************************************************************/
    void OnUserRoleChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of PanelSelected
     */
    /****************************************************************************/
    void PanelSelected(int Index);


};

} // end namespace Reagents

#endif // REAGENTS_REAGENTWIDGET_H

