/****************************************************************************/
/*! \file TestHimalayaMasterThread/Source/TestHimalayaMasterThreadController.cpp
 *
 *  \brief Implementation file for class TestHimalayaMasterThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-02-21
 *  $Author:    $ BhanuPrasad Hulsoor
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

#include <TestHimalayaMasterThread/Include/TestHimalayaMasterThreadController.h>
#include <HimalayaGuiController/Include/HimalayaGuiController.h>
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>
#include <../Include/DataLoggingSources.h>
#include <QFileDialog>

#include <NetCommands/Include/CmdConfigurationFile.h>
#include <NetCommands/Include/CmdProcessState.h>
#include <NetCommands/Include/CmdLanguageFile.h>
#include <NetCommands/Include/CmdEventReport.h>
#include <Global/Include/Commands/CmdDateAndTime.h>
#include <NetCommands/Include/CmdEventStrings.h>
#include "Global/Include/GlobalDefines.h"
#include "CommandExecuter/Include/CommandExecuter.h"
#include "HimalayaDataContainer/Containers/StainingReagents/Include/DataReagentList.h"
#include <NetCommands/Include/CmdChangeUserLevelReply.h>
#include <QXmlStreamReader>
#include <Global/Include/Translator.h>
#include <Global/Include/UITranslator.h>

using namespace DataManager;

namespace Himalaya {
//unit of measurement is Seconds for below mentioned macros.
#define RACK_TRANSFER_TIME          10
#define EXECUTE_NOW                 0
#define UNLOADING_STATION_DURATION  10
#define LOADING_STATION_DURATION    10
#define TRANSFER_STATION_DURATION   10
//#define MAXIMUM_STATIONS            42
#define START_STATION_NUMBER        10

QHash <int, int> g_EventIdEventTypeHash;
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 */
/****************************************************************************/
HimalayaMasterThreadController::HimalayaMasterThreadController() :
    MasterThreadController(HEARTBEAT_SOURCE_MASTERTHREAD, HEARTBEAT_SOURCE_DATALOGGING,
                           HEARTBEAT_SOURCE_EVENTHANDLER,"HimalayaShutdown"),
    m_CommandChannelGui(this,"GUI"),
    m_CommandChannelScheduler(this,"SCHEDULER"),
    m_CommandChanngelDevice(this, "Device"),
    mp_DataManager(NULL),
    m_ProgramStartableManager(this)
{
    //ReadEventTranslations(QLocale::English, QLocale::English);
    mp_ByteArray = NULL;
    mp_CommandExecuter = new CommandExecuter::CommandExecuter(this);
    m_ChangeLanguage =false;

    RegisterCommands();
    RegisterDeviceCommands();
    RegisterSchedulerCommands();    
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
HimalayaMasterThreadController::~HimalayaMasterThreadController() {

    delete mp_ByteArray;
    delete mp_CmdExecutionStateChanged;
}

/****************************************************************************/
void HimalayaMasterThreadController::CreateAndInitializeObjects() {
    mp_DataManager = new DataManager::CDataManager(this);
    //initialize the DataManagerBase pointer in MasterThread
    //initialize the DataManagerBase pointer in MasterThread
    mp_DataManagerBase = mp_DataManager;
    //Create RMS manager
    MasterThreadController::CreateAndInitializeObjects();
    m_ProgramStartableManager.Init();
}

/****************************************************************************/
/*!
 *  \brief CreateControllersAndThreads :  Creation of Gui Controller component
*/
/****************************************************************************/
void HimalayaMasterThreadController::CreateControllersAndThreads() {
    // let base class create controllers and threads
    MasterThreadController::CreateControllersAndThreads();

    // create and connect gui controller
    AddAndConnectController(new HimalayaGui::HimalayaGuiController(HEARTBEAT_SOURCE_GUI),
                            &m_CommandChannelGui, static_cast<int>(HIMALAYA_GUI_THREAD));
    CHECKPTR(mp_DataManager);
    CHECKPTR(mp_DataManager->GetDeviceConfigurationInterface());
    DataManager::CDeviceConfiguration *p_DeviceConfiguration = mp_DataManager->GetDeviceConfigurationInterface()->GetDeviceConfiguration();
    CHECKPTR(p_DeviceConfiguration);
}

/****************************************************************************/
void HimalayaMasterThreadController::OnPowerFail() {
}

void HimalayaMasterThreadController::OnGoReceived() {
    CreateAndInitializeObjects();
    CreateControllersAndThreads();
    MasterThreadController::OnGoReceived();
    //Global::EventObject::Instance().RaiseEvent(EVENT_HIMALAYA_START, Global::FmtArgs() << mp_DataManager->GetDeviceConfigurationInterface()->GetDeviceConfiguration()->GetValue("SerialNumber"), true);
}

/****************************************************************************/
void HimalayaMasterThreadController::InitiateShutdown() {
}

/****************************************************************************/
void HimalayaMasterThreadController::RegisterDeviceCommands() {
//    RegisterCommandForProcessing<DeviceCommandProcessor::CmdLoaderUpdate, HimalayaMasterThreadController>
//            (&HimalayaMasterThreadController::LoaderUpdateHandler, this);
}

/****************************************************************************/
void HimalayaMasterThreadController::RegisterSchedulerCommands() {

}

/****************************************************************************/
void HimalayaMasterThreadController::RegisterCommands() {

    RegisterCommandForProcessing<NetCommands::CmdChangeUserLevel, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::ChangeUserLevelHandler, this);
    RegisterCommandForProcessing<Global::CmdDateAndTime, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::SetDateTime, this);
    RegisterCommandForProcessing<MsgClasses::CmdDataExport, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::ExportDataFileHandler, this);
    RegisterCommandForProcessing<MsgClasses::CmdDataImport, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::ImportDataFileHandler, this);

    RegisterAcknowledgeForProcessing<Global::AckOKNOK, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::OnAckOKNOK, this);
    RegisterAcknowledgeForProcessing<NetCommands::CmdAcknEventReport, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::OnEventReportAck, this);    
    RegisterCommandForProcessing<NetCommands::CmdGuiInit, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::OnCmdGuiInitHandler, this);

#if 0
    RegisterCommandForProcessing<Global::CmdDateAndTime, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::SetDateTime, this);
    RegisterCommandForProcessing<MsgClasses::CmdReagentUpdate, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::UpdateReagent, this);

    RegisterAcknowledgeForProcessing<Global::AckOKNOK, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::OnAckOKNOK, this);
    RegisterCommandForProcessing<MsgClasses::CmdChangeUserSettings, HimalayaMasterThreadController>
            (&HimalayaMasterThreadController::UpdateSettings, this);
#endif
}

///****************************************************************************/
//void HimalayaMasterThreadController::RMSForecastHandler(Global::tRefType Ref, const MsgClasses::CmdUpdateRMSForecast &Cmd, Threads::CommandChannel &AckCommandChannel) {
//    ListOfForecastValues_t ForecastValues;
//    QDataStream ForecastDataStream(&const_cast<QByteArray &>(Cmd.GetRMSForecastData()), QIODevice::ReadWrite);
//    ForecastDataStream >> ForecastValues;
//    if (!(mp_DataManager->SetRMSForecast(ForecastValues))) {
//            SendAcknowledgeNOK(Ref, AckCommandChannel);
//    }
//    else {
//        SendAcknowledgeOK(Ref, AckCommandChannel);
//        QByteArray  StationData;
//        QDataStream StationDataStream(&StationData, QIODevice::ReadWrite);
//        DataManager::CDataStationList *p_StationList = mp_DataManager->GetStationList();
//        //Stream in station data
//        StationDataStream << *p_StationList;
//        SendCommand(Global::CommandShPtr_t(new MsgClasses::CmdStationUpdate(1000, StationDataStream)), m_CommandChannelGui);
//    }
//}



/****************************************************************************/
void HimalayaMasterThreadController::SetDateTime(Global::tRefType Ref, const Global::CmdDateAndTime &Cmd, Threads::CommandChannel &AckCommandChannel) {
    // extract new date and time
    QDateTime NewDateTime = Cmd.GetDateTime();
    if(!MasterThreadController::SetAdjustedDateTimeOffset(NewDateTime)) {
        // send ACK
        SendAcknowledgeOK(Ref, AckCommandChannel);
        // send DataChanged signal
        /// \todo implement
    } else {
        // send NACK with according error message
        /// \todo implement message stuff
        SendAcknowledgeNOK(Ref, AckCommandChannel);
    }
}



/****************************************************************************/
/*!
 *  \brief Sends The Configuration xml files to the GUI.
 *\iparam   TextBrowser =  Text Browser Reference
 * \return true if the excecution succeeded, false otherwise
 *
 */
/****************************************************************************/
bool HimalayaMasterThreadController::SendXML(QTextBrowser &TextBrowser)
{
    QByteArray *p_ByteArray = new QByteArray();
    p_ByteArray->clear();
    QDataStream XmlStream(p_ByteArray, QIODevice::ReadWrite);
    XmlStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));

    DataManager::CDashboardDataStationList *p_DashboardStationList = mp_DataManager->GetDashboardStationList();
    if (p_DashboardStationList) {
        XmlStream << *p_DashboardStationList;
        XmlStream.device()->reset();
        SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::STATION , XmlStream));
    }
    TextBrowser.setTextColor(QColor(0,0,255,255));
    XmlStream.device()->reset();
    qDebug()<< "Data stream"<<XmlStream.device()->readAll();
    TextBrowser.append("CmdConfigurationFile ---  Sending DashboardStation XML file");
    p_ByteArray->clear();
    XmlStream.device()->reset();

    // always first send the reagent group xml file then reagents xml file
    DataManager::CDataReagentGroupList *p_ReagentGroupList = mp_DataManager->GetReagentGroupList();
    if (p_ReagentGroupList) {
        XmlStream << *p_ReagentGroupList;
        XmlStream.device()->reset();
        SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::REAGENTGROUP , XmlStream));
    }
    TextBrowser.setTextColor(QColor(0,0,255,255));
    XmlStream.device()->reset();
    qDebug()<< "Data stream"<<XmlStream.device()->readAll();
    TextBrowser.append("CmdConfigurationFile ---  Sending ReagentGroup XML files");
    p_ByteArray->clear();
    XmlStream.device()->reset();


    DataManager::CDataReagentList *p_ReagentList = mp_DataManager->GetReagentList();
    if (p_ReagentList) {
        XmlStream << *p_ReagentList;
        XmlStream.device()->reset();
        SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::REAGENT , XmlStream));
    }
    TextBrowser.setTextColor(QColor(0,0,255,255));
    XmlStream.device()->reset();
    qDebug()<< "Data stream"<<XmlStream.device()->readAll();
    TextBrowser.append("CmdConfigurationFile ---  Sending Reagent XML files");
    p_ByteArray->clear();
    XmlStream.device()->reset();

    //Update Supported Languages
    (void)UpdateSupportedGUILanguages();
    DataManager::CUserSettingsInterface *p_SettingsInterface = mp_DataManager->GetUserSettingsInterface();
    if (p_SettingsInterface) {
        XmlStream << *p_SettingsInterface;
        SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::USER_SETTING, XmlStream));
    }
    if (p_SettingsInterface) {
        if (p_SettingsInterface->GetUserSettings(false) != NULL) {
            // get the current language
            QLocale::Language CurrentLanguage =
                p_SettingsInterface->GetUserSettings(false)->GetLanguage();

            // store the langauge name de_DE
            QString LanguageName(QLocale(CurrentLanguage).name());
            // remove the DE
            LanguageName.truncate(LanguageName.lastIndexOf('_'));
            qDebug() << "Language Name ############################" << LanguageName << "#########################";

            QString LanguageFileName = "Himalaya_" + LanguageName + ".qm";
            (void) SendLanguageFileToGUI(LanguageFileName);
        }
    }
    TextBrowser.setTextColor(QColor(0,0,255,255));
    TextBrowser.append("CmdConfigurationFile ---  Sending User Settings XML files");
    XmlStream.device()->reset();
    p_ByteArray->clear();
    XmlStream.device()->reset();
    DataManager::CDataProgramList *p_ProgramList = mp_DataManager->GetProgramList();
    if (p_ProgramList) {
        XmlStream << *p_ProgramList;
        SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::PROGRAM , XmlStream));
    }
    XmlStream.device()->reset();
    TextBrowser.setTextColor(QColor(0,0,255,255));
    TextBrowser.append("CmdConfigurationFile ---  Sending Program XML file");
    p_ByteArray->clear();
    XmlStream.device()->reset();

    DataManager::CDeviceConfigurationInterface *p_DeviceConfigInterface = mp_DataManager->GetDeviceConfigurationInterface();
    if (p_DeviceConfigInterface) {
        XmlStream << *p_DeviceConfigInterface;
        XmlStream.device()->reset();
        qDebug()<<"Device configuration filessss"<<XmlStream.device()->readAll();
        XmlStream.device()->reset();
        SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::DEVICE_CONFIGURATION, XmlStream));
    }
    TextBrowser.setTextColor(QColor(0,0,255,255));
    XmlStream.device()->reset();
    TextBrowser.append("CmdConfigurationFile ---  Sending Device configuration XML files");
    XmlStream.device()->reset();

    // send language file to GUI
    //(void)SendLanguageFileToGUI("Himalaya_en.qm");
    //Send all other containers too
    QByteArray EventData;
    QDataStream EventDataStream(&EventData, QIODevice::ReadWrite);
    EventDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    Global::tTranslations TempTranslations;
    TempTranslations = Global::UITranslator::TranslatorInstance().GetTranslations();
    EventDataStream << TempTranslations;
    EventDataStream.device()->reset();
    SendCommand(Global::CommandShPtr_t(new NetCommands::CmdEventStrings(5000, EventDataStream)), m_CommandChannelGui);

    qDebug() << "HimalayaMasterThreadController::SendXML called";
    return true;
}


/****************************************************************************/
/**
 * \brief Acknowledge of type AckOKNOK received.
 *
 *\iparam   Ref     Command reference.
 *\iparam   Ack     Acknowledge.
 */
/****************************************************************************/
void HimalayaMasterThreadController::OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack)
{

    qDebug() << "HimalayaMasterThreadController::OnAckOKNOK: recieved acknowledge for Ref: " << Ref;
    qDebug() << "    Status: " << Ack.GetStatus() << "  Text: " << Ack.GetText() << Ack.GetName() << Ack.GetType();

    //checking the ref of the received ack
    m_Ref = Ref;
    qDebug() << "Ref" << m_Ref;
    if(RefList.contains(Ref) && Ack.GetStatus() == true)
    {
        QString Msg = "Acknowldgement recevied with Ref no  " +  QString::number(Ref);
        emit SendAckToTextBrowser(Msg);
    }
    else
    {
        qDebug() << "Reference not found " ;
    }
    if(Ack.GetStatus() != true)
    {
        qDebug() << "Negative Ack";
        QString Msg = "Negative Acknowldgement recevied for Ref no  " +  QString::number(Ref);
        emit SendAckToTextBrowser(Msg);
        m_Ref = 0;
    }    
}

/****************************************************************************/
/**
 * \brief Handler for Event report ACK.
 *
 *\iparam  Ref -- unique command reference.
 *\iparam  Ack -- received acknowledge instance.
 */
/****************************************************************************/
void HimalayaMasterThreadController::OnEventReportAck(Global::tRefType Ref, const NetCommands::CmdAcknEventReport &Ack)
{
    qDebug() << "HimalayaMasterThreadController::OnEventReportAck: recieved acknowledge for Ref: " << Ref << Ack.GetButtonClicked() << Ack.GetEventKey();

    //checking the ref of the received ack
    m_Ref = Ref;
    qDebug() << "Ref" << m_Ref;
    NetCommands::ClickedButton_t Button = Ack.GetButtonClicked();
    QString ButtonText;
    switch (Button) {
        case 0:
            ButtonText = ", Ok button clicked";
            break;
        case 1:
            ButtonText = ", Yes button clicked";
            break;
        case 2:
            ButtonText = ", Continue button clicked";
            break;
        case 3:
            ButtonText = ", NO button clicked";
            break;
        case 4:
            ButtonText = ", Cancel button clicked";
            break;
        case 5:
            ButtonText = ", Stop button clicked";
            break;
        default:
            break;
    }
    qDebug() << "Button text is" << ButtonText;
    if(RefList.contains(Ref))
    {
        QString Msg = "Ack recevied Ref no " +  QString::number(Ref) + ",  With Event Id " + QString::number(Ack.GetEventKey()) + ButtonText ;
        emit SendAckToTextBrowser(Msg);
    }
    else
    {
        qDebug() << "Reference not found " ;
    }
}

/****************************************************************************/
/*!
 *  \brief Send Global::Command type messages to GUI
 *  \iparam p_Command = command to send
 */
/****************************************************************************/
void HimalayaMasterThreadController::SendMessage(Global::Command *p_Command)
{
    Global::tRefType CMDRef;
    QString Str = p_Command->GetName();
    CMDRef = SendCommand(Global::CommandShPtr_t(p_Command), m_CommandChannelGui);
    RefList << CMDRef;
    if (!RefList.empty()) {
        QString Msg = "Sending the command " + Str + " with ref no  " +  QString::number(CMDRef);
        emit SendAckToTextBrowser(Msg);
    }
}

/*********************************************************************************/
/*!
 *  \brief Sending the Process state command to GUI(Starting the Staining Process)
 */
/*********************************************************************************/
void HimalayaMasterThreadController::SendProcessStart()
{
    CommandExecuter::CommandExecuter *p_CommandExecuter = new CommandExecuter::CommandExecuter(this);
    bool ProcessRunning = true;
    p_CommandExecuter->AddCommandToQueue(new NetCommands::CmdProcessState(1000, ProcessRunning), 0);
}


/*********************************************************************************/
/*!
 *  \brief Sending the Process state command to GUI(Stopping the Staining Process)
 */
/*********************************************************************************/
void HimalayaMasterThreadController::SendProcessStop()
{
    CommandExecuter::CommandExecuter *p_CommandExecuter = new CommandExecuter::CommandExecuter(this);
    bool ProcessRunning = false;
    p_CommandExecuter->AddCommandToQueue(new NetCommands::CmdProcessState(1000, ProcessRunning), 0);
}

/****************************************************************************/
/*!
 *  \brief Clears the memory created in heap for test sequences.
 */
/****************************************************************************/
void HimalayaMasterThreadController::ClearHeap()
{
    delete mp_ByteArray;
}

/****************************************************************************/
/*!
 *  \brief Sends the language file to the GUI
 *\iparam   TextBrowser =  Text Browser Reference.
 * \return true if the sequence excecution succeeded, false otherwise
 *
 */
/****************************************************************************/
bool HimalayaMasterThreadController::SendChangeLanguage(QTextBrowser &TextBrowser)
{
    CommandExecuter::CommandExecuter *p_CommandExecuter = new CommandExecuter::CommandExecuter(this);
    if (!m_ChangeLanguage) {
        //The hardcoded filepath will be replaced with new path later
        QString TranslationPath = Global::SystemPaths::Instance().GetTranslationsPath() + "/Himalaya_de.qm";
        QFile File(TranslationPath);
        File.open(QIODevice::ReadWrite);
        QDataStream LangDataStream(&File);
        LangDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        p_CommandExecuter->AddCommandToQueue(new NetCommands::CmdLanguageFile(5000, LangDataStream), 0);
        TextBrowser.setTextColor(QColor(0,0,0,255));
        TextBrowser.append("CmdLanguageFile ");
        LangDataStream.device()->reset();
        TextBrowser.setTextColor(QColor(0,0,255,255));
        TextBrowser.append(LangDataStream.device()->readAll());
        LangDataStream.device()->reset();
        m_ChangeLanguage =true;
        return true;
    }
    else {
        QString TranslationPath = Global::SystemPaths::Instance().GetTranslationsPath() + "/Himalaya_en.qm";
        QFile File(TranslationPath);
        File.open(QIODevice::ReadWrite);
        QDataStream LangDataStream(&File);
        LangDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        p_CommandExecuter->AddCommandToQueue(new NetCommands::CmdLanguageFile(5000, LangDataStream), 0);
        TextBrowser.setTextColor(QColor(0,0,0,255));
        TextBrowser.append("CmdLanguageFile ");
        LangDataStream.device()->reset();
        TextBrowser.setTextColor(QColor(0,0,255,255));
        TextBrowser.append(LangDataStream.device()->readAll());
        LangDataStream.device()->reset();
        m_ChangeLanguage =false;
        return true;
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief Set the new date and time and send it to the GUI
 *\iparam   DateTime = modified Date and time
 * \return true if the sequence excecution succeeded, false otherwise
 *
 */
/****************************************************************************/
bool HimalayaMasterThreadController::SetDateAndTime(QDateTime &DateTime)
{
    CommandExecuter::CommandExecuter *p_CommandExecuter = new CommandExecuter::CommandExecuter(this);
    Global::CmdDateAndTime *cmd = new Global::CmdDateAndTime(500, DateTime);
    qDebug() << " in SetDateAndTime func dateandtime is " << cmd->GetDateTime().date();
    SendMessage(cmd);
    return true;
}


/****************************************************************************/
/*!
 *  \brief SendEventMessage : Sending different event messages to the GUI
 *\iparam   Type = Event Message type.
 *\iparam   ButtonType = Event message Button type
 *\iparam   StatusBarIcon = Display in statusbar yes/no
 *\iparam   EventString = Event message string
 * \return true if the sequence excecution succeeded, false otherwise
*/
/****************************************************************************/
bool HimalayaMasterThreadController::SendEventMessage(int Type, Global::GuiButtonType ButtonType, bool StatusBarIcon,QString EventString)
{
    static quint64 EventId = 33619980;
    static quint32 Count = 1;
    qDebug() << "Event Id value before shifting is " << EventId;
    Count++;
    EventId  = (EventId << 32)+ Count;
    qDebug()<< "Event ID Sent" << EventId;
    NetCommands::EventReportDataStruct EventReportData;
    switch(Type) {
        case 2:
            EventReportData.EventStatus = true;
            EventReportData.EventType = Global::EVTTYPE_INFO;
            EventReportData.ID = EventId;
            if (EventString == "") {
                EventReportData.MsgString = QString("DI water not available.Program(s) using DI water step will no longer be startable");
            }
            else {
                EventReportData.MsgString = EventString;
            }
            EventReportData.Time = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
            EventReportData.BtnType = ButtonType;
            EventReportData.StatusBarIcon = StatusBarIcon;
            SendMessage(new NetCommands::CmdEventReport(1000, EventReportData));
            qDebug()<< "in NetCommands::INFORMATION \n";
            break;
        case 3:
            EventReportData.EventStatus = true;
            EventReportData.EventType = Global::EVTTYPE_WARNING;
            EventReportData.ID = EventId;
            if (EventString == "") {
                EventReportData.MsgString = QString("Skipping slide counting as oven cover is open");
            }
            else {
                EventReportData.MsgString = EventString;
            }
            EventReportData.Time = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
            EventReportData.BtnType = ButtonType;
            EventReportData.StatusBarIcon = StatusBarIcon;
            SendMessage(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData));
            qDebug()<< "in NetCommands::WARNING \n";
            break;
        case 4:
            EventReportData.EventStatus = true;
            EventReportData.EventType = Global::EVTTYPE_ERROR;
            EventReportData.ID = EventId;
            if (EventString == "") {
                EventReportData.MsgString = QString("Please remove all racks from device and press Ok when finished");
            }
            else {
                EventReportData.MsgString = EventString;
            }
            EventReportData.Time = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
            EventReportData.BtnType = ButtonType;
            EventReportData.StatusBarIcon = StatusBarIcon;
            SendMessage(new NetCommands::CmdEventReport(Global::Command::MAXTIMEOUT, EventReportData));
            break;
        default:
            break;
    }
    g_EventIdEventTypeHash.insert(EventId, Type);
    qDebug()<< "Type"<<Type;
    qDebug()<<"EVent Key" <<EventId;
    EventId = ((EventId & 0xffffffff00000000) >> 32) - Count;
    EventId++;
    return true;
}

/****************************************************************************/
/*!
 *  \brief SendRemoveMessageBox : Removing event messages from the Message queue
 *\iparam   EventID = Event Id of the message to be removed
 * \return true if the sequence excecution succeeded, false otherwise
*/
/****************************************************************************/
bool HimalayaMasterThreadController::SendRemoveMessageBox(quint64 EventID)
{
    Global::EventType Type = static_cast<Global::EventType>(g_EventIdEventTypeHash.value(EventID));
    NetCommands::EventReportDataStruct EventReportData;
    //By changing the Status as "False", removing the message from Event message queue
    EventReportData.EventStatus = false;
    EventReportData.EventType = Type;
    EventReportData.ID = EventID;
    EventReportData.MsgString = QString("Error EventID: %1").arg(EventID);
    EventReportData.Time = Global::AdjustedTime::Instance().GetCurrentDateTime().toString();
    EventReportData.BtnType = Global::OK;
    EventReportData.StatusBarIcon = true;
    SendMessage(new NetCommands::CmdEventReport(1000, EventReportData));
    return true;
}


/************************************************************************************/
/*!
 *  \brief Change User level Handler
 *  \iparam Ref = Command reference
 *  \iparam Cmd = The command
 *  \iparam AckCommandChannel = command channel
*/
/************************************************************************************/
void HimalayaMasterThreadController::ChangeUserLevelHandler(Global::tRefType Ref, const NetCommands::CmdChangeUserLevel &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    qDebug() << "Cmd.GetPassword" << Cmd.GetPassword();
    //Checking the password for "Himalaya"
    if (Cmd.GetPassword() == "Himalaya") {
        SendAcknowledgeOK(Ref, AckCommandChannel);
        Global::GuiUserLevel UserLevel = Cmd.GetUserLevel();
        //Sending the Chnage userlevel reply command to change the user level
        SendMessage(new NetCommands::CmdChangeUserLevelReply(1000,UserLevel));
    }
    else {
        //If the password is not matching with "himalaya" sending Negative ack
        SendAcknowledgeNOK(Ref, AckCommandChannel,"Incorrect Password");
    }
}

/****************************************************************************/
/**
 * \brief Handler for CmdDataExport.
 *
 * \iparam Ref - Refernce of the command argument
 * \iparam Cmd - Command class
 * \iparam AckCommandChannel - Channel class for the command
 *
 */
/****************************************************************************/
void HimalayaMasterThreadController::ExportDataFileHandler(Global::tRefType Ref, const MsgClasses::CmdDataExport &Cmd, Threads::CommandChannel &AckCommandChannel)
{
    //Sending Every time Ack status as TRUE for Export
    sleep(5);
    SendAcknowledgeOK(Ref, AckCommandChannel);
}


/****************************************************************************/
/**
 * \brief Handler for CmdDataImport.
 *
 * \iparam Ref - Refernce of the command argument
 * \iparam Cmd - Command class
 * \iparam AckCommandChannel - Channel class for the command
 *
 */
/****************************************************************************/
void HimalayaMasterThreadController::ImportDataFileHandler(Global::tRefType Ref, const MsgClasses::CmdDataImport &Cmd,Threads::CommandChannel &AckCommandChannel)
{
    QString FilePath = Global::SystemPaths::Instance().GetTempPath() + "/ImportConfig.xml";
    QString ImportType = GetImportType(FilePath);
    //If the Config import is selcted in Configuration file
    if (ImportType == "ConfigImport") {
        bool ConfigImport = ConfigFilesImport();
        if (ConfigImport == true) {
            sleep(5);
            SendAcknowledgeOK(Ref, AckCommandChannel);
        }
        else {
            SendAcknowledgeNOK(Ref, AckCommandChannel);
        }
    }
    //If the Language import is selcted in Configuration file
    if (ImportType == "LangImport") {
        //When we select language import sending the german file every time
        QString TranslationPath = Global::SystemPaths::Instance().GetTempPath() + "/Language/Himalaya_de.qm";
        QFile File(TranslationPath);
        File.open(QIODevice::ReadWrite);
        QDataStream LangDataStream(&File);
        LangDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        sleep(5);
        SendAcknowledgeOK(Ref, AckCommandChannel);
        SendMessage(new NetCommands::CmdLanguageFile(5000, LangDataStream));
        m_ChangeLanguage =true;
        return;
    }
    //If the LeicaProgImport import is selcted in Configuration file
    if (ImportType == "LeicaProgImport") {
        bool LeicaImport = LeicaProgImport();
        if (LeicaImport == true) {
            sleep(5);
            SendAcknowledgeOK(Ref, AckCommandChannel);
        }
        else {
            SendAcknowledgeNOK(Ref, AckCommandChannel);
        }

    }

}

/****************************************************************************/
/**
 * \brief Get the type of the import mentioned in config file.
 *
 * \iparam FilePath - Path of the importConfig file
 * \return Type of the import
 */
/****************************************************************************/
QString HimalayaMasterThreadController::GetImportType(QString FilePath)
{
    QFile *xmlFile = new QFile(FilePath);
    qDebug() << "File path is " << FilePath;
    QString Error = "Error";
    if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        return Error;
    }
    QXmlStreamReader *xmlReader = new QXmlStreamReader(xmlFile);
    xmlReader->device()->reset();
    qDebug() << "xmlReader->device()->readAll();" << xmlReader->device()->readAll();
    xmlReader->device()->reset();
    //Parse the XML until we reach end of it
    while (!xmlReader->atEnd() && !xmlReader->hasError()) {
        // Read next element
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        //If token is just StartDocument - go to next
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }
        //If token is StartElement - read it
        if (token == QXmlStreamReader::StartElement) {
            if (xmlReader->name() == "Import_Type") {
                continue;
            }

            if (xmlReader->name() == "Type") {
                //qDebug() << "Type of the Element" << xmlReader->readElementText();
                QString ImportType;
                bool IntFlag;
                int Type = xmlReader->readElementText().toInt(&IntFlag,10);
                switch (Type) {
                    case 1:
                    ImportType = "ConfigImport";
                    break;
                    case 2:
                    ImportType = "LangImport";
                    break;
                    case 3:
                    ImportType = "LeicaProgImport";
                    break;
                }
                return ImportType;
            }
        }
    }

    if (xmlReader->hasError()) {
        qDebug() << "Errror" ;
        return Error;
    }

    //close reader and flush file
    xmlReader->clear();
    xmlFile->close();
}


/****************************************************************************/
/**
 * \brief Send the config files for import.
 *
 * \return true if success
 */
/****************************************************************************/
bool HimalayaMasterThreadController::ConfigFilesImport()
{
    QByteArray *p_ByteArray = new QByteArray();
    p_ByteArray->clear();
    QDataStream ReagentDataStream(p_ByteArray, QIODevice::ReadWrite);
    ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    //Importing the Reagent Configuration file
    QString ReagentsPath = Global::SystemPaths::Instance().GetTempPath() + "/Configuration/Reagents.xml";
    DataManager::CDataReagentList ReagentList;
    ReagentList.SetDataVerificationMode(false);
    ReagentList.Read(ReagentsPath);
    ReagentDataStream << ReagentList;
    SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::REAGENT , ReagentDataStream));
    p_ByteArray->clear();
    QDataStream SettingsDataStream(p_ByteArray, QIODevice::ReadWrite);
    SettingsDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    //Importing the Settings Configuration file
    QString SettingsPath = Global::SystemPaths::Instance().GetTempPath() + "/Configuration/UserSettings.xml";
    DataManager::CUserSettingsInterface UserSettings;
    UserSettings.SetDataVerificationMode(false);
    UserSettings.Read(SettingsPath);
    SettingsDataStream << UserSettings;
    SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::USER_SETTING , SettingsDataStream));
    p_ByteArray->clear();
    QString ProgramsPath = Global::SystemPaths::Instance().GetTempPath() + "/Configuration/Programs.xml";
    //Importing the Programs Configuration file
    QDataStream ProgDataStream(p_ByteArray, QIODevice::ReadWrite);
    ProgDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataManager::CDataProgramList ProgramList;
    ProgramList.SetDataVerificationMode(false);
    ProgramList.Read(ProgramsPath);
    ProgDataStream << ProgramList;
    SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::PROGRAM , ProgDataStream));
    p_ByteArray->clear();
//    QString StationsPath = Global::SystemPaths::Instance().GetTempPath() + "/Configuration/Stations.xml";
//    //Importing the Stations Configuration file
//    QDataStream StationDataStream(p_ByteArray, QIODevice::ReadWrite);
//    StationDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
//    DataManager::CDataStationList StationList;
//    StationList.SetDataVerificationMode(false);
//    StationList.Read(StationsPath);
//    StationDataStream << StationList;
//    SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::STATION , StationDataStream));
//    p_ByteArray->clear();
    return true;
}
/****************************************************************************/
/**
 * \brief Send the Leica programs/Reagent for import.
 *
 * \return true if success
 */
/****************************************************************************/
bool HimalayaMasterThreadController::LeicaProgImport()
{
    // get the Leica Programs
    QByteArray *p_ByteArray = new QByteArray();
    p_ByteArray->clear();
    DataManager::CDataProgramList *p_DataProgramList = new DataManager::CDataProgramList();
    //Retriving the original Program list
    p_DataProgramList = mp_DataManager->GetProgramList();
    DataManager::CProgramStep *p_ImportProgramStep2 = new DataManager::CProgramStep();
    DataManager::CProgramStep *p_ImportProgramStep = new DataManager::CProgramStep();
    DataManager::CProgramStep *p_ImportProgramStep3 = new DataManager::CProgramStep();
    //Creating a new Leica program to be imported
    DataManager::CProgram *p_ImportProgram = new DataManager::CProgram;
    p_ImportProgram->SetID("L32");
    p_ImportProgram->SetShortName("LIR");
    p_ImportProgram->SetLongName("Iron Stain (HE average)");
    p_ImportProgram->SetColor("white");
    p_ImportProgram->LockProgram();
    //Adding Program steps to the created program
    p_ImportProgramStep->SetStepID("0");
    p_ImportProgramStep->SetReagentID("L4");
    p_ImportProgramStep->SetExclusive(false);
    p_ImportProgramStep->SetIntensity(0);
    p_ImportProgramStep->SetMinDuration("5m");
    p_ImportProgramStep->SetMaxDurationInPercent("100%");    
    p_ImportProgram->AddProgramStep(p_ImportProgramStep);
    //Adding Program steps to the created program
    p_ImportProgramStep2->SetStepID("1");
    p_ImportProgramStep2->SetReagentID("L5");
    p_ImportProgramStep2->SetExclusive(false);
    p_ImportProgramStep2->SetIntensity(3);
    p_ImportProgramStep2->SetMinDuration("3s");
    p_ImportProgramStep2->SetMaxDurationInPercent("100%");
    p_ImportProgram->AddProgramStep(p_ImportProgramStep2);
    //Adding Program steps to the created program
    p_ImportProgramStep3->SetStepID("2");
    p_ImportProgramStep3->SetReagentID("S7");
    p_ImportProgramStep3->SetExclusive(false);
    p_ImportProgramStep3->SetIntensity(6);
    p_ImportProgramStep3->SetMinDuration("1s");
    p_ImportProgramStep3->SetMaxDurationInPercent("0%");
    p_ImportProgram->AddProgramStep(p_ImportProgramStep3);
    p_ImportProgram->SetNextFreeStepID("3");
    p_DataProgramList->AddLeicaProgram(p_ImportProgram);
    //Getting the Original Program seq list
    //Creating a program sequence and adding to the existing Program seq list
    QDataStream ProgDataStream(p_ByteArray, QIODevice::ReadWrite);
    ProgDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    p_DataProgramList->SetDataVerificationMode(false);
    ProgDataStream << *p_DataProgramList;
    SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::PROGRAM , ProgDataStream));
    p_ByteArray->clear();
    QDataStream ProgSeqDataStream(p_ByteArray, QIODevice::ReadWrite);
    ProgSeqDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::PROGRAM_SEQUENCE , ProgSeqDataStream));
    p_ByteArray->clear();
    DataManager::CDataReagentList *p_ReagentList = new DataManager::CDataReagentList();
    //Retriving the original Reagent List
    p_ReagentList = mp_DataManager->GetReagentList();
    //Creating a new Leica Reagent to be imported
    DataManager::CReagent *p_Reagent = new DataManager::CReagent();
    p_ReagentList->SetDataVerificationMode(false);
    p_ReagentList->AddReagent(p_Reagent);
    QDataStream ReagentDataStream(p_ByteArray, QIODevice::ReadWrite);
    ReagentDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    ReagentDataStream << *p_ReagentList;
    ReagentDataStream.device()->reset();
    qDebug() << "ProgDataStream.device()->readAll()"<<ReagentDataStream.device()->readAll();
    ReagentDataStream.device()->reset();
    SendMessage(new NetCommands::CmdConfigurationFile(5000, NetCommands::REAGENT , ReagentDataStream));
    p_ByteArray->clear();
    delete p_ByteArray;
    delete p_ImportProgramStep2;
    delete p_ImportProgramStep;
    delete p_ImportProgramStep3;
    delete p_DataProgramList;
    delete p_ReagentList;
    delete p_Reagent;
    return true;
}

/****************************************************************************/
/**
 * \brief Update the GUI language files (qm files from translation directory)
 */
/****************************************************************************/
bool HimalayaMasterThreadController::UpdateSupportedGUILanguages() {
    QDir TheDir(Global::SystemPaths::Instance().GetTranslationsPath());
        QStringList FileNames = TheDir.entryList(QStringList("Colorado_*.qm"));
        DataManager::CDeviceConfigurationInterface *p_DeviceConfigInterface = mp_DataManager->GetDeviceConfigurationInterface();
        if (p_DeviceConfigInterface) {
            DataManager::CDeviceConfiguration *p_DeviceConfiguration = p_DeviceConfigInterface->GetDeviceConfiguration();
            if (p_DeviceConfiguration) {
                QStringList LanguageList;
                for (int i = 0; i < FileNames.size(); ++i)
                {
                    // get locale extracted by filename
                    QString Locale;
                    Locale = FileNames[i];                  // "Colorado_de.qm"
                    Locale.truncate(Locale.lastIndexOf('.'));   // "Colorado_de"
                    Locale.remove(0, Locale.indexOf('_') + 1);   // "de"
                    LanguageList << QLocale::languageToString(QLocale(Locale).language());
                }
                p_DeviceConfiguration->SetLanguageList(LanguageList);
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
}

/****************************************************************************/
 /**
  * \brief Send the language file to GUI
  *
  * \param FileName - Name of the file
  *
  * \return On successful (True) or not (false)
  */
 /****************************************************************************/
bool HimalayaMasterThreadController::SendLanguageFileToGUI(QString FileName) {

    QString TranslationPath = Global::SystemPaths::Instance().GetTranslationsPath()
            + QDir::separator() + FileName;
    QFile File(TranslationPath);
    // check the existence of the file
    if (File.exists()) {
        // open the file
        if (File.open(QIODevice::ReadWrite)) {
           DataManager::CUserSettingsInterface *p_SettingsInterface = mp_DataManager->GetUserSettingsInterface();
            if (p_SettingsInterface) {
                const DataManager::CUserSettings *p_Settings = p_SettingsInterface->GetUserSettings();
                if (p_Settings) {
                    QLocale::Language CurrentLanguage =  p_Settings->GetLanguage();
                    QDataStream LangDataStream(&File);
                    LangDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
                    // send the command to GUI
                    SendCommand(Global::CommandShPtr_t(new NetCommands::CmdLanguageFile(5000, LangDataStream)),
                                m_CommandChannelGui);
                }
            }

            // change the UI translator default language also
            // get locale extracted by filename
            QString Locale;
            Locale = FileName;                  // "Himalaya_de.qm"
            Locale.truncate(Locale.lastIndexOf('.'));   // "Himalaya_de"
            Locale.remove(0, Locale.indexOf('_') + 1);   // "de"
            Global::UITranslator::TranslatorInstance().SetDefaultLanguage(QLocale(Locale).language());

        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief   This handler is called on reception of "CmdGuiInit", The remaining
             threads are created on reception of this command.
 *  \iparam Ref = Command reference
 *  \iparam Cmd = Command
 *  \AckCommandChannel = Command channel to send acknowledge
 */
/****************************************************************************/
void HimalayaMasterThreadController:: OnCmdGuiInitHandler(Global::tRefType Ref, const NetCommands::CmdGuiInit &Cmd, Threads::CommandChannel &AckCommandChannel)  {
    //Send Ack to GUI
    SendAcknowledgeOK(Ref, AckCommandChannel);
}

/****************************************************************************/
/**
 * \brief Start statemachine.
 *
 * Start own statemachine and start to work.
 *
 */
/****************************************************************************/
void HimalayaMasterThreadController::StartStatemachine()
{
    // start own state machine and start to work.

}

/****************************************************************************/
/**
 * \brief Send DataContainers to scheduler
 */
/****************************************************************************/
void HimalayaMasterThreadController::SendContainersToScheduler()
{
    // start own state machine and start to work.

}
} // end namespace Himalaya
