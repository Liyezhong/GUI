#ifndef CDASHBOARDWIDGET_H
#define CDASHBOARDWIDGET_H

#include <QWidget>
#include <HimalayaDataContainer/Helper/Include/Global.h>
#include  <QDateTime>
#include "MainMenu/Include/MainWindow.h"

namespace Core
{
    class CDataConnector;
}

namespace MsgClasses
{
    class CmdProgramSelectedReply;
    class CmdStationSuckDrain;
    class CmdLockStatus;
    class CmdCurrentProgramStepInfor;
    class CmdRecoveryFromPowerFailure;
}


namespace MainMenu
{
    class CMainWindow;
    class CMessageDlg;
}

namespace DataManager
{
    class CDataProgramList;
    class CProgram;
    class CHimalayaUserSettings;
    class CUserSettings;
}

namespace Dashboard {

namespace Ui {
    class CDashboardWidget;
}

class CDashboardDateTimeWidget;


/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class Dashboard::CDashboardWidget
 */
/****************************************************************************/
class CDashboardWidget : public QWidget
{
    Q_OBJECT
    
public:
    ///Program stage status, some stage(filling/draining) the "Pause" button can not be pressed.
    ///so it should be disabled.
    typedef enum {
        Undefined,
        Enabled,
        Disabled
    } ProgramStageStatus_t;

    ///Program status, defined for "Start", "Abort" button status control
    typedef enum {
        Undefined_ProgramStatus,
        ProgramRunning,
        Paused,
        Aborting,
        Completed,
        CompletedAsSafeReagent,
        CompletedAsPowerFailure,
        Aborted
    } ProgramStatus_t;

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CDashboardWidget
     *
     *  \param p_DataConnector
     *  \param p_Parent
     *
     *  \return from CDashboardWidget
     */
    /****************************************************************************/
    explicit CDashboardWidget(Core::CDataConnector *p_DataConnector,
                               MainMenu::CMainWindow *p_Parent = NULL);
    ~CDashboardWidget();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsAbortEnabled
     *
     *  \return from IsAbortEnabled
     */
    /****************************************************************************/
    bool IsAbortEnabled();
protected:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function changeEvent
     *
     *  \param p_Event = QEvent type parameter
     *
     *  \return from changeEvent
     */
    /****************************************************************************/
    void changeEvent(QEvent *p_Event);

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsParaffinInProgram
     *
     *  \param p_Program =  DataManager::CProgram type parameter
     *
     *  \return from IsParaffinInProgram
     */
    /****************************************************************************/
    bool IsParaffinInProgram(const DataManager::CProgram* p_Program);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsFixationInFirstStep
     *
     *  \return true if the first program step is fixation reagent group
     */
    /****************************************************************************/
    bool IsFixationInFirstStep();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetParaffinHeatingDuration
     *
     *  \return the paraffin heating duration
     */
    /****************************************************************************/
    int GetParaffinHeatingDuration();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetASAPTime
     *
     *  \return from GetASAPTime
     */
    /****************************************************************************/
    int GetASAPTime(int, int, int, int&, bool&);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RetranslateUI
     *
     *  \return from RetranslateUI
     */
    /****************************************************************************/
    void RetranslateUI();
    void SetCassetteNumber();
    bool IsOKPreConditionsToRunProgram(QString& reagentExpiredFlag);
    Ui::CDashboardWidget *ui;       ///<  Definition/Declaration of variable ui
    Core::CDataConnector *mp_DataConnector;          //!< Data object
    MainMenu::CMainWindow *mp_MainWindow;           //!< Reference to main window.
    DataManager::CDataProgramList *mp_ProgramList;       ///<  Definition/Declaration of variable mp_ProgramList
    QString m_NewSelectedProgramId;       ///<  Definition/Declaration of variable m_NewSelectedProgramId
    int m_ParaffinStepIndex;       ///<  Definition/Declaration of variable m_ParaffinStepIndex
    DataManager::CHimalayaUserSettings* m_pUserSetting;       ///<  Definition/Declaration of variable m_pUserSetting
    MainMenu::CMessageDlg* mp_ProgramWillCompleteMsgDlg;                      //!< Message Dialogue
    MainMenu::CMessageDlg* mp_TissueProtectPassedMsgDlg;
    MainMenu::CMessageDlg* mp_RetortLidOpenMsgDlg;              //!< retort lid open message dialog
    MainMenu::CMessageDlg* mp_OvenLidOpenMsgDlg;              //!< Owen lid open message dialog
    MainMenu::CMessageDlg* mp_PausingMsgDlg;                    //!< message dialog "It is pausing..."
    MainMenu::CMessageDlg* mp_RemoveSpecimenDlg;
    MainMenu::CMessageDlg* mp_RemoveSpecimenWhenCompletedDlg;
    MainMenu::CMessageDlg* mp_RemoveSpecimenWhenAbortedDlg;

    QString m_strCheckSafeReagent;
    QString m_strNotFoundStation;
    QString m_strCheckEmptyStation;
    QString m_strDiffTemp;       ///<  Declaration of variable m_strDiffTemp
    static QString m_SelectedProgramId;
    QList<QString> m_StationList;
    int m_TimeProposedForProgram;           //!< Time costed only for the whole program, exclude the time for the delayed time
    int m_CostedTimeBeforeParaffin;         //!< Time costed before the program step of paraffin.
    int m_ParaffinHeatingDuration;
    QDateTime m_EndDateTime;
    int m_TimeDelta;
    QDateTime m_AsapEndDateTime;
    QDateTime m_ParaffinStartHeatingTime;
    QString m_strCannotStartParaffinMelt;
    QString m_strWaitTimeforParaffinMelt;
    QString m_strPromptProgramDelay;
    QString m_strInputCassetteBoxTitle;
    bool m_ProgramStartReady;
    QString m_strProgramWillComplete;
    QString m_strProgramComplete;
    QString m_strProgramCompleteSuccessfully;
    QString m_strProgramCompleteSafeReagent;
    QString m_strTissueProtectPassed;
    QString m_strTissueProtectPassed_Warning;
    QString m_strOvenCoverOpen;
    QString m_strRetortCoverOpen;
    QString m_strItIsPausing;
    QString m_strItIsResuming;
    QString m_PowerFailureBoxTitle;
    QString m_strTakeOutSpecimen;
    QString m_strRetortContaminated;
    QString m_strWaitRotaryValveHeatingPrompt;
    QString m_strCoolingDown;
    QDateTime m_StartDateTime;
    QString m_strProgramIsAborted;
    int m_CurProgramStepIndex;
    bool m_ProcessRunning;                      //!< Process running state
    bool m_IsDrainingWhenPrgrmCompleted;
    bool m_bRetortLocked;
    QString m_strRetortNotLock;
    QString m_strNotStartRMSOFF;
    QString m_strNotStartExpiredReagent;
    int m_iWhichStepHasNoSafeReagent;
    QString m_strStartExpiredReagent;
    QString m_strChangeCassetteBoxTitle;
    QString m_strAddCassete;
    bool m_bIsFirstStepFixation;
    quint32 m_TotalCassette;
    bool m_HaveSucked;
    ProgramStageStatus_t m_ProgramStageStatus;
    ProgramStatus_t m_ProgramStatus;
    bool m_IsProgramAbortedOrCompleted;
    Dashboard::CDashboardDateTimeWidget *mp_wdgtDateTime;       ///<  Definition/Declaration of variable mp_wdgtDateTime
    bool m_IsInAppendCasseteStatus;
    bool m_bWaitRotaryValveHeatingPrompt;                       //!< waiting for rotary valve heating to be completed message dialog
    QString m_strReagentExpiredLog;
    QTimer* m_pCheckRetortLidTimer;
    QString m_strReadyStartProgram;
    bool m_ShowReadyStartPrompt;
    QString m_checkMachinePrompt;
public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnUnselectProgram
     */
    /****************************************************************************/
    void OnUnselectProgram();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnOvenCoverClosed
     */
    /****************************************************************************/
    void OnOvenCoverClosed(bool closed);

private slots:
    void PrepareSelectedProgramChecking(const QString& selectedProgramId);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramSelectedReply
     */
    /****************************************************************************/
    void OnProgramSelectedReply(const MsgClasses::CmdProgramSelectedReply& cmd);
    void OnRecoveryFromPowerFailure(const MsgClasses::CmdRecoveryFromPowerFailure& cmd);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnSelectEndDateTime
     */
    /****************************************************************************/
    void OnSelectEndDateTime(const QDateTime&);
    void RequestAsapDateTime();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramStartReadyUpdated
     */
    /****************************************************************************/
    void OnProgramStartReadyUpdated();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramWillComplete
     */
    /****************************************************************************/
    void OnProgramWillComplete();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of CancelProgramWillCompletePrompt
     */
    /****************************************************************************/
    void CancelProgramWillCompletePrompt();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of CancelTissueProtectPassedPrompt
     */
    /****************************************************************************/
    void CancelTissueProtectPassedPrompt();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of CancelRetortCoverOpenMessage
     */
    /****************************************************************************/
    void CancelRetortCoverOpenMessage();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnTissueProtectPassed
     *  \param  flag = To indicate if safe reagent is totally passed or passed with warning
     */
    /****************************************************************************/
    void OnTissueProtectPassed(bool flag);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnOvenCoverOpen
     */
    /****************************************************************************/
    void OnOvenCoverOpen();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnRetortCoverOpen
     */
    /****************************************************************************/
    void OnRetortCoverOpen();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnPowerFailureMsg
     */
    /****************************************************************************/
    void OnPowerFailureMsg();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of OnShowMsgDialog
     */
    /****************************************************************************/
    void OnShowMsgDialog(DataManager::MsgBoxType_t msgBoxType);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of OnDismissMsgDialog
     */
    /****************************************************************************/
    void OnDismissMsgDialog();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of OnWaitRotaryValveHeatingPrompt
     */
    /****************************************************************************/
    void OnWaitRotaryValveHeatingPrompt();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of OnDismissRotaryValveHeatingPrompt
     */
    /****************************************************************************/
    void OnDismissRotaryValveHeatingPrompt();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of OnCoolingDown
     */
    /****************************************************************************/
    void OnCoolingDown();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramAborted
     *  \param  IsRetortContaminated = is retort contaminated?
     */
    /****************************************************************************/
    void OnProgramAborted(bool IsRetortContaminated);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramBeginAbort
     */
    /****************************************************************************/
    void OnProgramBeginAbort();
    /****************************************************************************/
    /*!
     *  \brief  Declaration of OnCleanPrgmCompleteAsSafeReagent
     */
    /****************************************************************************/
    void OnCleanPrgmCompleteAsSafeReagent();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramCompleted
     */
    /****************************************************************************/
    void OnProgramCompleted(DataManager::CompletedProgramType_t completedProgramType
                            = DataManager::COMPLETED_PROGRAM_GENERAL,
                            bool IsRetortContaminated = true);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramRunBegin
     */
    /****************************************************************************/
    void OnProgramRunBegin();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramPaused
     */
    /****************************************************************************/
    void OnProgramPaused();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnPauseButtonEnble
     */
    /****************************************************************************/
    void OnPauseButtonEnable(bool bEnable);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnStartButtonEnable
     */
    /****************************************************************************/
    void OnStartButtonEnable(bool bEnable);
    /****************************************************************************/
    /*!
    *  \brief  Definition/Declaration of OnPauseTimeout15Mintues
    */
    /****************************************************************************/
    void OnPauseTimeout15Mintues();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnStationSuckDrain
     */
    /****************************************************************************/
    void OnStationSuckDrain(const MsgClasses::CmdStationSuckDrain & cmd);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnRetortLockStatusChanged
     */
    /****************************************************************************/
    void OnRetortLockStatusChanged(const MsgClasses::CmdLockStatus& cmd);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnCurrentProgramStepInforUpdated
     */
    /****************************************************************************/
    void OnCurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor & cmd);
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
     *  \brief  Definition/Declaration of CheckPreConditionsToRunProgram
     */
    /****************************************************************************/
    void CheckPreConditionsToRunProgram();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TakeOutSpecimenAndWaitRunCleaning
     *  \param  lastReagentGroupId = last ReagentGroupId
     *  \return from TakeOutSpecimenAndWaitRunCleaning
     */
    /****************************************************************************/
    void TakeOutSpecimenAndWaitRunCleaning(const QString& lastReagentGroupId);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OnTimerCheckRetortLid
     *  \return from OnTimerCheckRetortLid
     */
    /****************************************************************************/
    void OnTimerCheckRetortLid();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OnProgramResumeAfterPowerFailure
     *  \return from OnProgramResumeAfterPowerFailure
     */
    /****************************************************************************/
    void OnProgramResumeAfterPowerFailure();
 signals:
    /****************************************************************************/
    /*!
     *  \brief  refresh the program selection
     *  \param reset = whether reset the selected program or not
     */
    /****************************************************************************/
    void ResetFocus(bool reset);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal AddItemsToFavoritePanel
     *  \param  bOnlyAddCleaningProgram = whether only show cleaning program or not
     */
    /****************************************************************************/
    void AddItemsToFavoritePanel(bool bOnlyAddCleaningProgram = false);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateItemsToFavoritePanel
     *
     */
    /****************************************************************************/
    void UpdateItemsToFavoritePanel();
    /****************************************************************************/
    /*!
     *  \brief  when selete a program, emit signal ProgramSelected
     *  \param  programId = the seleted program ID
     *  \param  selectedStationList = the used station for the selected program
     */
    /****************************************************************************/
    void ProgramSelected(QString & programId, QList<QString>& selectedStationList);
    /****************************************************************************/
    /*!
     *  \brief  when selete a program, emit signal ProgramSelected
     *  \param  programId = the seleted program ID
     *  \param asapEndTime = as soon as possible end time
     *  \param bIsFirstStepFixation = the first program step is fixation reagent
     *  \param selectedStationList = the selected stations
     *  \param firstProgramStepIndex = the first program step index
     */
    /****************************************************************************/
    void ProgramSelected(QString & programId, int asapEndTime,
                         bool bIsFirstStepFixation, QList<QString>& selectedStationList,
                         int firstProgramStepIndex);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateSelectedStationList
     */
    /****************************************************************************/
    void UpdateSelectedStationList(QList<QString>&);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of signal ProgramActionStarted
     *  \param programActionType = opration type, eg.Start/pause/abort.
     *  \param  remainingTimeTotal = the total remaining time
     *  \param  startDateTime = program start datetime
     *  \param IsResume = is resume to run program
     */
    /****************************************************************************/
    void ProgramActionStarted(DataManager::ProgramActionType_t programActionType, int remainingTimeTotal, const QDateTime& startDateTime, bool IsResume);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramActionStopped
     */
    /****************************************************************************/
    void ProgramActionStopped(DataManager::ProgramStatusType_t);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateUserSetting
     */
    /****************************************************************************/
    void UpdateUserSetting(DataManager::CUserSettings&);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal SwitchToFavoritePanel
     */
    /****************************************************************************/
    void SwitchToFavoritePanel();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UndoProgramSelection
     */
    /****************************************************************************/
    void UndoProgramSelection();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal OnInteractStart
     */
    /****************************************************************************/
    void OnInteractStart();    
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateProgram
     */
    /****************************************************************************/
    void UpdateProgram(DataManager::CProgram &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal SendAsapDateTime
     *  \param asapDateTime = as soon as possible datetime
     *  \param bIsFirstStepFixation = whether the first step is fixation reagent or not
     *  \param bNeedUpdateUI = whether update GUI or not
     *
     */
    /****************************************************************************/
    void SendAsapDateTime(int asapDateTime, bool bIsFirstStepFixation, bool bNeedUpdateUI);

};

} // end namespace Dashboard

#endif // CDASHBOARDWIDGET_H

