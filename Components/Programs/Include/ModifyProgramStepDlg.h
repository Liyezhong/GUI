/****************************************************************************/
/*! \file ModifyProgramStepDlg.h
 *
 *  \brief Include file for class CModifyProgramStepDlg.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-28
 *   $Author:  $ M.Scherer, C.Adaragunchi1234, Swati Tiwari
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

#ifndef PROGRAMS_MODIFYPROGRAMSTEPDLG_H
#define PROGRAMS_MODIFYPROGRAMSTEPDLG_H

#include "Core/Include/ReagentRMSModel.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "HimalayaDataContainer/Containers/Programs/Include/ProgramStep.h"
#include "HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h"
#include "Programs/Include/ModifyProgramDlg.h"
#include "Core/Include/ReagentStationEditModel.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MessageDlg.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/MainWindow.h"
#include "Programs/Include/ProgramWidget.h"
#include "Programs/Include/StepModel.h"

//lint -sem(Programs::CModifyProgramStepDlg::InitDurationWidget, initializer)
//lint -e1565
namespace DataManager
{
    class CDataReagentGroupList;
}

namespace Programs {

namespace Ui {
    class CModifyProgramStepDlg;
}

/****************************************************************************/
/**
 * \brief Edit dialog for a program stop.
 */
/****************************************************************************/
class CModifyProgramStepDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT

private:
    Ui::CModifyProgramStepDlg *mp_Ui;                   //!< User interface
    MainMenu::CBaseTable *mp_TableWidget;               //!< Reagent table
    MainMenu::CScrollWheel *mp_ScrollWheelHour;         //!< Time of the program step (hours)
    MainMenu::CScrollWheel *mp_ScrollWheelMin;          //!< Time of the program step (minutes)
    MainMenu::CScrollWheel *mp_ScrollWheelTemp;         //!< Temperature of the program step
    MainMenu::CMessageDlg *mp_MessageBox;               //!< Message dialog object
    DataManager::CProgramStep *mp_ProgramStep;          //!< The data of the dialog
    DataManager::CProgramStep m_ProgramStep;            //!< Local Program Step object.
    DataManager::CDataReagentList *mp_ReagentList;      //!< Global reagent list
    DataManager::CDashboardDataStationList *mp_DashboardStationList;      //!< Global Station list
    QButtonGroup m_ButtonGroup;                         //!< Group combining tolerance radio buttons
    qint32 m_RowSelected;                               //!< Row selected in table
    bool m_RowNotSelected;                              //!< True - row not selected else selected
    bool m_NewProgramStep;                              //!< True if dialog popped up for New Program Step.
    bool m_ProcessRunning;                              //!< State of process running    
    Programs::ButtonType_t m_ModifyProgramDlgButtonType;//!< Informs which button was clicked in ModifyProgramDialog
    bool m_ReagentExists;                               //!< True if Reagent exists in the reagent model
    DataManager::CUserSettings *mp_UserSettings;        //!< Data object
    CStepModel m_StepModel;                             //!< Model of the program table
    Core::CDataConnector *mp_DataConnector;             //!< Global data container
    DataManager::CProgram *mp_Program;       ///<  Definition/Declaration of variable mp_Program
    Core::CReagentStationEditModel m_ReagentEditModel;  //!< Model for the table
    QString m_ReagentID;       ///<  Definition/Declaration of variable m_ReagentID
    QPixmap* m_pAmbientTempraturePixmap;       ///<  Definition/Declaration of variable m_pAmbientTempraturePixmap
    QPixmap* m_pAmbientTempratureBigPixmap;       ///<  Definition/Declaration of variable m_pAmbientTempratureBigPixmap
    QString m_strConfirmMsg;       ///<  Definition/Declaration of variable m_strConfirmMsg
    QString m_strOK;       ///<  Definition/Declaration of variable m_strOK
    QString m_strDiffTemp;       ///<  Definition/Declaration of variable m_strDiffTemp
    QString m_strInforMsg;       ///<  Definition/Declaration of variable m_strInforMsg
    QString m_strYes;       ///<  Definition/Declaration of variable m_strYes
    QString m_strNo;       ///<  Definition/Declaration of variable m_strNo
private:
    void showEvent(QShowEvent *p_Event);
    void RetranslateUI();

public:
    explicit CModifyProgramStepDlg(QWidget *p_Parent = NULL, MainMenu::CMainWindow *p_MainWindow = NULL,
                                   Core::CDataConnector *p_DataConnector= NULL);

    ~CModifyProgramStepDlg();

    /****************************************************************************/
    /*!
     *  \brief Informs which button was clicked on Menu program screen
     *
     *  \iparam ButtonType - Edit/New/Copy
     */
    /****************************************************************************/
    void SetButtonType(ButtonType_t ButtonType) { m_ModifyProgramDlgButtonType = ButtonType;}
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetButtonType
     *
     *  \return from GetButtonType
     */
    /****************************************************************************/
    ButtonType_t GetButtonType(){return m_ModifyProgramDlgButtonType;}

    void SetProgramStep(DataManager::CProgramStep *p_ProgramStep);
    void NewProgramStep();
    void SelectRow(qint32 Row);
    void ResetButtons(bool Disable);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SetRadioButtonStatus
     *
     *  \param Pressure =  QString type parameter
     *  \param Vaccum =  QString type parameter
     *
     *  \return from SetRadioButtonStatus
     */
    /****************************************************************************/
    void SetRadioButtonStatus(const QString &Pressure, const QString &Vaccum);

private:
    void InitDurationWidget();
    void ResizeHorizontalSection();
    void InitTemperatureWidget(const DataManager::CReagent * pReagent, DataManager::CProgramStep *pProgramStep);
protected:
    void changeEvent(QEvent *p_Event);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnOk
     */
    /****************************************************************************/
    void OnOk();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnCancel
     */
    /****************************************************************************/
    void OnCancel();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnSelectionChanged
     */
    /****************************************************************************/
    void OnSelectionChanged(QModelIndex);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProcessStateChanged
     */
    /****************************************************************************/
    void OnProcessStateChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateUserSetting
     */
    /****************************************************************************/
    void UpdateUserSetting();

signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when a new program step is added.
     *
     *  \iparam p_ProgramStep = New Program step.
     *  \iparam AddProgramStep = True if New Program step is added else False
     */
    /****************************************************************************/
    void AddProgramStep(DataManager::CProgramStep *p_ProgramStep, bool AddProgramStep);
};

} // end namespace Programs

#endif // PROGRAMS_MODIFYPROGRAMSTEPDLG_H

