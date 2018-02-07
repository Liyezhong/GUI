/****************************************************************************/
/*! \file ProgramWidget.h
 *
 *  \brief Include file for class CProgramWidget.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-28
 *   $Author:  $ M.Scherer, C.Adaragunchi1234
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

#ifndef PROGRAMS_PROGRAMWIDGET_H
#define PROGRAMS_PROGRAMWIDGET_H

#include "Core/Include/DataConnector.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/PanelFrame.h"
#include "Programs/Include/ProgramModel.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "MainMenu/Include/MainWindow.h"
#include "HimalayaDataContainer/Containers/Programs/Include/ProgramBase.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"

namespace Programs {

namespace Ui {
    class CProgramWidget;
}

//!< Forward declaration of Manual Program Dlg and Modify Program Dialog class
class CManualProgramDlg;
class CModifyProgramDlg;
class CModifyProgramStepDlg;

//!< Button Type -Edit, New or Copy
typedef enum {
    EDIT_BTN_CLICKED = 1,
    NEW_BTN_CLICKED,
    COPY_BTN_CLICKED
}ButtonType_t;

/****************************************************************************/
/**
 * \brief This is the main widget for the  programs of the device.
 */
/****************************************************************************/
class CProgramWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT
    friend class CTestPrograms;

private:
    Ui::CProgramWidget *mp_Ui;                  //!< User interface
    MainMenu::CBaseTable *mp_TableWidget;       //!< Table for the program list
    Core::CDataConnector *mp_DataConnector;     //!< Global data container
    DataManager::CProgram const *mp_Program;    //!< Currently selected program
    CProgramModel m_ProgramModel;               //!< Model of the program table
    CModifyProgramDlg *mp_ModifyProgramDlg;     //!< Modify program dialog
    MainMenu::CMainWindow *mp_MainWindow;       //!< Reference to main window
    MainMenu::CMessageDlg m_MessageDlg;         //!< Information Message Dialog
    bool m_UserRoleChanged;                     //!< True if user Role changed else false
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    bool m_ProcessRunning;                      //!< Process running state
    bool m_ShowMessageDialog;                   //!< To show Information Message Dialog
    ButtonType_t m_ButtonType;                  //!< Informs which button was clicked
    bool m_ColorReplaced;                       //!< Flag for program color change.
    QString m_ProgramID;                        //!< Program ID
    QModelIndex m_CurrentIndex;       ///<  Definition/Declaration of variable m_CurrentIndex
    bool m_UserChanged;       ///<  Definition/Declaration of variable m_UserChanged
    int m_UserProgramCount;                     //!< Count of User Programs
    DataManager::CUserSettings m_UserSettings;          //!< UserSettings object
    QString m_strEditProgram;       ///<  Definition/Declaration of variable m_strEditProgram
    QString m_strViewProgram;       ///<  Definition/Declaration of variable m_strViewProgram
    QString m_strNewProgram;       ///<  Definition/Declaration of variable m_strNewProgram
    QString m_strDelProgram;       ///<  Definition/Declaration of variable m_strDelProgram
    QString m_strView;       ///<  Definition/Declaration of variable m_strView
    QString m_strEdit;       ///<  Definition/Declaration of variable m_strEdit
    QString m_strConfirmMsg;       ///<  Definition/Declaration of variable m_strConfirmMsg
    QString m_strYes;       ///<  Definition/Declaration of variable m_strYes
    QString m_strCancel;       ///<  Definition/Declaration of variable m_strCancel


protected:
    void showEvent(QShowEvent *);

public:
    explicit CProgramWidget(Core::CDataConnector *p_DataConnector, MainMenu::CMainWindow *p_Parent = NULL,
                            KeyBoard::CKeyBoard *p_KeyBoard = NULL);
    ~CProgramWidget();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function IsFlagSet
     *
     *  \return from IsFlagSet
     */
    /****************************************************************************/
    bool IsFlagSet() { if(m_UserChanged == true) return true; else return false;}
    int GetNumberOfUserPrograms();

private:
    void PopulateProgramList();
    void ResizeHorizontalSection();
    void ResetButtons();
    void RetranslateUI();

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnEdit
     */
    /****************************************************************************/
    void OnEdit();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnNew
     */
    /****************************************************************************/
    void OnNew();
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
     *  \brief  Definition/Declaration of SelectionChanged
     */
    /****************************************************************************/
    void SelectionChanged(QModelIndex Index);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnUserRoleChanged
     */
    /****************************************************************************/
    void OnUserRoleChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnProcessStateChanged
     */
    /****************************************************************************/
    void OnProcessStateChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of CloseDialogs
     */
    /****************************************************************************/
    void CloseDialogs();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnCurrentTabChanged
     */
    /****************************************************************************/
    void OnCurrentTabChanged(int CurrentIndex);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnCancelClicked
     */
    /****************************************************************************/
    void OnCancelClicked();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of ProgramUpdated
     */
    /****************************************************************************/
    void ProgramUpdated();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of ProgramAdded
     */
    /****************************************************************************/
    void ProgramAdded();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of RevertChangedProgram
     */
    /****************************************************************************/
    void RevertChangedProgram();

protected:
    void changeEvent(QEvent *p_Event);

signals:
/****************************************************************************/
    /*!
     *  \brief Emit this Signal when Program Icon is updated
     */
    /****************************************************************************/
    void ProgramIconUpdated();
    /****************************************************************************/
    /*!
     *  \brief Emit this Signal when Favorite Program List is updated
     */
    /****************************************************************************/
    void FavoriteProgramListUpdated(DataManager::CProgram &);
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when the ProgramList is updated and informs
     *  Program model to update the program list data.
     */
    /****************************************************************************/
    void UpdateProgramList();

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
     *  \brief This signal is emitted when a program is deleted.
     *
     *  \iparam ProgramID = ProgramID of the selected program to be deleted.
     */
    /****************************************************************************/
    void DeleteProgram(QString &ProgramID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal UnselectProgram
     */
    /****************************************************************************/
    void UnselectProgram();
};

} // end namespace Programs

#endif // PROGRAMS_PROGRAMWIDGET_H

