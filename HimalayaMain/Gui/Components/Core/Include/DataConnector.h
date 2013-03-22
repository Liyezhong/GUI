/****************************************************************************/
/*! \file DataConnector.h
 *
 *  \brief DataConnector definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-02-24 - 29-Nov-2012
 *   $Author:  $ M.Scherer,Ningu
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
#ifndef CORE_DATACONNECTOR_H
#define CORE_DATACONNECTOR_H

#include "Core/Include/DataContainer.h"
#include "Global/Include/Translator.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/MessageDlg.h"
#include "MainMenu/Include/WaitDialog.h"
#include "NetworkLayer/Include/MasterLinkDevice.h"
#include "Settings/Include/AlarmSettingsDlg.h"

/************************** Commands ***********************************************/
#include "DataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdDashboardStationUpdate.h"
#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdProgramUpdate.h"
#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdNewProgram.h"
#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdDeleteItem.h"
#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentAdd.h"
#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentUpdate.h"
#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentRemove.h"
#include "HimalayaDataContainer/Containers/ReagentGroups/Commands/Include/CmdReagentGroupUpdate.h"
#include "HimalayaDataContainer/Containers/Stations/Commands/Include/CmdStationChangeReagent.h"

/********************** Net Commands ************************************************/
#include "NetCommands/Include/CmdConfigurationFile.h"
#include "NetCommands/Include/CmdProcessState.h"
#include "NetCommands/Include/CmdEventStrings.h"
#include "NetCommands/Include/CmdExecutionStateChanged.h"
#include "NetCommands/Include/CmdLanguageFile.h"
#include <NetCommands/Include/CmdEventReport.h>
#include <NetCommands/Include/CmdChangeUserLevel.h>
#include <NetCommands/Include/CmdChangeAdminPassword.h>
#include <NetCommands/Include/CmdChangeAdminPasswordReply.h>
#include <NetCommands/Include/CmdChangeUserLevelReply.h>
#include <NetCommands/Include/CmdAcknEventReport.h>
#include <NetCommands/Include/CmdDayRunLogReply.h>
#include <NetCommands/Include/CmdDayRunLogReplyFile.h>
#include <NetCommands/Include/CmdDayRunLogRequest.h>
#include <NetCommands/Include/CmdDayRunLogRequestFile.h>
#include <NetCommands/Include/CmdGuiInit.h>
#include "MainMenu/Include/MessageDlg.h"
#include "MainMenu/Include/MsgBoxManager.h"
#include <HimalayaDataContainer/Containers/Stations/Commands/Include/CmdStationResetData.h>
#include <HimalayaDataContainer/Containers/Stations/Commands/Include/CmdStationSetAsEmpty.h>
#include <HimalayaDataContainer/Containers/Stations/Commands/Include/CmdStationSetAsFull.h>
#include <HimalayaDataContainer/Containers/Stations/Commands/Include/CmdUpdateStationReagentStatus.h>


//namespace MainMenu {
//    class CMsgBoxManager;
//}

namespace Core {



///Leica consumables type
typedef enum {
    KIT,         ///< Leica Kit
    BOTTLE       ///< Leica Bottle
} LeicaConsumableType_t;
///Message box buttons
enum ClickedButton_t {
    OK_BUTTON,
    YES_BUTTON,
    CONTINUE_BUTTON,
    NO_BUTTON,
    CANCEL_BUTTON,
    STOP_BUTTON
};



typedef QHash<qint16, QString> ScheduledStations_t; //!< Key- Step number, Value- Station ID

/****************************************************************************/
/**
 *  \brief Controller connecting the data model to various views
 *
 *      This class receives commands from the network layer, stores the
 *      data in the data manager and notifies the different views of the
 *      changes.
 */
/****************************************************************************/
class CDataConnector : public DataManager::CDataContainer
{
    Q_OBJECT

public:
    CDataConnector(MainMenu::CMainWindow *p_Parent);
    virtual ~CDataConnector();

    void OnAckDateAndTime(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    void OnAckTwoPhase(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    void EmitUpdateModeSignal(QString Mode);
    void OnAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    void RequestDayRunLogFileNames();
    void OnUserSettingsAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    void OnProgramAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);
    void OnReagentAck(Global::tRefType Ref, const Global::AckOKNOK &Ack);


public slots:
    void SendDateTime(QDateTime DateTime);
    void SendReagentUpdate(DataManager::CReagent &Reagent);
    void SendReagentGroupUpdate(DataManager::CReagentGroup &ReagentGroup);
    void SendStationChangeReagent(const QString& StationId, const QString& ReagentId);
    void SendStationResetData(const QString& StationId);
    void SendStationSetAsEmpty(const QString StationId);
    void SendStationSetAsFull(const QString& StationId);
    void SendRMSChanged(Global::RMSOptions_t);
    void SendUpdatedSettings(DataManager::CUserSettings &Settings);
    void SendCmdPlayTestToneAlarm(quint8 Volume, quint8 Sound, bool Type);
    void SendReagentAdd(DataManager::CReagent &Reagent);
    void SendReagentRemove(QString ReagentID);
    void SendProgramUpdate(DataManager::CProgram &Program);
    void SendProgramAdd(DataManager::CProgram &Program);
    void SendProgramRemove(QString &ProgramID);
    void SendDataImportExport(const QString Name, const QStringList &ParamList);

    void SendProgramColorUpdate(DataManager::CProgram &ColorReplacedProgram,
                                DataManager::CProgram &ColorAssignedProgram);

    void LeicaConsumablesScanMessages(QString Message,QString Title,QMessageBox::Icon Type);

    void SendUserLevel(QDataStream &DataStream);
    void SendChangepassword(QDataStream &DataStream);
    void SendSelectedDayRunLogFile(const QString &FileName);
    void OnDeviceConfigurationUpdated();
    void OnCurrentTabChanged(int CurrentTabIndex);

signals:
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void ReagentGroupUpdated();
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void DashboardStationsUpdated();
   	void ReagentColorGrioupUpdated();
    void DashboardStationChangeReagent();
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void ProgramsUpdated();
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void ReagentsUpdated();
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void UserSettingsUpdated();
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam StationID = Station id
     */
    /****************************************************************************/
    void UpdateMode(const QString &);
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void DateTimeAcked();
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam StationID = Station id
     */
    /****************************************************************************/
    void RemoveProgram(QString &);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam EventsStringsData = Events data
     */
    /****************************************************************************/
    void UpdateEventStrings(QByteArray EventsStringsData);
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void StopTimers();
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void ReCalculateEndTimes();
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam StationID = Station id
     */
    /****************************************************************************/
    void ShowManualProgramDlg(QString);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam AuthenticatedLevel = AuthenticatedLevel
     */
    /****************************************************************************/
    void UserAuthentication(const qint32 &AuthenticatedLevel);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam IsAuthenticated = Authenticated
     */
    /****************************************************************************/
    void ChangeAdminPassword(const QString &IsAuthenticated);

    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam FileNames = Dailyrunlog file names
     */
    /****************************************************************************/
    void UpdateDayRunLogFileNames(const QStringList &FileNames);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam DataStream = Dailyrunlog data stream
     */
    /****************************************************************************/
    void UpdateDayRunLogFileContent(const QDataStream &DataStream);
    /****************************************************************************/
    /*!
     *  \brief
     *  \iparam IsBlgChangesDlg = Bathlayout changes dialog display flag
     */
    /****************************************************************************/
    void ShowPrioritizeScreenDlg(bool IsBlgChangesDlg);
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void DeviceConfigurationUpdated();
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void RevertChangedUserSettings();
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void RevertChangedProgram();
    /****************************************************************************/
    /*!
     *  \brief
     */
    /****************************************************************************/
    void RevertChangedReagent();

private:
    void ConfFileHandler(Global::tRefType Ref, const NetCommands::CmdConfigurationFile &Command);
    void ProcessStateHandler(Global::tRefType ref, const NetCommands::CmdProcessState &Command);
    void UpdateProgramHandler(Global::tRefType Ref, const MsgClasses::CmdProgramUpdate &Command);
    void AddNewProgramHandler(Global::tRefType Ref, const MsgClasses::CmdNewProgram &Command);
    void ProgramRemoveHandler(Global::tRefType Ref, const MsgClasses::CmdDeleteItem &Command);
    void AddNewReagentHandler(Global::tRefType Ref, const MsgClasses::CmdReagentAdd &Command);
    void UpdateReagentHandler(Global::tRefType Ref, const MsgClasses::CmdReagentUpdate &Command);
    void UpdateReagentGroupHandler(Global::tRefType Ref, const MsgClasses::CmdReagentGroupUpdate &Command);
    void UpdateStationChangeReagentHandler(Global::tRefType Ref, const MsgClasses::CmdStationChangeReagent &Command);
    void UpdateStationResetData(Global::tRefType Ref, const MsgClasses::CmdStationResetData &Command);
    void UpdateStationSetAsEmpty(Global::tRefType Ref, const MsgClasses::CmdStationSetAsEmpty &Command);
    void UpdateStationSetAsFull(Global::tRefType Ref, const MsgClasses::CmdStationSetAsFull &Command);
    void UpdateStationReagentStatus(Global::tRefType Ref, const MsgClasses::CmdUpdateStationReagentStatus &Command);

    void EventStringHandler(Global::tRefType ref, const NetCommands::CmdEventStrings &Command);
    void ReagentRemoveHandler(Global::tRefType Ref, const MsgClasses::CmdReagentRemove &Command);
    void ExecutionStateHandler(Global::tRefType ref, const NetCommands::CmdExecutionStateChanged &Command);
    void LanguageFileHandler(Global::tRefType Ref, const NetCommands::CmdLanguageFile &Command);
    void SettingsUpdateHandler(Global::tRefType Ref, const MsgClasses::CmdChangeUserSettings &Command);
    void EventReportHandler(Global::tRefType Ref, const NetCommands::CmdEventReport &Command);
    void UserAuthenticationHandler(Global::tRefType Ref, const NetCommands::CmdChangeUserLevelReply &Command);
    void AdminPasswordChangeHandler(Global::tRefType Ref, const NetCommands::CmdChangeAdminPasswordReply &Command);
    void DayRunLogReplyFileHandler(Global::tRefType Ref, const NetCommands::CmdDayRunLogReplyFile &Command);
    void DayRunLogReplyHandler(Global::tRefType Ref, const NetCommands::CmdDayRunLogReply &Command);
    void OnCancelMessageBoxDisplay();
    void OnCmdGuiInit(Global::tRefType Ref, const NetCommands::CmdGuiInit &Command);
    void ShowMessageDialog(Global::GUIMessageType MessageType, QString MessageText);

    NetLayer::CMasterLinkDevice m_NetworkObject;    //!< The network layer
    MainMenu::CMainWindow *mp_MainWindow;           //!< Main window of the GUI
    MainMenu::CMessageDlg *mp_MessageDlg;           //!< Error message box
    MainMenu::CMessageDlg *mp_RmsMessageDlg;        //!< Error message box
    MainMenu::CWaitDialog *mp_WaitDialog;           //!< Displayed when busy
    MainMenu::CWaitDialog *mp_BlgScanWaitDialog;    //!< Displayed when busy
    QFile *mp_LanguageFile;                         //!< Lanugage file object (qm file)
    QFile *mp_OldFile;                              //!< Old/Previous language file(qm file)
    QTranslator m_Translator;                       //!< Language translator
    bool m_UpdateProgramColor;
    bool m_AddProgramWithUpdateColor;
    quint32 m_LanguageChangeCount;                  //!< Count of language changes
    QHash<quint32, ScheduledStations_t> m_ScheduledStationsHash;    //!< Scheduled Stations hash
    MainMenu::CMsgBoxManager *mp_MesgBoxManager;              //!< Msg Box manager for CmdEventReport
    LeicaConsumableType_t m_ConsumableType;         ///< Leica Consumable type
    quint32 m_BottleCount;                          ///< Leica Bottle count
    bool m_GuiAllConfigurationReceived;             //!< True -if all configuration data (XML)from Main is received
    bool m_GuiInit;                                 //!< True - if Gui is getting initialized
    int m_CurrentTabIndex;
private slots:
    void OnMasterConnected(const QString &name);
    void OnMasterDisconnected(const QString &name);
    void OnCmdTimeout(Global::tRefType Ref, const QString &CommandName);
    void SetDateTime(QDateTime DateTime);
    void RetranslateUI();
    void OnEventReportAck(NetCommands::ClickedButton_t ClickedButton, Global::tRefType CmdRef, quint64 EventKey);

};

} // end namespace Core

#endif // CORE_DATACONNECTOR_H
