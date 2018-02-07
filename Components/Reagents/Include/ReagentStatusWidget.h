#ifndef REAGENTSTATUSWIDGET_H
#define REAGENTSTATUSWIDGET_H

#include "Core/Include/ReagentStatusModel.h"
#include "Core/Include/DataConnector.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/FileView.h"


#include "KeyBoard/Include/KeyBoard.h"

namespace Reagents {

namespace Ui {
    class CReagentStatusWidget;
}



//!< Button Type -Edit, New or Copy
typedef enum {
    REDIT_BTN_CLICKED = 1,
    SET_AS_EMPTY_BTN_CLICKED,
    SET_AS_FULL_BTN_CLICKED,
    RESET_DATA_BTN_CLICKED
}ButtonGroup_t;
/****************************************************************************/
/**
 * \brief Main  reagents widget listing all reagents available
 */
/****************************************************************************/
class CReagentStatusWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT
    friend class CTestReagents;

private:
    Ui::CReagentStatusWidget *mp_Ui;      //!< User interface
    MainMenu::CBaseTable *mp_TableWidget;           //!< Reagent table
    Core::CReagentStatusModel m_ReagentStatusModel;             //!< Model for the table
    Core::CDataConnector *mp_DataConnector;         //!< Global data container
    DataManager::CDataReagentList *mp_ReagentList;  //!< Reagent list
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole; //!< Current user role
    bool m_ProcessRunning;                          //!< Process running state
    bool m_ShowMessageDialog;                       //!< To show Information Message Dialog
    DataManager::CReagent *mp_Reagent;              //!< Reagent object
    DataManager::CDashboardStation *mp_DashStation;        //!< Station object
    QStringList m_ReagentList;                      //!< List of Reagents
    Global::RMSOptions_t m_RMSOptions;       ///<  Definition/Declaration of variable m_RMSOptions
    Global::RMSOptions_t m_RMSCleaningOptions;       ///<  Definition/Declaration of variable m_RMSCleaningOptions
    QModelIndex m_CurrentIndex;       ///<  Definition/Declaration of variable m_CurrentIndex
    QString m_strSetAsEmpty;       ///<  Definition/Declaration of variable m_strSetAsEmpty
    QString m_strResetData;       ///<  Definition/Declaration of variable m_strResetData
    QString m_strSetAsFull;       ///<  Definition/Declaration of variable m_strSetAsFull
    QString m_strBottleCheckConfirm;    ///<  Definition/Declaration of variable m_strBottleCheckConfirm
    QString m_CurrentStationName;       ///<  Definition/Declaration of variable m_CurrentStationName
    QList<QString> m_StationList;       ///<  Definition/Declaration of variable m_StationList
    QString m_strConfirmMsg;       ///<  Definition/Declaration of variable m_strConfirmMsg
    QString m_strYes;       ///<  Definition/Declaration of variable m_strYes
    QString m_strNo;       ///<  Definition/Declaration of variable m_strNo
    bool m_EnableBottleCheck;
protected:
    void showEvent(QShowEvent *);
    void changeEvent(QEvent *p_Event);
private:
    void PopulateReagentList();
    void ResizeHorizontalSection();
    void RetranslateUI();
    void ResetButtons();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ControlColumnShow
     *
     *  \return from ControlColumnShow
     */
    /****************************************************************************/
    void ControlColumnShow();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsReagentExpired
     *
     *  \return from IsReagentExpired
     */
    /****************************************************************************/
    bool IsReagentExpired();

public:
    explicit CReagentStatusWidget(QWidget *p_Parent = 0);
    ~CReagentStatusWidget();
    void SetUserSettings(DataManager::CUserSettings *p_UserSettings);
    void SetPtrToMainWindow(Core::CDataConnector *p_DataConnector,
                            DataManager::CDataReagentList *p_ReagentList,
                            KeyBoard::CKeyBoard *p_KeyBoard = NULL);
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnSetAsEmpty
     */
    /****************************************************************************/
    void OnSetAsEmpty();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnSetAsFull
     */
    /****************************************************************************/
    void OnSetAsFull();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnResetData
     */
    /****************************************************************************/
    void OnResetData();
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
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of EnableBottleCheckFlag
     *
     */
    /****************************************************************************/
    void EnableBottleCheckFlag();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of DisableBottleCheckFlag
     *
     */
    /****************************************************************************/
    void DisableBottleCheckFlag();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of EnableBottleCheck
     *  \param bSet = true is enabled, false is disabled
     *
     */
    /****************************************************************************/
    void EnableBottleCheck(bool bSet);
public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of RMSChanged
     */
    /****************************************************************************/
    void RMSChanged(const Global::RMSOptions_t RMSValue);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of RMSCleaningChanged
     *  \param  RMSValue = rms option
     */
    /****************************************************************************/
    void RMSCleaningChanged(const Global::RMSOptions_t RMSValue);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateSelectedStationList
     */
    /****************************************************************************/
    void UpdateSelectedStationList(QList<QString>&);

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
     *  \brief  Definition/Declaration of signal UpdateStationSetAsFull
     */
    /****************************************************************************/
    void UpdateStationSetAsFull(const QString&);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateStationResetData
     */
    /****************************************************************************/
    void UpdateStationResetData(const QString&);
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

};

} // end namespace Reagents

#endif // REAGENTSTATUSWIDGET_H

