/****************************************************************************/
/*! \file ModifyReagentRMSDlg.h
 *
 *  \brief ModifyReagentDlg definition.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2011-08-22, 2013-01-17
 *   $Author:  $ M.Scherer, C.Adaragunchi, Swati Tiwari
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

#ifndef REAGENTS_MODIFYREAGENTRMSDLG_H
#define REAGENTS_MODIFYREAGENTRMSDLG_H

#include "HimalayaDataContainer/Containers/Reagents/Include/Reagent.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "KeyBoard/Include/KeyBoard.h"
#include "Reagents/Include/ReagentRMSWidget.h"
#include "Core/Include/ReagentGroupModel.h"

//lint -sem(Reagents::CModifyReagentRMSDlg::Init,initializer)
//lint -e1565
namespace Reagents {
        #define MAX_LONGNAME_LENGTH 32  //!< Maximum length reagent long name
        #define MIN_LONGNAME_LENGTH 2   //!< Manimum length reagent long name

namespace Ui {
    class CModifyReagentRMSDlg;
}


/****************************************************************************/
/**
 * \brief Edit dialog for  reagents of the user
 */
/****************************************************************************/
class CModifyReagentRMSDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class CTestReagents;

private:
    Ui::CModifyReagentRMSDlg *mp_Ui;                   //!< User interface
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;         //!< Reference to Keyboard widget
    Core::CDataConnector *mp_DataConnector;         //!< DataConnector local object
    DataManager::CReagent m_Reagent;                //!< Local reagent object    
    bool m_ReagentNameBtnClicked;                   //!< Reagent Name btn clicked status
    bool m_CassetteValueBtnClicked;                 //!< Reagent cassette value btn clicked status
    Reagents::ButtonType_t m_ButtonType;            //!< Informs which button was clicked
    bool m_ProcessRunning;                          //!< Process running state
    DataManager::CDataReagentList m_ReagentCloneList;   //!< ReagentList local object
    MainMenu::CMessageDlg* mp_MessageDlg;             //!< Message dialog object
    MainMenu::CBaseTable *mp_TableWidget;           //!< Reagent table
    Core::CReagentGroupModel m_ReagentGroupModel;             //!< Model for the table
    DataManager::CDataReagentGroupList *mp_ReagentGroupList;  //!< Reagent list
    Global::RMSOptions_t m_RMSOption;       ///<  Definition/Declaration of variable m_RMSOption
    bool m_SelectionFlag;       ///<  Definition/Declaration of variable m_SelectionFlag
    QString m_strCassettesUntilChange;       ///<  Definition/Declaration of variable m_strCassettesUntilChange
    QString m_strCyclesUntilChange;       ///<  Definition/Declaration of variable m_strCyclesUntilChange
    QString m_strDaysUntilChange;       ///<  Definition/Declaration of variable m_strDaysUntilChange
    QString m_strEnterValidName;       ///<  Definition/Declaration of variable m_strEnterValidName
    QString m_strReagentNameHasLaicaString;       ///<  Definition/Declaration of variable m_strReagentNameHasLaicaString
    QString m_strInputReagentSameName;       ///<  Definition/Declaration of variable m_strInputReagentSameName
    QString m_strEnterValidData;       ///<  Definition/Declaration of variable m_strEnterValidData
    QString m_strSelectReagentGroup;       ///<  Definition/Declaration of variable m_strSelectReagentGroup
    QString m_strReagentAddFailed;       ///<  Definition/Declaration of variable m_strReagentAddFailed
    QString m_strReagentCopyFailed;       ///<  Definition/Declaration of variable m_strReagentCopyFailed
    QString m_strEnterCassetteValue;       ///<  Definition/Declaration of variable m_strEnterCassetteValue
    QString m_strEnterCycleValue;       ///<  Definition/Declaration of variable m_strEnterCycleValue
    QString m_strEnterDayValue;       ///<  Definition/Declaration of variable m_strEnterDayValue
    QString m_strInforMsg;       ///<  Definition/Declaration of variable m_strInforMsg
    QString m_strLastReagentName;  ///<  Definition/Declaration of variable m_strLastReagentName
    QString m_strOK;       ///<  Definition/Declaration of variable m_strOK


public:
    explicit CModifyReagentRMSDlg(QWidget *p_Parent = NULL, KeyBoard::CKeyBoard *p_KeyBoard = NULL,
                               MainMenu::CMainWindow *p_MainWindow = NULL,
                               Core::CDataConnector *p_DataConnector= NULL);
    virtual ~CModifyReagentRMSDlg();
    void InitDialog(DataManager::CReagent const *p_Reagent,
                    const DataManager::CDataReagentGroupList *p_ReagentGroupList,
                    Global::RMSOptions_t Option,
                    bool bShowCleaningReagent = true);
    /****************************************************************************/
    /*!
     *  \brief Update Rms label
     *  \param Option = rms option
     */
    /****************************************************************************/
    void UpdateRmsLabel(Global::RMSOptions_t Option);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function EnableElements
     *
     *  \param m_Enable = bool type parameter
     *
     */
    /****************************************************************************/
    void EnableElements(bool m_Enable);

    /****************************************************************************/
    /*!
     *  \brief Informs which button was clicked on Menu reagent screen
     *
     *  \iparam ButtonType - Edit/New/Copy
     */
    /****************************************************************************/
    void SetButtonType(ButtonType_t ButtonType) { m_ButtonType = ButtonType;}

private:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Init
     *
     *  \return from Init
     */
    /****************************************************************************/
    void Init();
    void RetranslateUI();
    void ResizeHorizontalSection();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ShowReagentValue
     *
     *  \param Option = Global::RMSOptions_t type parameter
     *
     *  \return from ShowReagentValue
     */
    /****************************************************************************/
    void ShowReagentValue(Global::RMSOptions_t Option);
    void ConnectKeyBoardSignalSlots();
    void DisconnectKeyBoardSignalSlots();

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
     *  \brief  Definition/Declaration of OnEditName
     */
    /****************************************************************************/
    void OnEditName();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnEditCassetteValue
     */
    /****************************************************************************/
    void OnEditCassetteValue();
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
     *  \brief  Definition/Declaration of SelectionChanged
     *  \iparam Index index
     */
    /****************************************************************************/
    void SelectionChanged(QModelIndex Index);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of HandleEscapedChar
     *  \iparam str str to handle
     *  \return handled str
     */
    /****************************************************************************/
    QString HandleEscapedChar(QString& str);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnUserRoleChanged
     */
    /****************************************************************************/
    void OnUserRoleChanged();

protected:
    void changeEvent(QEvent *p_Event);

signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted to update the selected reagent.
     *
     *  \iparam Reagent = Selected reagent.
     */
    /****************************************************************************/
    void UpdateReagent(DataManager::CReagent &Reagent);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted to add new reagent.
     *
     *  \iparam Reagent = New reagent.
     */
    /****************************************************************************/
    void AddReagent(DataManager::CReagent &Reagent);

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when Cancel button in the dailog is pressed.
     *
     */
    /****************************************************************************/
    void CancelPressed();
};

} // end namespace Reagents

#endif // REAGENTS_MODIFYREAGENTDLG_H

