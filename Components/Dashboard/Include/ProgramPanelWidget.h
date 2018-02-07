#ifndef PROGRAMPANELWIDGET_H
#define PROGRAMPANELWIDGET_H

#include "MainMenu/Include/PanelFrame.h"
#include <QButtonGroup>
#include "HimalayaDataContainer/Helper/Include/Global.h"
#include <QDateTime>
#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"


namespace MsgClasses
{
    class CmdCurrentProgramStepInfor;
    class CmdLockStatus;
    class CmdRecoveryFromPowerFailure;
}

namespace MainMenu
{
    class CMainWindow ;
    class CMessageDlg;
}

namespace Core
{
    class CDataConnector;
}

namespace DataManager
{
    class CHimalayaUserSettings;
    class CDataProgramList;
}

namespace Dashboard {

namespace Ui {
    class CProgramPanelWidget;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class Dashboard::CProgramPanelWidget
 */
/****************************************************************************/
class CProgramPanelWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT
    
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CProgramPanelWidget
     *
     *  \param parent = QWidget type parameter
     *
     *  \return from CProgramPanelWidget
     */
    /****************************************************************************/
    explicit CProgramPanelWidget(QWidget *parent = 0);
    ~CProgramPanelWidget();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetPtrToMainWindow
     *
     *  \param p_MainWindow = MainMenu::CMainWindow type parameter
     *  \param p_DataConnector =  Core::CDataConnector type parameter
     *
     *  \return from SetPtrToMainWindow
     */
    /****************************************************************************/
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ChangeStartButtonToStopState
     *
     *  \return from ChangeStartButtonToStopState
     */
    /****************************************************************************/
    void ChangeStartButtonToStopState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ChangeStartButtonToStartState
     *
     *  \return from ChangeStartButtonToStartState
     */
    /****************************************************************************/
    void ChangeStartButtonToStartState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function EnableStartButton
     *
     *  \param bEnable = bool type parameter
     *  \param bProgramNotStart = bool type parameter
     *
     *  \return from EnableStartButton
     */
    /****************************************************************************/
    void EnableStartButton(bool bEnable, bool bProgramNotStart = false);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResumePauseRunningStatus
     *
     *  \param bEnable = bool type parameter
     *
     *  \return from ResumePauseRunningStatus
     */
    /****************************************************************************/
    void ResumePauseRunningStatus(bool bEnable);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function EnablePauseButton
     *
     *  \param bEnable = bool type parameter
     *
     *  \return from EnablePauseButton
     */
    /****************************************************************************/
    void EnablePauseButton(bool bEnable);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PauseTimeout15Mintues
     *
     *
     *  \return from PauseTimeout15Mintues
     */
    /****************************************************************************/
    void PauseTimeout15Mintues();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsResumeRun
     *
     *  \param bSet = bool type parameter
     *
     *  \return from IsResumeRun
     */
    /****************************************************************************/
    void IsResumeRun(bool bSet);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsResumeRun
     *
     *  \return from IsResumeRun
     */
    /****************************************************************************/
    bool IsResumeRun();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsAbortEnabled
     *
     *  \return from IsAbortEnabled
     */
    /****************************************************************************/
    bool IsAbortEnabled();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SwitchToProgramRunningStatus
     *  \param  cmd = CmdRecoveryFromPowerFailure type parameter
     *
     *  \return from SwitchToProgramRunningStatus
     */
    /****************************************************************************/
    void SwitchToProgramRunningStatus(const MsgClasses::CmdRecoveryFromPowerFailure& cmd);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of function ProgramStartReady
     *  \param  bSet = the set value
     *
     *  \return from ProgramStartReady
     */
    /****************************************************************************/
    void ProgramStartReady(bool bSet);
    /****************************************************************************/
    /*!
     *  \brief  Declaration of function WaitRotaryValveHeatingPrompt
     *  \param  bSet = the set value
     *
     *  \return from WaitRotaryValveHeatingPrompt
     */
    /****************************************************************************/
    void WaitRotaryValveHeatingPrompt(bool bSet);
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
     *  \brief Declaration of signal AddItemsToFavoritePanel
     *  \param bOnlyAddCleaningProgram = indicate only add cleaning program
     */
    /****************************************************************************/
    void AddItemsToFavoritePanel(bool bOnlyAddCleaningProgram);
    /****************************************************************************/
    /*!
     *  \brief Declaration of signal AddItemsToFavoritePanel
     *
     */
    /****************************************************************************/
    void AddItemsToFavoritePanel();
    /****************************************************************************/
    /*!
     *  \brief  send signal PrepareSelectedProgramChecking to master, for checking program
     *  \param selectedProgramId = the selected program id
     *  \param bCheckEndDatetimeAgain = whether check end datetime again or not
     */
    /****************************************************************************/
    void PrepareSelectedProgramChecking(const QString& selectedProgramId, bool bCheckEndDatetimeAgain = false);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal OnSelectEndDateTime
     */
    /****************************************************************************/
    void OnSelectEndDateTime(const QDateTime &);
    /****************************************************************************/
    /*!
     *  \brief when Program select, send this signal
     *  \param ProgramId = the curent selected program id.
     *  \param asapEndTime = the as soon as possible end time in seconds
     *  \param bIsFirstStepFixation = whether the first step is fixation or not
     *  \param selectedStationList = store the used station IDs for the current selected program
     *  \param firstProgramStepIndex = the first program step index
     */
    /****************************************************************************/
    void ProgramSelected(QString& ProgramId, int asapEndTime,
                         bool bIsFirstStepFixation, QList<QString>& selectedStationList,
                         int firstProgramStepIndex);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramActionStopped
     */
    /****************************************************************************/
    void ProgramActionStopped(DataManager::ProgramStatusType_t);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramActionStarted
     *  \param  programActionType = program operation type
     *  \param  remainingTimeTotal = the total program remaining time\
     *  \param  startDateTime = the program start datetime
     *  \param  IsResume = is first time to press "Start", or resume to run program
     */
    /****************************************************************************/
    void ProgramActionStarted(DataManager::ProgramActionType_t programActionType, int remainingTimeTotal, const QDateTime& startDateTime, bool IsResume);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal CurrentProgramStepInforUpdated
     *  \param  Command = the current program updating information passed by this command
     */
    /****************************************************************************/
    void CurrentProgramStepInforUpdated(const MsgClasses::CmdCurrentProgramStepInfor & Command);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UndoProgramSelection
     */
    /****************************************************************************/
    void UndoProgramSelection();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateProgram
     */
    /****************************************************************************/
    void UpdateProgram(DataManager::CProgram &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal CheckPreConditionsToRunProgram
     *
     */
    /****************************************************************************/
    void CheckPreConditionsToRunProgram();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal CheckPreConditionsToRunProgram
     *
     */
    /****************************************************************************/
    void PreTestDone();

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

public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramStartReadyUpdated
     */
    /****************************************************************************/
     void OnProgramStartReadyUpdated();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnWaitRotaryValveHeatingPrompt
      */
     /****************************************************************************/
     void OnWaitRotaryValveHeatingPrompt();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of ResetInFavProgramButtonClicked
      */
     /****************************************************************************/
     void ResetInFavProgramButtonClicked();
private slots:
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of OnButtonClicked
      */
     /****************************************************************************/
    void OnButtonClicked(int whichBtn);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramSelected
     */
    /****************************************************************************/
    void OnProgramSelected(QString& ProgramId, int asapEndTime,
                           bool bIsFirstStepFixation, QList<QString>& selectedStationList);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramActionStarted
     */
    /****************************************************************************/
    void OnProgramActionStarted(DataManager::ProgramActionType_t ProgramActionType, int remainingTimeTotal,
                                const QDateTime& startDateTime, bool IsResume);//in seconds
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProgramActionStopped
     */
    /****************************************************************************/
    void OnProgramActionStopped(DataManager::ProgramStatusType_t ProgramStatusType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of SwitchToFavoritePanel
     */
    /****************************************************************************/
    void SwitchToFavoritePanel();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnUpdatePanelProgram
     */
    /****************************************************************************/
    void OnUpdatePanelProgram();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnPreTestDone
     */
    /****************************************************************************/
    void OnPreTestDone();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateProgramTimerStatus
     */
    /****************************************************************************/
    void UpdateProgramTimerStatus(bool enable);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnPrepareSelectedProgramChecking
     */
    /****************************************************************************/
    void OnPrepareSelectedProgramChecking(const QString&);

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RetranslateUI
     *
     *  \return from RetranslateUI
     */
    /****************************************************************************/
    void RetranslateUI();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CheckPreConditionsToPauseProgram
     *
     *  \return from CheckPreConditionsToPauseProgram
     */
    /****************************************************************************/
    bool CheckPreConditionsToPauseProgram();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CheckPreConditionsToAbortProgram
     *
     *  \return from CheckPreConditionsToAbortProgram
     */
    /****************************************************************************/
    bool CheckPreConditionsToAbortProgram();

    void ProgramReadyPrompt();

    Ui::CProgramPanelWidget *ui;       ///<  Definition/Declaration of variable ui
    QButtonGroup m_btnGroup;                                    //!< Button Group
    DataManager::ProgramActionType_t m_ProgramNextAction;       //!< Program Next Action
    bool m_IsResumeRun;       ///<  Definition/Declaration of variable m_IsResumeRun
    QString m_SelectedProgramId;       ///<  Definition/Declaration of variable m_SelectedProgramId
    Core::CDataConnector *mp_DataConnector;                     //!< Global data container
    DataManager::CHimalayaUserSettings* m_pUserSetting;       ///<  Definition/Declaration of variable m_pUserSetting
    QDateTime m_EndDateTime;       ///<  Definition/Declaration of variable m_EndDateTime
    MainMenu::CMessageDlg   *mp_MessageDlg;                      //!< Message Dialogue
    DataManager::CDataProgramList *mp_ProgramList;       ///<  Definition/Declaration of variable mp_ProgramList
    QList<QString> m_StationList;                                //!< Station list occupied

    QString m_strConfirmation;       ///<  Definition/Declaration of variable m_strConfirmation
    QString m_strAbortProgram;       ///<  Definition/Declaration of variable m_strAbortProgram

    QString m_strWarning;       ///<  Definition/Declaration of variable m_strWarning
    QString m_strYes;       ///<  Definition/Declaration of variable m_strYes
    QString m_strCancel;       ///<  Definition/Declaration of variable m_strCancel
    QString m_strProgram;       ///<  Definition/Declaration of variable m_strProgram
    QString m_strInformation;       ///<  Definition/Declaration of variable m_strInformation
    QString m_strOK;       ///<  Definition/Declaration of variable m_strOK
    QString m_strNo;       ///<  Definition/Declaration of variable m_strNo
    QString m_strAbort, m_strStart;
    bool m_startButtonDisabledAsSysError;
    bool m_pauseButtonDisabledAsSysError;
    bool m_ProgramStartReady;
    bool m_bWaitRotaryValveHeatingPrompt;
};

}// end namespace Dashboard

#endif // PROGRAMPANELWIDGET_H

