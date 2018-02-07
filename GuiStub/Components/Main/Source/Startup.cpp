/****************************************************************************/
/*! \file Components/Main/Source/Startup.cpp
 *
 *  \brief Implementation file for Startup file
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-12-26
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

#include <Main/Include/Startup.h>
#include <Global/Include/Utils.h>
#include <Global/Include/SystemPaths.h>
//#include <ImportExport/WriteArchive/Include/WriteArchive.h>
//#include <ImportExport/ReadArchive/Include/ReadArchive.h>
//#include <ImportExport/General/Include/General.h>
//#include <ImportExport/AbstractFile/Include/AbstractFile.h>
#include <QMessageBox>
#include <QListView>
#include <QStringListModel>
#include <QLabel>
#include <QDebug>
#include <QFileDialog>
//#include <MainMenu/Include/WaitDialog.h>


/****************************************************************************/

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \param[in] MasterThreadController  TestHimalayaMasterThreadController instance
 *
*/
/****************************************************************************/
Startup::Startup(Himalaya::HimalayaMasterThreadController &MasterThreadController)
    : m_MasterThreadController(MasterThreadController)
{
}

/****************************************************************************/
/*!
 *  \brief Destructor
*/
/****************************************************************************/
Startup::~Startup()
{    
}


/****************************************************************************/
/*!
 *  \brief GuiInit : Initializes the Test SW GUI. Displays the main window
*/
/****************************************************************************/
void Startup::GuiInit()
{
    QTabWidget *p_Widget = new QTabWidget();
    Ui_MainWindow.setupUi(p_Widget);
    p_Widget->show();

    // When button "Send XML" clicks the content of the xml files
    // is sent to the GUI and a status text gets updated
    QObject::connect(Ui_MainWindow.pushButton_SendXML, SIGNAL(clicked()),this, SLOT(SendXML()));

    // When button clicked it will change the language in the GUI
    QObject::connect(Ui_MainWindow.btnSendLanguageFile, SIGNAL(clicked()),this, SLOT(onChangeLanguage()));

    // When button clicked it will Send event message to the GUI
    QObject::connect(Ui_MainWindow.btnSendMessage, SIGNAL(clicked()),this, SLOT(onSendMessage()));
    QObject::connect(Ui_MainWindow.btnRemoveMsgBox, SIGNAL(clicked()),this, SLOT(onRemoveMessageBox()));


    CONNECTSIGNALSLOT(Ui_MainWindow.btnStartProcess, clicked(),&m_MasterThreadController, SendProcessStart());

    CONNECTSIGNALSLOT(Ui_MainWindow.btnStopProcess, clicked(), &m_MasterThreadController, SendProcessStop());

    QObject::connect(Ui_MainWindow.btnSetDateTime, SIGNAL(clicked()),this, SLOT(onSetDateAndTime()));

    CONNECTSIGNALSLOT(&m_MasterThreadController, SendAckToTextBrowser(QString),this, ShowCmdAcknowledgements(QString));

    Ui_MainWindow.buttonGroup->exclusive();
    Ui_MainWindow.buttonGroup_2->exclusive();
}

/****************************************************************************/
/*!
 *  \brief UserMessage : Once 'Sequence1' button is pressed this mesasge
*   is displayed.
*/
/****************************************************************************/
void Startup::UserMessage()
{
    QMessageBox msgBox;
    msgBox.setText("Start Himalaya GUI manually");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle("Test SW");
    int ret = msgBox.exec();

    switch(ret) {
        case QMessageBox::Ok:
            //m_MasterThreadController.SendRackUpdateSequenceOne();
            break;
        default:
            break;
    }
}


/****************************************************************************/
/*!
 *  \brief SendXML : Once button "Send XML" is pressed the content of the xml
 *  files is sent to the GUI and status updates in the control
*/
/****************************************************************************/
void Startup::SendXML()
{

    if (m_MasterThreadController.SendXML(*Ui_MainWindow.textBrowser)) {
        //msgBox.setText("SendXML OK");
        Ui_MainWindow.textBrowser->setTextColor(QColor(0,0,0,255));
        Ui_MainWindow.textBrowser->append("SendXML :: OK");
    } else {
        //Alert with red color
        Ui_MainWindow.textBrowser->setTextColor(QColor(255,0,0,255));
        Ui_MainWindow.textBrowser->append("SendXML :: NOK");
        //msgBox.setText("SendXML failed!");

    }
}

/****************************************************************************/
/*!
 *  \brief onChangeLanguage : it will change the language in the GUI
*/
/****************************************************************************/
void Startup::onChangeLanguage()
{
    if (m_MasterThreadController.SendChangeLanguage(*Ui_MainWindow.textBrowser)) {
        Ui_MainWindow.textBrowser->setTextColor(QColor(0,0,0,255));
        Ui_MainWindow.textBrowser->append(Ui_MainWindow.btnSendLanguageFile->text() + " :: OK");
    }
    else {
        Ui_MainWindow.textBrowser->setTextColor(QColor(255,0,0,255));
        Ui_MainWindow.textBrowser->append(Ui_MainWindow.btnSendLanguageFile->text() + " :: NOK");
    }
}


/****************************************************************************/
/*!
 *  \brief onSetDateAndTime : it will set the new date and time in the GUI
*/
/****************************************************************************/
void Startup::onSetDateAndTime()
{
    QDialog dialog;
    Ui_dialog.setupUi(&dialog);
    if (dialog.exec() == (int)QDialog::Accepted) {
        QDateTime dateTime = Ui_dialog.dateTimeEdit->dateTime();
        if(m_MasterThreadController.SetDateAndTime(dateTime)) {
            Ui_MainWindow.textBrowser->setTextColor(QColor(0,0,0,255));
            Ui_MainWindow.textBrowser->append(Ui_MainWindow.btnSetDateTime->text() + " :: OK");
        }
        else {
            Ui_MainWindow.textBrowser->setTextColor(QColor(255,0,0,255));
            Ui_MainWindow.textBrowser->append(Ui_MainWindow.btnSetDateTime->text() + " :: NOK");
        }
    }
}


/****************************************************************************/
/*!
 *  \brief ShowCmdAcknowledgements : Display Command acknowledgements
 *  \param[in] AckMsg -Acknowledgement string
*/
/****************************************************************************/
void Startup::ShowCmdAcknowledgements(QString AckMsg)
{
    QFont AckFont("Times", 10, QFont::Bold);
    Ui_MainWindow.TextBrowserAck->setTextColor(QColor(0,0,0,255));
    Ui_MainWindow.TextBrowserAck->setFont(AckFont);
    Ui_MainWindow.TextBrowserAck->append(AckMsg);
}


/****************************************************************************/
/*!
 *  \brief onSendMessage : Sending different Event Messages to the GUI
*/
/****************************************************************************/
void Startup::onSendMessage()
{
    quint32 MessageType;
    QString Msg;
    Global::GuiButtonType ButtonType;
    bool StatusBarIcon;
    QString EventString;
    if (Ui_MainWindow.infoRadioButton->isChecked()) {
        MessageType = 2;
        Msg = "Information Message";
    } else if (Ui_MainWindow.warningRadioButton->isChecked()) {
        MessageType = 3;
        Msg = "Warning Message";
    }else if (Ui_MainWindow.errorRadioButton->isChecked()) {
       MessageType = 4;
       Msg = "Error Message";
    }

    if (Ui_MainWindow.radioButtonOk->isChecked()) {
        ButtonType = Global::OK;
    }
    else if (Ui_MainWindow.radioButtonOkCancel->isChecked()) {
        ButtonType = Global::OK_CANCEL;
    }
    else if (Ui_MainWindow.radioButtonYesNo->isChecked()) {
        ButtonType = Global::YES_NO;
    }
    else if (Ui_MainWindow.radioButtonContinueStop->isChecked()) {
        ButtonType = Global::CONTINUE_STOP;
    }

    if (Ui_MainWindow.yesRadioButton->isChecked()) {
        StatusBarIcon = true;
    }
    else if (Ui_MainWindow.noRadioButton->isChecked()) {
        StatusBarIcon =false;
    }
    EventString = Ui_MainWindow.EventStrTextEdit->toPlainText();
    if(m_MasterThreadController.SendEventMessage(MessageType, ButtonType,StatusBarIcon,EventString)) {
        Ui_MainWindow.textBrowserErrMsg->setTextColor(QColor(0,0,0,255));
        Ui_MainWindow.textBrowserErrMsg->append(Msg + " :: OK");
    }
    else {
        Ui_MainWindow.textBrowserErrMsg->setTextColor(QColor(255,0,0,255));
        Ui_MainWindow.textBrowserErrMsg->append(Msg + " :: NOK");
    }
}

void Startup::onRemoveMessageBox()
{
    if (!Ui_MainWindow.lineEdit->text().isEmpty()) {
        if(m_MasterThreadController.SendRemoveMessageBox(Ui_MainWindow.lineEdit->text().toLongLong())) {
            Ui_MainWindow.textBrowserErrMsg->setTextColor(QColor(0,0,0,255));
            Ui_MainWindow.textBrowserErrMsg->append("Event ID with " + Ui_MainWindow.lineEdit->text() + " is removed");
        }
        else {
            Ui_MainWindow.textBrowserErrMsg->setTextColor(QColor(255,0,0,255));
            Ui_MainWindow.textBrowserErrMsg->append(Ui_MainWindow.btnRemoveMsgBox->text() + " :: NOK");
        }
    }
    else {
        Ui_MainWindow.textBrowserErrMsg->setTextColor(QColor(255,0,0,255));
        Ui_MainWindow.textBrowserErrMsg->append(Ui_MainWindow.infoRadioButton->text() + " :: NOK");
    }
}

