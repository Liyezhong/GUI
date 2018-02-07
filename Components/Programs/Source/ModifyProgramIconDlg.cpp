
/****************************************************************************/
/*! \file ModifyReagentGroupColorDlg.cpp
 *
 *  \brief ModifyReagentGroupColorDlg Implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-03-15
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "Programs/Include/ModifyProgramIconDlg.h"
#include "ui_ModifyProgramIconDlg.h"


namespace Programs {
/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 *  \param p_MainWindow =  MainMenu::CMainWindow type parameter
 */
/****************************************************************************/
CModifyProgramIconDlg::CModifyProgramIconDlg(QWidget *p_Parent, MainMenu::CMainWindow *p_MainWindow) :
    MainMenu::CDialogFrame(p_Parent, p_MainWindow), mp_Ui(new Ui::CModifyProgramIconDlg),
    m_strConfirmMsg(tr("Information Message")),
    m_strOK(tr("OK")), m_strCancel(tr("Cancel")), m_strClose(tr("Close"))
{
    Q_UNUSED(p_MainWindow)
    mp_Ui->setupUi(GetContentFrame());
    m_ProcessRunning = false ;
    SetButtonGroup();

    QString strName("Icon");
    for (int i = 0; i < m_ButtonGroup.buttons().count(); i++)
    {
        strName.append(QString::number(i));
        m_IconName.push_back(strName);
        m_ButtonGroup.button(i)->setIcon(QIcon(":/HimalayaImages/Icons/Program/"+strName+".png"));
        m_ButtonGroup.button(i)->setIconSize(QSize(71, 62));
        strName = strName.left(4);
    }

    mp_Ui->btnCancel->setText(m_strCancel);
    mp_Ui->btnOk->setText(m_strOK);

    // Connecting Signal and Slots
    CONNECTSIGNALSLOT(&m_ButtonGroup, buttonClicked(int), this, OnIconGroup(int));
    CONNECTSIGNALSLOT(mp_Ui->btnCancel, clicked(), this, OnCancel());
    CONNECTSIGNALSLOT(mp_Ui->btnOk, clicked(), this, OnOk());
    m_MessageDlg.SetTitle(m_strConfirmMsg);
    m_MessageDlg.SetIcon(QMessageBox::Information);
    m_MessageDlg.SetButtonText(1, m_strOK);
    m_MessageDlg.HideButtons();
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  Function to create Map of PushButton and Creation of Button Group.
 */
/****************************************************************************/
void CModifyProgramIconDlg ::SetButtonGroup()
{
        m_ButtonGroup.addButton(mp_Ui->IconButton_1, 0);
        m_ButtonGroup.addButton(mp_Ui->IconButton_2, 1);
        m_ButtonGroup.addButton(mp_Ui->IconButton_3, 2);
        m_ButtonGroup.addButton(mp_Ui->IconButton_4, 3);
        m_ButtonGroup.addButton(mp_Ui->IconButton_5, 4);
        m_ButtonGroup.addButton(mp_Ui->IconButton_6, 5);

        m_ButtonGroup.addButton(mp_Ui->IconButton_7, 6);
        m_ButtonGroup.addButton(mp_Ui->IconButton_8, 7);
        m_ButtonGroup.addButton(mp_Ui->IconButton_9, 8);
        m_ButtonGroup.addButton(mp_Ui->IconButton_10, 9);
        m_ButtonGroup.addButton(mp_Ui->IconButton_11, 10);
        m_ButtonGroup.addButton(mp_Ui->IconButton_12, 11);
}
/****************************************************************************/
/*!
     *  \brief Destructor
     */
/****************************************************************************/
CModifyProgramIconDlg::~CModifyProgramIconDlg()
{
    delete mp_Ui;
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CModifyProgramIconDlg::changeEvent(QEvent *p_Event)
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
 *  \brief This slot is called when Cancel button in ModifyReagentDlg is pressed.
 */
/****************************************************************************/
void CModifyProgramIconDlg::OnCancel()
{
    emit CancelPressed();
    reject();
}

/****************************************************************************/
/*!
 *  \brief Saves the changes in the dialog
 */
/****************************************************************************/
void CModifyProgramIconDlg::OnOk()
{
    emit UpdateProgramIcon(&m_Program);
    accept();
}
/****************************************************************************/
/*!
 *  \brief Selects the Icon in the Widget
 *
 *  \param Id = int type parameter
 */
/****************************************************************************/
 void CModifyProgramIconDlg::OnIconGroup(int Id)
 {
     mp_Ui->btnOk->setEnabled(true);
     m_Program.SetIcon(m_IconName.value(Id));
 }

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CModifyProgramIconDlg::OnProcessStateChanged()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if (!m_ProcessRunning) {
        //Edit Mode
        mp_Ui->btnOk->setEnabled(true);
        mp_Ui->btnCancel->setText(m_strCancel);
    }
    else {
        //View Mode
        mp_Ui->btnOk->setEnabled(false);
        mp_Ui->btnCancel->setText(m_strClose);
    }
}

void CModifyProgramIconDlg::EnableAvailableIcon(DataManager::CDataProgramList* pProgramList)
{
    //enable all icons
    for (int i = 0; i < m_ButtonGroup.buttons().count(); i++)
    {
        m_ButtonGroup.button(i)->setEnabled(true);
        m_ButtonGroup.setExclusive(false);
        m_ButtonGroup.button(i)->setChecked(false);
        m_ButtonGroup.setExclusive(true);
    }

    //loop all used icons, disable it
    int count = pProgramList->GetNumberOfPrograms();
    for (int j = 0; j< count; j++)
    {
        if (m_IconName.contains(pProgramList->GetProgram(j)->GetIcon()))
        {
            int idx = m_IconName.indexOf(pProgramList->GetProgram(j)->GetIcon());
            m_ButtonGroup.button(idx)->setEnabled(false);
        }
    }
    mp_Ui->btnOk->setEnabled(false);
}

void CModifyProgramIconDlg::RetranslateUI()
{
    m_strConfirmMsg = QApplication::translate("Programs::CModifyProgramIconDlg", "Information Message", Q_NULLPTR, -1);
    m_strOK = QApplication::translate("Programs::CModifyProgramIconDlg", "OK", Q_NULLPTR, -1);
    m_strCancel = QApplication::translate("Programs::CModifyProgramIconDlg", "Cancel", Q_NULLPTR, -1);
    m_strClose = QApplication::translate("Programs::CModifyProgramIconDlg", "Close", Q_NULLPTR, -1);
}
}
