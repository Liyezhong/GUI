/****************************************************************************/
/*! \file ModifyReagentStationDlg.cpp
 *
 *  \brief ModifyReagentStationDlg Implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-29
 *   $Author:  $ Swati Tiwari
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
#ifndef MODIFYREAGENTSTATIONDLG_H
#define MODIFYREAGENTSTATIONDLG_H


#include "HimalayaDataContainer/Containers/Reagents/Include/Reagent.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "Reagents/Include/ReagentStatusWidget.h"
#include "MainMenu/Include/ScrollTable.h"
#include "Core/Include/ReagentStationEditModel.h"

namespace Reagents {

namespace Ui {
    class CModifyReagentStationDlg;
}

/****************************************************************************/
/**
 * \brief Edit dialog for  reagents of the user
 */
/****************************************************************************/
class CModifyReagentStationDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class CTestReagents;

private:
    Ui::CModifyReagentStationDlg *mp_Ui;                   //!< User interface
    MainMenu::CMessageDlg m_MessageDlg;             //!< Message dialog object
    DataManager::CDashboardStation m_EditedStation; //!< Definition/Declaration of variable m_EditedStation
    //Flags
    bool m_ProcessRunning;                          //!< Process running state
    //UI related
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    MainMenu::CBaseTable *mp_TableWidget;           //!< Reagent table
    Core::CReagentStationEditModel m_ReagentsDataModel;             //!< Model for the table
    Core::CDataConnector *mp_DataConnector;         //!< Global data container
    QString m_strCancel;       ///<  Definition/Declaration of variable m_strCancel
    QString m_strClose;       ///<  Definition/Declaration of variable m_strClose
    QString m_strInforMsg;       ///<  Definition/Declaration of variable m_strInforMsg
    QString m_strOK;       ///<  Definition/Declaration of variable m_strOK

public:
    explicit CModifyReagentStationDlg(QWidget *p_Parent = NULL,
                               MainMenu::CMainWindow *p_MainWindow = NULL,
                               Core::CDataConnector *p_DataConnector= NULL);
    virtual ~CModifyReagentStationDlg();
    /****************************************************************************/
    /*!
    *   \brief Set the edited dashboard station
    *   \param p_Station = the edited station
    */
    /****************************************************************************/
    void SetEditedDashboardStation(DataManager::CDashboardStation* p_Station);

private:
    void RetranslateUI();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PopulateReagentList
     *
     *  \return from PopulateReagentList
     */
    /****************************************************************************/
    void PopulateReagentList();
    void ResizeHorizontalSection();

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnOk
     */
    /****************************************************************************/
    void OnOk();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnCancel
     */
    /****************************************************************************/
    void OnCancel();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SelectionChanged
     */
    /****************************************************************************/
    void SelectionChanged(QModelIndex Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProcessStateChanged
     */
    /****************************************************************************/
    void OnProcessStateChanged();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnUserRoleChanged
     */
    /****************************************************************************/
    void OnUserRoleChanged();
protected:
    void changeEvent(QEvent *p_Event);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function showEvent
     *
     *  \param p_Event = QShowEvent type parameter
     *
     *  \return from showEvent
     */
    /****************************************************************************/
    void showEvent(QShowEvent * p_Event);
signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when Cancel button in the dailog is pressed.
     *
     */
    /****************************************************************************/
    void CancelPressed();

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when Station Reagent changed to empty
     *
     */
    /****************************************************************************/
    void UpdateStationSetAsEmpty(const QString&);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when DashboardStation Reagent is Changed.
     *
     */
    /****************************************************************************/
    void UpdateStationChangeReagent(const QString&, const QString&);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UnselectProgram
     */
    /****************************************************************************/
    void UnselectProgram();

};
}

#endif // MODIFYREAGENTSTATUSDLG_H

