



/****************************************************************************/
/*! \file ReagentRMSWidget.h
 *
 *  \brief ReagentsWidget definition.
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
#ifndef REAGENTGROUPWIDGET_H
#define REAGENTGROUPWIDGET_H

#include "Core/Include/ReagentGroupModel.h"
#include "Core/Include/DataConnector.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/SliderControl.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "MainMenu/Include/FileView.h"
#include "Reagents/Include/ModifyReagentGroupColorDlg.h"
#include "HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColorList.h"

#include "KeyBoard/Include/KeyBoard.h"

namespace Reagents {

namespace Ui {
    class CReagentGroupWidget;
}

/****************************************************************************/
/**
 * \brief Main  reagents widget listing all reagents available
 */
/****************************************************************************/
class CReagentGroupWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT
    friend class CTestReagents;

private:
    Ui::CReagentGroupWidget *mp_Ui;                     //!< User interface
    MainMenu::CBaseTable *mp_TableWidget;           //!< Reagent table
    Core::CReagentGroupModel m_ReagentGroupModel;             //!< Model for the table
    DataManager::CDataReagentGroupList *mp_ReagentGroupList;  //!< Reagent list
    MainMenu::CMainWindow *mp_MainWindow;           //!< Reference to main window.
    MainMenu::CMessageDlg m_MessageDlg;             //!< Information Message Dialog
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole; //!< Current user role
    bool m_ProcessRunning;                          //!< Process running state
    bool m_ShowMessageDialog;                       //!< To show Information Message Dialog
    DataManager::CReagentGroup const *mp_ReagentGroup;        //!< Reagent object
    DataManager::CReagentGroupColorList *mp_CReagentGroupColorList; //!< CReagentGroupColor object
    CModifyReagentGroupColorDlg *mp_ModifyReagentColorDlg;        //!< Edit reagent dialog
    QString m_strSelectColor;       ///<  Definition/Declaration of variable m_strSelectColor
signals:

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when GroupColor is updated.
     *
     */
    /****************************************************************************/
    void ReagentGroupColorUpdated();

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when ReagentList is updated.
     *
     */
    /****************************************************************************/
     void UpdateReagentGroupList();

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when ReagentList is updated.
     *
     */
    /****************************************************************************/
    void UpdateReagentGroup(DataManager::CReagentGroup&);
protected:
    void showEvent(QShowEvent *);
    void changeEvent(QEvent *p_Event);

private:
    void ResizeHorizontalSection();
    void RetranslateUI();
    void ResetButtons();

public:
    explicit CReagentGroupWidget(QWidget *p_Parent = 0);
    ~CReagentGroupWidget();
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);
    void SetPtrToMainWindow(DataManager::CReagentGroupColorList *p_ReagentGroupColorList,DataManager::CDataReagentGroupList *p_ReagentGroupList,
                                        MainMenu::CMainWindow *p_MainWindow);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnEdit
     */
    /****************************************************************************/
    void OnEdit();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SelectionChanged
     */
    /****************************************************************************/
    void SelectionChanged(QModelIndex Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnUserRoleChanged
     */
    /****************************************************************************/
    void OnUserRoleChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProcessStateChanged
     */
    /****************************************************************************/
    void OnProcessStateChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnUpdateReagentGroup
     */
    /****************************************************************************/
    void OnUpdateReagentGroup(DataManager::CReagentGroup&);
};

} // end namespace Reagents

#endif // REAGENTGROUPWIDGET_H


