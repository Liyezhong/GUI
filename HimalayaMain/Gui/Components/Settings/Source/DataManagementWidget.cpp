/****************************************************************************/
/*! \file DataManagementWidget.cpp
 *
 *  \brief DataManagementWidget implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-29
 *   $Author:  $ M.Scherer
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

#include "Settings/Include/DataManagementWidget.h"
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "ui_DataManagementWidget.h"
#include <QEvent>
#include <QApplication>

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CDataManagementWidget::CDataManagementWidget(QWidget *p_Parent) : MainMenu::CPanelFrame(p_Parent),mp_Ui(new Ui::CDataManagementWidget)
{
    mp_Ui->setupUi(GetContentFrame());
    // mp_DataManagement = new MainMenu::CDataManagement(this);
    //SetContent(mp_DataManagement->layout());
    SetPanelTitle(tr("Data Management"));
    //CONNECTSIGNALSIGNAL(mp_DataManagement, ExecSending(const QString, const QStringList &), this, ExecSending(const QString, const QStringList &));
    CONNECTSIGNALSLOT(mp_Ui->importButton,clicked(),this,ImportDialog());
    CONNECTSIGNALSLOT(mp_Ui->saveusbButton,clicked(),this,SaveUSBDialog());
    CONNECTSIGNALSLOT(mp_Ui->serviceExportButton,clicked(),this,ExportDialog());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDataManagementWidget::~CDataManagementWidget()
{
    try {
      //  delete mp_DataManagement;
    }
    catch (...) {
        // to please Lint.
    }
}


/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CDataManagementWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            RetranslateUI();
            break;
        default:
            break;
    }
}

void CDataManagementWidget:: ExportDialog()
{
    QStringList Type;
    Type << "User";
    emit ExecSending("DataExport", Type);
}
void CDataManagementWidget:: SaveUSBDialog()
{
    QStringList Type;
    Type << "User";
    emit ExecSending("SaveUSB", Type);
}
void CDataManagementWidget:: ImportDialog()
{
    QStringList Type;
    Type << "User";
    emit ExecSending("DataImport", Type);
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CDataManagementWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CDataManagementWidget", "Data Management", 0, QApplication::UnicodeUTF8));
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 */
/****************************************************************************/
void CDataManagementWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
   // mp_DataManagement->SetPtrToMainWindow(mp_MainWindow);
}
