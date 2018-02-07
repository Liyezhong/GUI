/****************************************************************************/
/*! \file ModifyProgramDlg.h
 *
 *  \brief Include file for class CModifyProgramDlg.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-28
 *   $Author:  $ M.Scherer, C.Adaragunchi, Swati Tiwari
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

#ifndef PROGRAMS_MODIFYPROGRAMDLG_H
#define PROGRAMS_MODIFYPROGRAMDLG_H

#include "Core/Include/DataConnector.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "Programs/Include/ModifyProgramStepDlg.h"
#include "Core/Include/ReagentRMSModel.h"
#include "Programs/Include/StepModel.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "Programs/Include/ProgramWidget.h"
#include "MainMenu/Include/MainWindow.h"
#include "Core/Include/DataContainer.h"
#include "MainMenu/Include/ContentScroller.h"
#include "Programs/Include/ModifyProgramIconDlg.h"

namespace Programs {
    //#define MAX_PROGRAM_STEPS 50 //!< Maximum number of program steps
    #define MAX_PROGRAM_STEPS 14 //!< Maximum number of program steps
    #define MAX_LONGNAME_LENGTH 20  //!< Maximum length program long name
    #define MIN_LONGNAME_LENGTH 1   //!< Manimum length program long name
    #define MAX_SHORTNAME_LENGTH 3  //!< Maximum length program short name
    #define MIN_SHORTNAME_LENGTH 1  //!< Maximum length program short name

namespace Ui {
    class CModifyProgramDlg;
}

/****************************************************************************/
/**
 * \brief Edit dialog for a user defined  program.
 */
/****************************************************************************/
class CModifyProgramDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class CTestPrograms;

private:
    Ui::CModifyProgramDlg *mp_Ui;                   //!< User interface
    CModifyProgramStepDlg *mp_ModifyProgStepDlg;    //!< Program step edit dialog
    MainMenu::CBaseTable *mp_TableWidget;           //!< Program step table
    Core::CDataConnector *mp_DataConnector;         //!< Global data container
    DataManager::CProgram m_Program;                //!< Temporary/local Program data used for New Program
    DataManager::CProgramStep m_SelectedStep;       //!< Currently selected program step
    CStepModel m_StepModel;                         //!< Model for the program step table
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;         //!< Reference to Keyboard widget
    bool m_ProgNameBtnClicked;                      //!< True if Btn Program Name is clicked
    bool m_ProgShortNameBtnClicked;                 //!< True if Btn Program Short Name is clicked
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    bool m_ProcessRunning;                          //!< Process running state
    Programs::ButtonType_t m_ButtonType;            //!< Informs which button was clicked in program dialog
    QString m_TempColor;                            //!< String to store program color for temperory
    CProgramModel m_ProgramModel;                   //!< Program model object
    MainMenu::CMessageDlg *mp_MessageDlg;             //!< Message Dialog object
    DataManager::CProgramStep m_LastProgramStep;    //!< ProgramStep object
    DataManager::CDataProgramList m_ProgramListClone;   //!< ProgramList object
    DataManager::CProgram *mp_NewProgram;           //!< Program object
    bool m_ColorReplaced;                           //!< True if program color is replaced
    QList<DataManager::CProgramStep *> m_ListOfProgramSteps; //!< Container for Program Step pointers.
    MainMenu::CContentScroller m_ContentScroller;       //!< ContentScroller object
    DataManager::CReagentGroup m_ReagentGroup;       ///<  Definition/Declaration of variable m_ReagentGroup
    MainMenu::CMessageDlg m_MessageDlg;             //!< Information Message Dialog
    CModifyProgramIconDlg *mp_ModifyProgramIconDlg;     //!< Edit Program Icon dialog
    DataManager::CProgramStep m_LeicaProgramStepOne;           //!< ProgramStep object
    QString m_Icon;                                   //!< Program Icon Name
    DataManager::CUserSettings m_UserSettings;          //!< UserSettings object
    quint8 m_RowIndex;       ///<  Definition/Declaration of variable m_RowIndex

    QString m_strSelectIcon;       ///<  Definition/Declaration of variable m_strSelectIcon
    QString m_strConfirmMsg;       ///<  Definition/Declaration of variable m_strConfirmMsg
    QString m_strYes;       ///<  Definition/Declaration of variable m_strYes
    QString m_strOK;       ///<  Definition/Declaration of variable m_strOK
    QString m_strClose;       ///<  Definition/Declaration of variable m_strClose
    QString m_strCancel;       ///<  Definition/Declaration of variable m_strCancel
    QString m_strDelProgramStep;       ///<  Definition/Declaration of variable m_strDelProgramStep
    QString m_strEnterValidName;       ///<  Definition/Declaration of variable m_strEnterValidName
    QString m_strSeclectIcon;       ///<  Definition/Declaration of variable m_strSeclectIcon
    QString m_strLastProgName;       ///< Definition/Declaration of variable m_strPrevProgName
    bool    m_bIconSelected;        ///<  Definition/Declaration of variable m_bIconSelected
    bool    m_isInSaving;           ///<    Definition/Declaration of variable m_isInSaving

protected:
    void showEvent(QShowEvent *p_Event);
    void changeEvent(QEvent *p_Event);

public:
    explicit CModifyProgramDlg(QWidget *p_Parent = NULL, KeyBoard::CKeyBoard *p_KeyBoard = NULL,
                               MainMenu::CMainWindow *p_MainWindow = NULL,
                               Core::CDataConnector *p_DataConnector = NULL);
    ~CModifyProgramDlg();
    void InitDialog(DataManager::CProgram const *p_Program);
    void SetRowFocus(qint32 CurrentRow);

    /****************************************************************************/
    /*!
     *  \brief Informs which button was clicked on Menu program screen
     *
     *  \iparam ButtonType - Edit/New/Copy
     */
    /****************************************************************************/
    void SetButtonType(ButtonType_t ButtonType) { m_ButtonType = ButtonType;}

    /****************************************************************************/
    /*!
     *  \brief Closes ModifyProgramStep dialog
     *
     */
    /****************************************************************************/
    void CloseDialogModifyStepDlg();
    void NewProgram();    
    void DeleteSelectedStep(DataManager::CProgram* p_CurrentProgram);
    /****************************************************************************/
    /*!
     *  \brief Enable save button in ModifyProgramStep dialog
     *
     */
    /****************************************************************************/
    void EnablebtnSave();

private:
    void ResizeHorizontalSection();
    DataManager::CProgramStep *SelectedStep();
    void RetranslateUI();
    void ResetButtons(DataManager::CProgram &CurrentProgram, bool SelectionChanged);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ButtonPrgIconEnable
     *
     *  \param enable = bool type parameter
     *
     *  \return from ButtonPrgIconEnable
     */
    /****************************************************************************/
    void ButtonPrgIconEnable(bool enable);
    void ConnectKeyBoardSignalSlots();
    void DisconnectKeyBoardSignalSlots();
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnEditName
     */
    /****************************************************************************/
    void OnEditName();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnEdit
     */
    /****************************************************************************/
    void OnEdit();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnNewStep
     */
    /****************************************************************************/
    void OnNewStep();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnCopy
     */
    /****************************************************************************/
    void OnCopy();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnDelete
     */
    /****************************************************************************/
    void OnDelete();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnSave
     */
    /****************************************************************************/
    void OnSave();
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
     *  \brief  Definition/Declaration of OnUserRoleChanged
     */
    /****************************************************************************/
    void OnUserRoleChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateProgramStepTable
     */
    /****************************************************************************/
    void UpdateProgramStepTable(DataManager::CProgramStep* ProgramStep, bool AddProgramStep);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of CloseDialogs
     */
    /****************************************************************************/
    void CloseDialogs();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnIconClicked
     */
    /****************************************************************************/
    void OnIconClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of UpdateProgramIcon
     */
    /****************************************************************************/
    void UpdateProgramIcon(DataManager::CProgram *Program);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnOkClicked
     *  \iparam EnteredText text from keyboard
     */
    /****************************************************************************/
    void OnOkClicked(QString EnteredText);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnESCClicked
     */
    /****************************************************************************/
    void OnESCClicked();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnOkClicked
     *  \iparam str string to handle
     *  \return handled string
     */
    /****************************************************************************/
    QString HandleEscapedChar(QString& str);
signals:

    /****************************************************************************/
    /*!
     *  \brief This siganl is emitted when Program Icon is updated.
     */
    /****************************************************************************/
    void ProgramIconUpdated();
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when a program is updated.
     *
     *  \iparam Program = Selected Program.
     */
    /****************************************************************************/
    void UpdateProgram(DataManager::CProgram &Program);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when a new program is added.
     *
     *  \iparam Program = New Program.
     */
    /****************************************************************************/
    void AddProgram(DataManager::CProgram &Program);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when Cancel button in the dailog is pressed.
     *
     */
    /****************************************************************************/
    void CancelClicked();
};

} // end namespace Programs

#endif // PROGRAMS_MODIFYPROGRAMDLG_H

