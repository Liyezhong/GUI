/****************************************************************************/
/*! \file DiagnosticsManufacturing/Source/OvenManufacturing.cpp
 *
 *  \brief COven class implementation
 *
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-05-12
 *   $Author:  $ Sunny Qu
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

#include "DiagnosticsManufacturing/Include/OvenManufacturing.h"
#include "Core/Include/SelectTestOptions.h"
#include "ui_OvenManufacturing.h"
#include <QDebug>
#include <QTableWidgetItem>

namespace DiagnosticsManufacturing {

const QString REGEXP_NUMERIC_VALIDATOR  = "^[0-9]{1,5}$"; //!< Reg expression for the validator
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
COven::COven(Core::CServiceGUIConnector *p_DataConnector, MainMenu::CMainWindow *p_Parent)
    : mp_DataConnector(p_DataConnector)
    , mp_MainWindow(p_Parent)
    , mp_Ui(new Ui::COvenManufacturing)
    , mp_TestReport(NULL)
    , mp_MessageDlg(NULL)
    , m_FinalTestResult("NA")
{
    mp_Ui->setupUi(this);
    mp_Ui->ovenSNEdit->installEventFilter(this);
    mp_Ui->ovenSNEdit->setFixedWidth(FIXED_LINEEDIT_WIDTH);

    mp_Ui->beginTestBtn->setEnabled(true);
    SetLineEditText(QString("006XXXXX"));

    mp_Ui->ovenSNEdit->setText("006XXXXX");

    m_TestReport.insert("ModuleName", "Oven");
    m_TestNames.append("ModuleName");

    m_TestReport.insert("SerialNumber", "");
    m_TestNames.append("SerialNumber");

    m_TestResult << "NA" << "NA";

    mp_MessageDlg = new MainMenu::CMessageDlg();

    mp_TableWidget = new MainMenu::CBaseTable;

    mp_TableWidget->resize(SET_FIXED_TABLE_WIDTH, SET_FIXED_TABLE_HEIGHT);
    //mp_TableWidget->SetVisibleRows(VISIBLE_TABLE_ROWS);

    mp_TableWidget->horizontalHeader()->show();

    if (Core::CSelectTestOptions::GetCurTestMode() == Core::MANUFACTURAL_ENDTEST ) {
        AddItem("1", QApplication::translate("DiagnosticsMaMANUFACTURAL_ENDTESTnufacturing::COven", "Cover sensor test", 0, QApplication::UnicodeUTF8));
        AddItem("2", QApplication::translate("DiagnosticsManufacturing::COven", "Heating test (with water)", 0, QApplication::UnicodeUTF8));
    }
    else {
        AddItem("2", QApplication::translate("DiagnosticsManufacturing::COven", "Heating test (empty)", 0, QApplication::UnicodeUTF8));
    }

    mp_TableWidget->setModel(&m_Model);
    mp_TableWidget->horizontalHeader()->resizeSection(0, 50);   // 0 => Index  50 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(1, 50);   // 1 => Index  50 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(2, 350);  // 2 => Index  400 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(3, 45);   // 3 => Index  45 => Size

    mp_TableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    mp_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mp_Ui->widget->setMinimumSize(mp_TableWidget->width(), mp_TableWidget->height());
    mp_Ui->widget->SetContent(mp_TableWidget);
    mp_Ui->sendTestReportBtn->setEnabled(false);

//    mp_Ui->testSuccessLabel->setPixmap(QPixmap(QString::fromUtf8(":/Large/CheckBoxLarge/CheckBox-enabled-large.png")));

    mp_KeyBoardWidget = new KeyBoard::CKeyBoard(KeyBoard::SIZE_1, KeyBoard::QWERTY_KEYBOARD);

    CONNECTSIGNALSLOTGUI(mp_Ui->beginTestBtn, clicked(), this, BeginTest());
    CONNECTSIGNALSLOTGUI(mp_Ui->sendTestReportBtn, clicked(), this, SendTestReport());
    CONNECTSIGNALSLOTGUI(mp_MainWindow, CurrentTabChanged(int), this, ResetTestStatus());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
COven::~COven()
{     
     try {                  
        delete mp_KeyBoardWidget;
        delete mp_TableWidget;
        delete mp_Ui;
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
void COven::changeEvent(QEvent *p_Event)
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
bool COven::eventFilter(QObject *p_Object, QEvent *p_Event)
{
    if (p_Object == mp_Ui->ovenSNEdit && p_Event->type() == QEvent::MouseButtonPress)
    {
        ConnectKeyBoardSignalSlots();
        mp_KeyBoardWidget->setModal(true);
        mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("DiagnosticsManufacturing::COven",
                                                           "Enter Serial Number", 0, QApplication::UnicodeUTF8));
        mp_KeyBoardWidget->SetPasswordMode(false);
        mp_KeyBoardWidget->SetValidation(true);
        mp_KeyBoardWidget->SetMinCharLength(5);
        mp_KeyBoardWidget->SetMaxCharLength(5);
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
 *  \iparam SerialNumber = Serial number of the module
 *  \iparam Test = Test name
 */
/****************************************************************************/
void COven::AddItem(QString SerialNumber, QString Test)
{
    QList<QStandardItem *> ItemList;

    QStandardItem *itemCheckFlag = new QStandardItem;
    itemCheckFlag->setData(Qt::Checked, (int)Qt::CheckStateRole);
    itemCheckFlag->setEditable(true);
    itemCheckFlag->setSelectable(true);
    itemCheckFlag->setCheckable(true);
    ItemList << itemCheckFlag;

    ItemList << new QStandardItem(SerialNumber);
    ItemList << new QStandardItem(Test);

    QPixmap SetPixmap;
    QPixmap PixMap(QString(":/Large/CheckBoxLarge/CheckBox-enabled-large.png"));
    if (!PixMap.isNull())
        SetPixmap = (PixMap.scaled(QSize(45,45),Qt::KeepAspectRatio, Qt::FastTransformation));

    QStandardItem *item = new QStandardItem;
    item->setData(SetPixmap, (int) Qt::DecorationRole);
    ItemList << item;


    m_Model.setHorizontalHeaderLabels(QStringList() << ""
                                                    << QApplication::translate("DiagnosticsManufacturing::COven", "Nr.", 0, QApplication::UnicodeUTF8)
                                                    << QApplication::translate("DiagnosticsManufacturing::COven", "Tests", 0, QApplication::UnicodeUTF8)
                                                    << "");
    m_Model.appendRow(ItemList);
}

/****************************************************************************/
/*!
 *  \brief This function is called when OK is clicked
 *  \iparam EnteredString = Stores line edit string
 */
/****************************************************************************/
void COven::OnOkClicked(QString EnteredString)
{
    mp_KeyBoardWidget->hide();
    m_LineEditString.chop(5);
    m_LineEditString.append(EnteredString.simplified());

    mp_Ui->ovenSNEdit->setText(m_LineEditString);

    if (m_TestNames.contains("SerialNumber")) {
        m_TestReport.remove("SerialNumber");
        m_TestReport.insert("SerialNumber", m_LineEditString);
    } else {
        m_TestNames.append("SerialNumber");
        m_TestReport.insert("SerialNumber", m_LineEditString);
    }
    mp_Ui->beginTestBtn->setEnabled(true);
    DisconnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief This function hides the keyboard when Esc is clicked
 */
/****************************************************************************/
void COven::OnESCClicked()
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
void COven::ConnectKeyBoardSignalSlots()
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
void COven::DisconnectKeyBoardSignalSlots()
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
void COven::BeginTest()
{
    emit BeginModuleTest(Service::OVEN);
#if 0
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_MANUF_XAXIS_TEST_REQUESTED);
    ResetTestStatus();
    emit BeginModuleTest(Service::X_AXIS);
#endif
}

/****************************************************************************/
/*!
 *  \brief Slot called for agitator tests
 *  \iparam TestName = Test name
 *  \iparam Result = Result of the test
 */
/****************************************************************************/
void COven::SetTestResult(Service::ModuleTestNames TestName, bool Result)
{
    QString ModuleTestName;
    QString TestResult;    
    QPixmap PixMapPass(QString(":/Large/CheckBoxLarge/CheckBox-Checked_large_green.png"));
    QPixmap PixMapFail(QString(":/Large/CheckBoxLarge/CheckBox-Crossed_large_red.png"));
    QPixmap SetPixMap;

#if 0
    switch (TestName) {
    case Service::X1_REFERENCE_RUN:
        ModuleTestName = QString("X1 Reference Run");
        if (Result) {
            if (!PixMapPass.isNull())
                SetPixMap = (PixMapPass.scaled(45,45,Qt::KeepAspectRatio));
            (void) m_Model.setData(m_Model.index(0, 2), SetPixMap, (int) Qt::DecorationRole);
            m_TestResult[0] = "true";
        } else {
            if (!PixMapFail.isNull())
                SetPixMap = (PixMapFail.scaled(45,45,Qt::KeepAspectRatio));
            (void) m_Model.setData(m_Model.index(0, 2), SetPixMap, (int) Qt::DecorationRole);
            m_TestResult[0] = "false";
        }
        break;
    case Service::X2_REFERENCE_RUN:
        ModuleTestName = QString("X2 Reference Run");
        if (Result) {
            if (!PixMapPass.isNull())
                SetPixMap = (PixMapPass.scaled(45,45,Qt::KeepAspectRatio));
            (void) m_Model.setData(m_Model.index(1, 2), SetPixMap, (int) Qt::DecorationRole);
            m_TestResult[1] = "true";
        } else {
            if (!PixMapFail.isNull())
                SetPixMap = (PixMapFail.scaled(45,45,Qt::KeepAspectRatio));
            (void) m_Model.setData(m_Model.index(1, 2), SetPixMap, (int) Qt::DecorationRole);
            m_TestResult[1] = "false";
        }
        break;
    default:
        // do nothing
        break;
    }
#endif

    if (Result)  {
        TestResult = QString("PASS");
    } else {
        TestResult = QString("FAIL");
    }

    if (m_TestResult.contains("NA")) {
//        mp_Ui->testSuccessLabel->setPixmap(QPixmap(QString::fromUtf8
//                                                   (":/Large/CheckBoxLarge/CheckBox-enabled-large.png")));
        m_FinalTestResult = QString("NA");
    } else if (m_TestResult.contains("false")) {
//        mp_Ui->testSuccessLabel->setPixmap(QPixmap(QString::fromUtf8
//                                                   (":/Large/CheckBoxLarge/CheckBox-Crossed_large_red.png")));
        m_FinalTestResult = QString("FAIL");
    } else if (m_TestResult.contains("true")) {
 //       mp_Ui->testSuccessLabel->setPixmap(QPixmap(QString::fromUtf8
 //                                                  (":/Large/CheckBoxLarge/CheckBox-Checked_large_green.png")));
        m_FinalTestResult = QString("PASS");
    }

    if (m_TestNames.contains("TestResult")) {
        m_TestReport.remove("TestResult");
        m_TestReport.insert("TestResult", m_FinalTestResult);
    } else {
        m_TestNames.append("TestResult");
        m_TestReport.insert("TestResult", m_FinalTestResult);
    }

    if (m_TestNames.contains(ModuleTestName)) {
        m_TestReport.remove(ModuleTestName);
        m_TestReport.insert(ModuleTestName, TestResult);
    } else {
        m_TestNames.append(ModuleTestName);
        m_TestReport.insert(ModuleTestName, TestResult);
    }
    mp_Ui->sendTestReportBtn->setEnabled(true);
}

/****************************************************************************/
/*!
 *  \brief Slot called for sending the test report to server
 */
/****************************************************************************/
void COven::SendTestReport()
{
//    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_MANUF_XAXIS_SENDTESTREPORT_REQUESTED);

    if (m_LineEditString.isEmpty()) {
        mp_MessageDlg->SetTitle(QApplication::translate("DiagnosticsManufacturing::COven",
                                                        "Serial Number", 0, QApplication::UnicodeUTF8));
        mp_MessageDlg->SetButtonText(1, QApplication::translate("DiagnosticsManufacturing::COven",
                                                                "Ok", 0, QApplication::UnicodeUTF8));
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(QApplication::translate("DiagnosticsManufacturing::COven",
                                             "Please enter the serial number.", 0, QApplication::UnicodeUTF8));
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->show();
    } else {

        DataManager::ModuleNumbers_t *ModuleNumbers = mp_DataConnector->GetServiceParameters()->GetModuleNumbers();

        mp_TestReport = new DiagnosticsManufacturing::CTestReportGeneration(ModuleNumbers->XAxis, m_LineEditString,
                                                                            m_FinalTestResult);
        bool Result = mp_TestReport->CreateTestReportFile(m_TestNames, m_TestReport);

        if (Result) {
            mp_MessageDlg->SetTitle(QApplication::translate("DiagnosticsManufacturing::COven",
                                                            "Test Report", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetButtonText(1, QApplication::translate("DiagnosticsManufacturing::COven",
                                                                    "Ok", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideButtons();
            mp_MessageDlg->SetText(QApplication::translate("DiagnosticsManufacturing::COven",
                                              "Test report saved successfully.", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetIcon(QMessageBox::Information);
            mp_MessageDlg->show();
        } else {
            mp_MessageDlg->SetTitle(QApplication::translate("DiagnosticsManufacturing::COven",
                                                            "Test Report", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetButtonText(1, QApplication::translate("DiagnosticsManufacturing::COven",
                                                                    "Ok", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideButtons();
            mp_MessageDlg->SetText(QApplication::translate("DiagnosticsManufacturing::COven",
                                                           "Test report save failed.", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetIcon(QMessageBox::Critical);
            mp_MessageDlg->show();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called to reset the test status
 */
/****************************************************************************/
void COven::ResetTestStatus()
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
void COven::RetranslateUI()
{

    m_Model.setHorizontalHeaderLabels(QStringList() << QApplication::translate("DiagnosticsManufacturing::COven",
                                                                               "Nr.", 0, QApplication::UnicodeUTF8)
                                                    << QApplication::translate("DiagnosticsManufacturing::COven",
                                                                               "Tests", 0, QApplication::UnicodeUTF8)
                                                    << "");

    m_Model.clear();

    AddItem("1", QApplication::translate("DiagnosticsManufacturing::COven",
                                         "X1 Reference Run", 0, QApplication::UnicodeUTF8));
    AddItem("2", QApplication::translate("DiagnosticsManufacturing::COven",
                                         "X2 Reference Run", 0, QApplication::UnicodeUTF8));

    mp_TableWidget->horizontalHeader()->resizeSection(0, 50);   // 0 => Index  50 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(1, 400);  // 1 => Index  400 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(2, 45);   // 2 => Index  45 => Size

    mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("DiagnosticsManufacturing::COven",
                                             "Enter Serial Number", 0, QApplication::UnicodeUTF8));

    mp_MessageDlg->SetTitle(QApplication::translate("DiagnosticsManufacturing::COven",
                                                    "Serial Number", 0, QApplication::UnicodeUTF8));

    mp_MessageDlg->SetText(QApplication::translate("DiagnosticsManufacturing::COven",
                           "Please enter the serial number.", 0, QApplication::UnicodeUTF8));

    mp_MessageDlg->SetTitle(QApplication::translate("DiagnosticsManufacturing::COven",
                                                    "Test Report", 0, QApplication::UnicodeUTF8));

    mp_MessageDlg->SetText(QApplication::translate("DiagnosticsManufacturing::COven",
                                                  "Test report saved successfully.", 0, QApplication::UnicodeUTF8));

    mp_MessageDlg->SetText(QApplication::translate("DiagnosticsManufacturing::COven",
                                                   "Test report save failed.", 0, QApplication::UnicodeUTF8));
}

}  // end namespace DiagnosticsManufacturing