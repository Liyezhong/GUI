/****************************************************************************/
/*! \file Components/Main/Include/Startup.h
 *
 *  \brief Include file for startup routine.
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

#ifndef STARTUP_H
#define STARTUP_H

#include <QObject>
#include <Main/Build/ui_tabwidget.h>
#include <Main/Build/ui_waterstationdialog.h>
#include <Main/Build/ui_dialog.h>
#include <TestHimalayaMasterThread/Include/TestHimalayaMasterThreadController.h>
#include <QListView>
#include <QStringListModel>

/****************************************************************************/
/**
* \brief This class is the start up for the test sw.
*/
/****************************************************************************/
class Startup : public QObject
{
Q_OBJECT

public:
    Startup(Himalaya::HimalayaMasterThreadController &MasterThreadController);  //!< Constructor of the Startup class
    ~Startup();  //!< Destructor of the Startup class

private:
    Himalaya::HimalayaMasterThreadController &m_MasterThreadController;//!< TestHimalayaMasterThreadController instance
    Ui::TabWidget Ui_MainWindow;  //!< Main Window of the Test SW
    Ui::Dialog Ui_dialog;         //!< Dialog for the DateAndTime

public slots:

    /****************************************************************************/
    /*!
     *  \brief GuiInit : Initializes the test SW GUI. Displays the main window
    */
    /****************************************************************************/
    void GuiInit();
    /****************************************************************************/
    /*!
     *  \brief UserMessage : Once 'Run Test' button is pressed this mesasge
     *   is displayed.
    */
    /****************************************************************************/
    void UserMessage();

    /****************************************************************************/
    /*!
     *  \brief SendXML : Once button "Send XML" is pressed the content of the xml
     *  files is sent to the GUI and status will be updated in the control
    */
    /****************************************************************************/
    void SendXML();

    /****************************************************************************/
    /*!
     *  \brief onChangeLanguage : it will change the language in the GUI
    */
    /****************************************************************************/
    void onChangeLanguage();

    /****************************************************************************/
    /*!
     *  \brief onSetDateAndTime : it will set the new date and time in the GUI
    */
    /****************************************************************************/
    void onSetDateAndTime();

    /****************************************************************************/
    /*!
     *  \brief ShowCmdAcknowledgements : Display Command acknowledgements
    * \param[in]   QString = Acknowledgement string.
    */
    /****************************************************************************/
    void ShowCmdAcknowledgements(QString);

    /****************************************************************************/
    /*!
     *  \brief onSendMessage : Sending different Event Messages to the GUI
    */
    /****************************************************************************/
    void onSendMessage();

    /************************************************************************************/
    /*!
     *  \brief onRemoveMessageBox : Remove an Event message from the queue
    */
    /************************************************************************************/
    void onRemoveMessageBox();
};

#endif // STARTUP_H
