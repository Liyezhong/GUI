/****************************************************************************/
/*! \file DiagnosticsManufacturing/Source/RetortManufacturing.cpp
 *
 *  \brief CRetort class implementation
 *
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-05-30
 *   $Author:  $ Dixiong Li
 *
 *  \b Company:
 *
 *       Leica Biosystems Shanghai.
 *
 *  (C) Copyright 2010 by Leica Biosystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "DiagnosticsManufacturing/Include/RetortManufacturing.h"
#include "DiagnosticsManufacturing/Include/TestCaseReporter.h"
#include "Core/Include/SelectTestOptions.h"
#include "ui_RetortManufacturing.h"
#include <QDebug>
#include <QTableWidgetItem>
#include "Core/Include/ServiceDefines.h"
#include "Core/Include/CMessageString.h"
#include "ServiceDataManager/Include/TestCaseGuide.h"
#include "Main/Include/HimalayaServiceEventCodes.h"

//lint -e578

namespace DiagnosticsManufacturing {

const QString REGEXP_NUMERIC_VALIDATOR  = "^[0-9]{1,4}$"; //!< Reg expression for the validator
const int FIXED_LINEEDIT_WIDTH = 241;           ///< Fixed line edit width
const int SET_FIXED_TABLE_WIDTH = 500;          ///< Set table width
const int SET_FIXED_TABLE_HEIGHT = 280;         ///< Set table height
const int VISIBLE_TABLE_ROWS = 4;               ///< Visible table rows

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataConnector = Service GUI connector object
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CRetort::CRetort(Core::CServiceGUIConnector *p_DataConnector, MainMenu::CMainWindow *p_Parent)
    : mp_DataConnector(p_DataConnector)
    , mp_MainWindow(p_Parent)
    , mp_Ui(new Ui::CRetortManufacturing)
    , mp_Module(NULL)
    , m_RetortSNString("041/XXXX")
    , m_FinalTestResult("NA")
    , m_TestFlag(false)
{
    mp_Ui->setupUi(this);

    if (mp_DataConnector->GetModuleListContainer()) {
        mp_Module = mp_DataConnector->GetModuleListContainer()->GetModule("Retort");
    }
    if (mp_Module) {
        m_RetortSNString = mp_Module->GetSerialNumber();
    }
    mp_Ui->retortSNEdit->installEventFilter(this);
    mp_Ui->retortSNEdit->setFixedWidth(FIXED_LINEEDIT_WIDTH);

    mp_Ui->beginTestBtn->setEnabled(true);

    mp_TestReporter = new CTestCaseReporter("Retort", mp_DataConnector, mp_MainWindow);
    mp_MessageDlg   = new MainMenu::CMessageDlg(mp_MainWindow);
    mp_WaitDlg      = new MainMenu::CWaitDialog(mp_MainWindow);
    mp_WaitDlg->setModal(true);

    mp_TableWidget = new MainMenu::CBaseTable;

    mp_TableWidget->resize(SET_FIXED_TABLE_WIDTH, SET_FIXED_TABLE_HEIGHT);

    mp_TableWidget->horizontalHeader()->show();

    mp_Ui->retortSNEdit->setText(m_RetortSNString);

    mp_TableWidget->setModel(&m_Model);
    RetranslateUI();

    mp_TableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    mp_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mp_Ui->widget->setMinimumSize(mp_TableWidget->width(), mp_TableWidget->height());
    mp_Ui->widget->SetContent(mp_TableWidget);
    mp_Ui->sendTestReportBtn->setEnabled(false);
    mp_Ui->groupBox->setFixedWidth(551);

//    mp_Ui->testSuccessLabel->setPixmap(QPixmap(QString::fromUtf8(":/Large/CheckBoxLarge/CheckBox-enabled-large.png")));

    mp_KeyBoardWidget = new KeyBoard::CKeyBoard(KeyBoard::SIZE_1, KeyBoard::QWERTY_KEYBOARD);

    CONNECTSIGNALSLOTGUI(mp_Ui->beginTestBtn, clicked(), this, BeginTest());
    CONNECTSIGNALSLOTGUI(mp_Ui->sendTestReportBtn, clicked(), this, SendTestReport());
    CONNECTSIGNALSLOTGUI(mp_MainWindow, CurrentTabChanged(int), this, ResetTestStatus());
    CONNECTSIGNALSLOTGUI(this, UpdateModule(ServiceDataManager::CModule&), mp_DataConnector, SendModuleUpdate(ServiceDataManager::CModule&));
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CRetort::~CRetort()
{
     try {
        delete mp_KeyBoardWidget;
        delete mp_TableWidget;
        delete mp_Ui;
        delete mp_MessageDlg;
        delete mp_WaitDlg;
        delete mp_TestReporter;
     }
     catch (...) {
         // to please Lint
     }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CRetort::changeEvent(QEvent *p_Event)
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
 *  \brief  Method will be called,when an event triggered on the object
 *
 *  \iparam p_Object = object of the widget
 *  \iparam p_Event = Event object
 *
 *  \return true when event is triggered
 */
/****************************************************************************/
bool CRetort::eventFilter(QObject *p_Object, QEvent *p_Event)
{
    if (p_Object == mp_Ui->retortSNEdit && p_Event->type() == QEvent::MouseButtonPress)
    {
        ConnectKeyBoardSignalSlots();
        mp_KeyBoardWidget->setModal(true);
        mp_KeyBoardWidget->SetKeyBoardDialogTitle(Service::CMessageString::MSG_TITLE_ENTER_SERILA_NUMBER);
        mp_KeyBoardWidget->SetPasswordMode(false);
        mp_KeyBoardWidget->SetValidation(true);
        mp_KeyBoardWidget->SetMinCharLength(4);
        mp_KeyBoardWidget->SetMaxCharLength(4);
        mp_KeyBoardWidget->DisplayNumericKeyBoard();
        // ^ and $ is used for any character. * is used to enter multiple characters
        // [0-9] is used to allow user to enter only 0 to 9 digits
        mp_KeyBoardWidget->SetLineEditValidatorExpression(REGEXP_NUMERIC_VALIDATOR);
        mp_KeyBoardWidget->show();
        return true;
    }
    return false;
}

/****************************************************************************/
/*!
 *  \brief  To add data item to the table
 *  \iparam Index = Test case No.
 *  \iparam
 */
/****************************************************************************/
void CRetort::AddItem(quint8 Index, Service::ModuleTestCaseID_t Id)
{
    QList<QStandardItem *> itemList;

    QStandardItem *itemCheckFlag = new QStandardItem;
    if (Id == Service::RETORT_LEVEL_SENSOR_HEATING && Core::CSelectTestOptions::GetCurTestMode() == Core::MANUFACTURAL_STATIONTEST) {
        itemCheckFlag->setData(Qt::Unchecked, (int)Qt::CheckStateRole);
    }
    else {
        itemCheckFlag->setData(Qt::Checked, (int)Qt::CheckStateRole);
    }
    itemCheckFlag->setEditable(true);
    itemCheckFlag->setSelectable(true);
    itemCheckFlag->setCheckable(true);
    itemCheckFlag->setToolTip(DataManager::CTestCaseGuide::Instance().GetTestCaseName(Id));
    itemList << itemCheckFlag;

    itemList << new QStandardItem(QString("%1").arg(Index));
    itemList << new QStandardItem(DataManager::CTestCaseGuide::Instance().GetTestCaseDescription(Id));

    QPixmap SetPixmap;
    QPixmap PixMap(QString(":/Large/CheckBoxLarge/CheckBox-enabled-large.png"));
    if (!PixMap.isNull())
        SetPixmap = (PixMap.scaled(QSize(45,45),Qt::KeepAspectRatio, Qt::FastTransformation));

    QStandardItem *item = new QStandardItem;
    item->setData(SetPixmap, (int) Qt::DecorationRole);
    itemList << item;

    m_Model.setHorizontalHeaderLabels(QStringList() << ""
                                                    << Service::CMessageString::MSG_DIAGNOSTICS_TEST_NUMBER
                                                    << Service::CMessageString::MSG_DIAGNOSTICS_TEST_NAME
                                                    << "");
    m_Model.appendRow(itemList);
    mp_TestReporter->AddTestCaseId(Id);
}

/****************************************************************************/
/*!
 *  \brief This function is called when OK is clicked
 *  \iparam EnteredString = Stores line edit string
 */
/****************************************************************************/
void CRetort::OnOkClicked(const QString& EnteredString)
{

    mp_KeyBoardWidget->hide();
    m_RetortSNString.chop(4);
    m_RetortSNString.append(EnteredString.simplified());

    mp_Ui->retortSNEdit->setText(m_RetortSNString);

    QString LogString = "The serial number of retort is changed to be " + m_RetortSNString;
    Global::EventObject::Instance().RaiseEvent(EVENT_COMMON_ID, Global::tTranslatableStringList() << LogString);

    mp_Ui->beginTestBtn->setEnabled(true);
    DisconnectKeyBoardSignalSlots();

    if (mp_Module && Core::CSelectTestOptions::GetCurTestMode() == Core::MANUFACTURAL_ENDTEST) {
        mp_Module->SetSerialNumber(m_RetortSNString);
        emit UpdateModule(*mp_Module);
    }
}

/****************************************************************************/
/*!
 *  \brief This function hides the keyboard when Esc is clicked
 */
/****************************************************************************/
void CRetort::OnESCClicked()
{
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
    mp_KeyBoardWidget->hide();
}

/****************************************************************************/
/*!
 *  \brief Connects signals and slots of keyboard.
 */
/****************************************************************************/
void CRetort::ConnectKeyBoardSignalSlots()
{
    // Connect signals and slots to keyboard.
    CONNECTSIGNALSLOT(mp_KeyBoardWidget, OkButtonClicked(QString), this, OnOkClicked(QString));
    CONNECTSIGNALSLOT(mp_KeyBoardWidget, EscButtonClicked(), this, OnESCClicked());
}

/****************************************************************************/
/*!
 *  \brief Disconnects signals and slots of keyboard.
 */
/****************************************************************************/
void CRetort::DisconnectKeyBoardSignalSlots()
{
    // Disconnect signals and slots connected to keyboard.
    (void) disconnect(mp_KeyBoardWidget, SIGNAL(OkButtonClicked(QString)), this, SLOT(OnOkClicked(QString)));
    (void) disconnect(mp_KeyBoardWidget, SIGNAL(EscButtonClicked()), this, SLOT(OnESCClicked()));

}

/****************************************************************************/
/*!
 *  \brief Slot called for agitator tests
 */
/****************************************************************************/
void CRetort::BeginTest()
{
    bool IsEndTest = (Core::CSelectTestOptions::GetCurTestMode() == Core::MANUFACTURAL_ENDTEST);
    if (IsEndTest) {
        if (!mp_TestReporter->CheckSystemSN()) {
            return;
        }
    }

    if (mp_Ui->retortSNEdit->text().endsWith("XXXX")) {
        mp_MessageDlg->SetTitle(Service::CMessageString::MSG_TITLE_SERIAL_NUMBER);
        mp_MessageDlg->SetButtonText(1, Service::CMessageString::MSG_BUTTON_OK);
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(Service::CMessageString::MSG_DIAGNOSTICS_ENTER_SN);
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        if (mp_MessageDlg->exec()) {
            mp_Ui->retortSNEdit->setFocus();
            mp_Ui->retortSNEdit->selectAll();
        }
        return;
    }

    if (!IsEndTest) {
        mp_TestReporter->SetSerialNumber(m_RetortSNString);
    }

    qDebug()<<"CRetort::BeginTest  ";
    QList<Service::ModuleTestCaseID> TestCaseList;
    for(int i=0; i<m_Model.rowCount(); i++) {
        QModelIndex ModelIndex = m_Model.index(i, 0);
        QStandardItem* item = m_Model.itemFromIndex(ModelIndex);
        Qt::CheckState State = item->checkState();
        qDebug()<<"checkable="<<item->checkState()<<" selected="<<item->isSelectable()<<" tooltip="<<item->toolTip();

        if (State == Qt::Checked) {
            Service::ModuleTestCaseID Id = DataManager::CTestCaseGuide::Instance().GetTestCaseId(item->toolTip());
            TestCaseList.append(Id);
        }
    }
    if (TestCaseList.count() == 0) {
        mp_MessageDlg->SetTitle(Service::CMessageString::MSG_TITLE_ERROR);
        mp_MessageDlg->SetButtonText(1, Service::CMessageString::MSG_BUTTON_OK);
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(Service::CMessageString::MSG_DIAGNOSTICS_SELECT_TEST_CASE);
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        mp_MessageDlg->show();
    }
    else {
        Global::EventObject::Instance().RaiseEvent(EVENT_GUI_MANUF_RETORT_TEST_REQUESTED);

        EnableButton(false);
        mp_Ui->widget->setFocus();
        emit BeginModuleTest(Service::RETORT, TestCaseList);

        if (m_TestFlag) {
            mp_Ui->sendTestReportBtn->setEnabled(true);
        }

        qDebug()<<"CLaSystem::BeginTest   --- emitted";
    }
//    ->HideAbort();
}

/****************************************************************************/
/*!
 *  \brief Slot called for agitator tests
 *  \iparam Id = Test case Id
 *  \iparam Result = Result of the test
 */
/****************************************************************************/
void CRetort::SetTestResult(Service::ModuleTestCaseID Id, bool Result)
{
    QPixmap PixMapPass(QString(":/Large/CheckBoxLarge/CheckBox-Checked_large_green.png"));
    QPixmap PixMapFail(QString(":/Large/CheckBoxLarge/CheckBox-Crossed_large_red.png"));
    QPixmap SetPixMap;

    for(int i=0; i<m_Model.rowCount(); i++) {
        QStandardItem *item = m_Model.item(i, 0);
        if (item->toolTip() == DataManager::CTestCaseGuide::Instance().GetTestCaseName(Id)) {
            if (Result) {
                if (!PixMapPass.isNull())
                    SetPixMap = (PixMapPass.scaled(45,45,Qt::KeepAspectRatio));
            }
            else {
                if (!PixMapFail.isNull())
                    SetPixMap = (PixMapFail.scaled(45,45,Qt::KeepAspectRatio));
            }
            (void) m_Model.setData(m_Model.index(i, 3), SetPixMap, (int) Qt::DecorationRole);
            break;
        }
    }
    m_TestFlag = true;
}

void CRetort::EnableButton(bool EnableFlag)
{
    mp_Ui->beginTestBtn->setEnabled(EnableFlag);
}

/****************************************************************************/
/*!
 *  \brief Slot called for sending the test report to server
 */
/****************************************************************************/
void CRetort::SendTestReport()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_MANUF_RETORT_SENDTESTREPORT_REQUESTED);

    if (mp_TestReporter->GenReportFile()) {
        (void)mp_TestReporter->SendReportFile();
    }
    else {
        mp_MessageDlg->SetTitle(Service::CMessageString::MSG_TITLE_SEND_REPORT);
        mp_MessageDlg->SetButtonText(1, Service::CMessageString::MSG_BUTTON_OK);
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(Service::CMessageString::MSG_DIAGNOSTICS_GEN_REPORT_FILE_FAILED);
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        (void)mp_MessageDlg->exec();
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called to reset the test status
 */
/****************************************************************************/
void CRetort::ResetTestStatus()
{
#if 0
    mp_Ui->beginTestBtn->setEnabled(false);
    mp_Ui->heaterSNEdit->setText("006XXXXX");
    SetLineEditText(QString("006XXXXX"));
    QPixmap PixMap(QString(":/Large/CheckBoxLarge/CheckBox-enabled-large.png"));
    QPixmap SetPixMap;
    if (!PixMap.isNull())
        SetPixMap = (PixMap.scaled(45,45,Qt::KeepAspectRatio));
    (void) m_Model.setData(m_Model.index(0, 2), SetPixMap, (int) Qt::DecorationRole);
    (void) m_Model.setData(m_Model.index(1, 2), SetPixMap, (int) Qt::DecorationRole);
    mp_Ui->sendTestReportBtn->setEnabled(false);
//    mp_Ui->testSuccessLabel->setPixmap(QPixmap(QString::fromUtf8
//                                               (":/Large/CheckBoxLarge/CheckBox-enabled-large.png")));
#endif

}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CRetort::RetranslateUI()
{
    m_Model.clear();
    m_Model.setHorizontalHeaderLabels(QStringList() << ""
                                                    << Service::CMessageString::MSG_DIAGNOSTICS_TEST_NUMBER
                                                    << Service::CMessageString::MSG_DIAGNOSTICS_TEST_NAME
                                                    << "");

    if (Core::CSelectTestOptions::GetCurTestMode() == Core::MANUFACTURAL_ENDTEST ) {
        AddItem(1, Service::RETORT_LID_LOCK);
        AddItem(2, Service::RETORT_LEVEL_SENSOR_HEATING);
        AddItem(3, Service::RETORT_LEVEL_SENSOR_DETECTING);
        AddItem(4, Service::RETORT_HEATING_WITH_WATER);
        if (mp_Module) {
            m_RetortSNString = mp_Module->GetSerialNumber();
        }
    }
    else {
        AddItem(1, Service::RETORT_LEVEL_SENSOR_HEATING);
        AddItem(2, Service::RETORT_HEATING_EMPTY);
    }

    mp_TableWidget->horizontalHeader()->resizeSection(0, 50);   // 0 => Index  50 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(1, 50);   // 1 => Index  50 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(2, 350);  // 2 => Index  400 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(3, 45);   // 3 => Index  45 => Size
}

}  // end namespace DiagnosticsManufacturing

