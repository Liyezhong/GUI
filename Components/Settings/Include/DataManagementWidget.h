/****************************************************************************/
/*! \file DataManagementWidget.h
 *
 *  \brief DataManagementWidget definition.
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

#ifndef DATAMANAGEMENTWIDGET_H
#define DATAMANAGEMENTWIDGET_H

#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/DataManagement.h"
#include "MainMenu/Include/PanelFrame.h"

namespace Settings {

namespace Ui {
    class CDataManagementWidget;
}

/****************************************************************************/
/**
 * \brief Frame for the data import / export widget
 */
/****************************************************************************/
class CDataManagementWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

    friend class CTestSettings;
public:
    explicit CDataManagementWidget(QWidget *p_Parent = 0);
    ~CDataManagementWidget();
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);

private:
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    Ui::CDataManagementWidget *mp_Ui;       ///<  Definition/Declaration of variable mp_Ui
    bool m_ProcessRunning;                          //!< Process running state
    bool remoteCareSoftwareAvailable;               //!< Process remote care software update status
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role

    void RetranslateUI();
    void ResetButtons();

protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *);


private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot ServiceExportDialog
     */
    /****************************************************************************/
    void ServiceExportDialog();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SaveToUSBDialog
     */
    /****************************************************************************/
    void SaveToUSBDialog();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot ImportDialog
     */
    /****************************************************************************/
    void ImportDialog();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SWUpdate
     */
    /****************************************************************************/
    void SWUpdate();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot RemoteSWUpdate
     */
    /****************************************************************************/
    void RemoteSWUpdate();
    void DisplaySelectionDialog(QStringList FileList);
    void SetRemoteSWButtonState(bool State);

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnUserLevelChanged
     */
    /****************************************************************************/
    void OnUserLevelChanged(QDataStream &);

signals:

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ExecSending
     */
    /****************************************************************************/
    void ExecSending(const QString, const QStringList &);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal EmitSWUpdate
     */
    /****************************************************************************/
    void EmitSWUpdate(bool);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when file selection window pop-up and clicks ok.
     *
     *  \iparam FileList = List of files which needs to be imported.
     *
     */
    /****************************************************************************/
    void SelectedImportFileList(QStringList FileList);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when RCsoftwre update button is clicked.
     *
     */
    /****************************************************************************/
    void UpdateSoftwareFromRC();
};

} //end of namespace
#endif // DATAMANAGEMENTWIDGET_H

