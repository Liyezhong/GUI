/****************************************************************************/
/*! \file ContainerPanelWidget.h
 *
 *  \brief ContainerPanel Widget Class Definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-03
 *   $Author:  $ Swati Tiwari
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef CONTAINERPANELWIDGET_H
#define CONTAINERPANELWIDGET_H

#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MessageDlg.h"
#include "Dashboard/Include/DashboardScene.h"
#include "Dashboard/Include/DashboardDataTypes.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"


namespace Dashboard {

namespace Ui {
    class CContainerPanelWidget;
}


/****************************************************************************/
/**
 * \brief This is the widget for container panel widget.
 */
/****************************************************************************/
class CContainerPanelWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

private:
    Ui::CContainerPanelWidget *mp_Ui;                                //!< User interface
    MainMenu::CMainWindow *mp_MainWindow;                           //!< Reference to MainWindow.
    Core::CDataConnector *mp_DataConnector;                     //!< Global data container
    CDashboardScene *mp_DashboardScene;                         //!< Displays the station items
    DataManager::CDataProgramList *mp_ProgramList;       ///<  Definition/Declaration of variable mp_ProgramList
    QStringList m_FavProgramIDs;       ///<  Definition/Declaration of variable m_FavProgramIDs

    bool m_UserRoleChanged;                                     //!< Flag to Verify the Change in User Role
    MainMenu::CMessageDlg   *mp_MessageDlg;                      //!< Message Dialogue
    QString m_strProgram;
    QString m_strInformation;
    QString m_strOK, m_strNo;
    QString m_strNeedMeltParaffin;
    DataManager::CHimalayaUserSettings* m_pUserSetting;
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RetranslateUI
     *
     *  \return from RetranslateUI
     */
    /****************************************************************************/
    void RetranslateUI();
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CContainerPanelWidget
     *
     *  \param p_Parent = QWidget type parameter
     *
     *  \return from CContainerPanelWidget
     */
    /****************************************************************************/
    explicit CContainerPanelWidget(QWidget *p_Parent = 0);
    ~CContainerPanelWidget();
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
     *  \brief  Definition/Declaration of function UpdateRetortStatus
     *
     *  \param retortStatusType = DataManager::ContainerStatusType_t type parameter
     *  \param reagentGroupID = reagent Group ID
     *  \param stationID = station ID
     *  \return from UpdateRetortStatus
     */
    /****************************************************************************/
    void UpdateRetortStatus(DataManager::ContainerStatusType_t retortStatusType, const QString& reagentGroupID, const QString& stationID);
    static QString m_strWarning;       ///<  Definition/Declaration of variable m_strWarning
    static QString m_strYes;          ///<  Definition/Declaration of variable m_strYes
    static QString m_strCancel;       ///<   Definition/Declaration of variable m_strCancel

protected:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function DrawSeparatorLine
     *
     *  \return from DrawSeparatorLine
     */
    /****************************************************************************/
    void DrawSeparatorLine();
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
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Initialize
     *
     *  \return from Initialize
     */
    /****************************************************************************/
    void Initialize();

signals:
    /****************************************************************************/
    /*!
     *  \brief  emit the signal of program operation action
     *  \param programID = the selected program id
     *  \param actionType = program action type:start/pause/abort.
     */
    /****************************************************************************/
    void ProgramAction(const QString& programID, DataManager::ProgramActionType_t actionType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UpdateDashboardSceneReagentStations
     *  \param  ProgramID = the Program ID
     */
    /****************************************************************************/
    void UpdateDashboardSceneReagentStations(QString& ProgramID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ProgramSelected
     *  \param programId = the selected program id
     *  \param selectedStationList = all used stations for the selected program
     */
    /****************************************************************************/
    void ProgramSelected(QString & programId, QList<QString>& selectedStationList);
    /****************************************************************************/
    /*!
     *  \brief when user has operations in GUI
     */
    /****************************************************************************/
    void OnInteractStart();
public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnRetortLockStatusChanged
     *  \param  cmd = command for retort lock/unlock
     */
    /****************************************************************************/
    void OnRetortLockStatusChanged(const MsgClasses::CmdLockStatus& cmd);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnStationSuckDrain
     *  \param cmd = command information for sucking or draing reagent
     */
    /****************************************************************************/
    void OnStationSuckDrain(const MsgClasses::CmdStationSuckDrain & cmd);

};

}
#endif // CONTAINERPANELWIDGET_H

