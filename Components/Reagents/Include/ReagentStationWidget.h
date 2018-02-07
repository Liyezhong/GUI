#ifndef REAGENTSTATIONWIDGET_H
#define REAGENTSTATIONWIDGET_H

#include "Core/Include/ReagentStationModel.h"
#include "Core/Include/DataConnector.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/FileView.h"
#include "Reagents/Include/ModifyReagentStationDlg.h"
#include "KeyBoard/Include/KeyBoard.h"

namespace Reagents {
namespace Ui{
    class CReagentStationWidget;
}

class CModifyReagentStationDlg;

/**********************************************************/
/**
 * \brief Main  reagents widget listing all reagents available
 */
/****************************************************************************/

class CReagentStationWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT
    friend class CTestReagents;

private:
    Ui::CReagentStationWidget *mp_Ui;      //!< User interface
    MainMenu::CBaseTable *mp_TableWidget;           //!< Reagent table
    Core::CReagentStationModel m_ReagentStationModel;             //!< Model for the table
    Core::CDataConnector *mp_DataConnector;         //!< Global data container
    DataManager::CDataReagentList *mp_ReagentList;  //!< Reagent list
    DataManager::CReagent m_SelectedReagent;        //!< Currently selected reagent
    MainMenu::CMessageDlg m_MessageDlg;             //!< Information Message Dialog
    bool m_UserRoleChanged;                         //!< True if user Role changed else false
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole; //!< Current user role
    bool m_ProcessRunning;                          //!< Process running state
    bool m_ShowMessageDialog;                       //!< To show Information Message Dialog
    DataManager::CReagent *mp_Reagent;              //!< Reagent object
    DataManager::CDashboardStation *mp_DashStation;        //!< Station object
    CModifyReagentStationDlg *mp_ModifiyReagentStationDlg;        //!< Edit reagent dialog
    KeyBoard::CKeyBoard *mp_KeyBoard;       ///<  Definition/Declaration of variable mp_KeyBoard
    QString m_strSelectReagent;       ///<  Definition/Declaration of variable m_strSelectReagent
    QList<QString> m_StationList;       ///<  Definition/Declaration of variable m_StationList
protected:
    void showEvent(QShowEvent *);
    void changeEvent(QEvent *p_Event);
private:
    void PopulateReagentList();
    void ResizeHorizontalSection();
    void RetranslateUI();
    void ResetButtons();

public:
    explicit CReagentStationWidget(QWidget *p_Parent = 0);
    ~CReagentStationWidget();
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);
    /****************************************************************************/
    /*!
     *  \brief Used to set pointer to DataConnector, ReagentList, mainwindow, KeyBoard used to retreive user role and
     *         process state changed.
     *  \param p_DataConnector = pointer to dataConnector
     *  \param p_ReagentList = pointer to all reagents
     *  \param p_MainWindow = pointer to main window
     *  \param p_KeyBoard = pointer to keyboard
     */
    /****************************************************************************/
    void SetPtrToMainWindow(Core::CDataConnector *p_DataConnector,
                            DataManager::CDataReagentList *p_ReagentList,
                            MainMenu::CMainWindow *p_MainWindow,
                            KeyBoard::CKeyBoard *p_KeyBoard = NULL);
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
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of StationReagentUpdated
     */
    /****************************************************************************/
    void StationReagentUpdated(const QString&);
signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when ReagentList is updated.
     *
     */
    /****************************************************************************/
    void UpdateReagentList();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateStationChangeReagent
     */
    /****************************************************************************/
    void UpdateStationChangeReagent(const QString&, const QString&);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateStationSetAsEmpty
     */
    /****************************************************************************/
    void UpdateStationSetAsEmpty(const QString&);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UnselectProgram
     */
    /****************************************************************************/
    void UnselectProgram();

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateSelectedStationList
     */
    /****************************************************************************/
    void UpdateSelectedStationList(QList<QString>&);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal HandleStationChangeReagent
     */
    /****************************************************************************/
    void HandleStationChangeReagent(const QString&, const QString&);


};

} // end namespace Reagents

#endif // REAGENTSTATUSWIDGET_H

