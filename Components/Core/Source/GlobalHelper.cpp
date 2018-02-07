/****************************************************************************/
/*! \file GlobalHelper.cpp
 *
 *  \brief GlobalHelper implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-01-27
 *   $Author:  $ Abe Yang
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
#include <qglobal.h>
#include <QtWidgets/QApplication>
#include "Core/Include/GlobalHelper.h"
#include "MainMenu/Include/MessageDlg.h"
#include "Core/Include/DataConnector.h"

namespace Core {


QString CGlobalHelper::m_selectedProgramId = "";
QString CGlobalHelper::m_strMsgUnselect = "";
QString CGlobalHelper::m_strWarning = "";
QString CGlobalHelper::m_strYes = "";
QString CGlobalHelper::m_strCancel = "";
QString CGlobalHelper::SELECTED_PROGRAM_NAME = "";
bool CGlobalHelper::m_programIsPaused = false;
QStringList CGlobalHelper::m_StationList;
Core::CDataConnector* CGlobalHelper::p_StaticDataConnector = NULL;
bool CGlobalHelper::m_IsErrorHandlingFailed = false;
bool CGlobalHelper::m_IsSystemError = false;
bool CGlobalHelper::m_IsSystemBusy = false;


CGlobalHelper::CGlobalHelper(Core::CDataConnector *p_DataConnector) : QObject(),
    mp_DataConnector(p_DataConnector)
{
    if (p_StaticDataConnector == NULL) {
        p_StaticDataConnector = p_DataConnector;
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CGlobalHelper::~CGlobalHelper()
{
    try {
    }
    catch (...) {}
}

bool CGlobalHelper::CheckSelectedProgram(bool& bRevertSelectProgram,
                          QString operatedProgramID)
{
    RetranslateUI();  // init string

    bRevertSelectProgram = false;
    if (!m_selectedProgramId.isEmpty())
    {
        if (!operatedProgramID.isEmpty())//not empty
        {
            if (operatedProgramID != m_selectedProgramId)
                return true;
        }

        MainMenu::CMessageDlg ConfirmationMessageDlg;
        ConfirmationMessageDlg.SetTitle(m_strWarning);
        QString strMsg;
        strMsg = m_strMsgUnselect.arg(SELECTED_PROGRAM_NAME);
        ConfirmationMessageDlg.SetText(strMsg);
        ConfirmationMessageDlg.SetIcon(QMessageBox::Warning);
        ConfirmationMessageDlg.SetButtonText(1, m_strYes);
        ConfirmationMessageDlg.SetButtonText(3, m_strCancel);
        ConfirmationMessageDlg.HideCenterButton();
        if (!ConfirmationMessageDlg.exec())
            return false;//cancel
        else
            bRevertSelectProgram = true;

    }
    return true;
}


QString  CGlobalHelper::TimeToString(int seconds, bool showSeconds)
{
    QString retStr;

    int hour = seconds/(60*60);
    int minute = (seconds/60)%60;

    if (hour < 100)
        (void)retStr.sprintf("%02d:%02d", hour, minute);
    else
        (void)retStr.sprintf("%d:%02d", hour, minute);

    if (showSeconds)
    {
        int s = seconds%60;
        if ( s < 10)
            retStr.append(":0");
        else
            retStr.append(":");
        retStr.append(QString::number(s));
    }

    return retStr;
}

void CGlobalHelper::OnProgramSelected(QString& programId, QList<QString>& stationList)
{
    Q_UNUSED(stationList);
    m_selectedProgramId = programId;

    const DataManager::CProgram* pProgram = mp_DataConnector->ProgramList->GetProgram(m_selectedProgramId);
    if (pProgram)
    {
        SELECTED_PROGRAM_NAME = pProgram->GetName();
    }
}

QString CGlobalHelper::GetSelectedProgramId()
{
    return m_selectedProgramId;
}

void CGlobalHelper::UnselectProgram()
{
    if (!m_programIsPaused)
    {
        m_selectedProgramId = "";
        SELECTED_PROGRAM_NAME = "";
        m_StationList.clear();
    }
}

bool CGlobalHelper::CheckIfCanEdit(const QString& id, int checkType)
{
    if (p_StaticDataConnector == NULL) {
        return true;
    }

    if ( m_selectedProgramId.isEmpty() || !m_programIsPaused ) {
        return true;
    }

    switch (checkType){
    case 0: // no id
        if (!m_selectedProgramId.isEmpty() && m_programIsPaused) {
            return false;
        }
        break;
    case 1:  // program id
        if (m_selectedProgramId == id && m_programIsPaused) {
            return false;
        }
        break;
    case 2: { // reagent id
        const DataManager::CProgram* pProgram = p_StaticDataConnector->ProgramList->GetProgram(m_selectedProgramId);
        if (pProgram) {
            QStringList list = pProgram->GetReagentIDList();
            if (list.contains(id) && m_programIsPaused)
                return false;
        }
        break;
    }
    case 3: {  // reagent group id
        const DataManager::CProgram* pProgram = p_StaticDataConnector->ProgramList->GetProgram(m_selectedProgramId);
        if (pProgram) {
            QStringList list = pProgram->GetReagentIDList();
            for(int i=0; i<list.size(); i++) {
                QString reagentGroupId = p_StaticDataConnector->ReagentList->GetReagent(list.at(i))->GetGroupID();
                if (reagentGroupId == id && m_programIsPaused)
                    return false;
            }
        }
        break;
    }
    case 4: { // station id
        for(int i=0; i<m_StationList.size(); i++) {            
            if (m_StationList.at(i) == id && m_programIsPaused)
                return false;
        }
        break;
    }
    default:
        break;
    }

    return true;
}

void CGlobalHelper::SetStationList(const QStringList &list)
{
    m_StationList = list;
}

bool CGlobalHelper::GetProgramPaused()
{
    return m_programIsPaused;
}

void CGlobalHelper::SetProgramPaused(bool pauseFlag)
{
    m_programIsPaused = pauseFlag;
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CGlobalHelper::RetranslateUI()
{
    m_strWarning = QApplication::translate("Core::CGlobalHelper", "Warning", Q_NULLPTR, -1);
    m_strMsgUnselect = QApplication::translate("Core::CGlobalHelper", "Do you want to make your change?", Q_NULLPTR, -1);
    m_strYes = QApplication::translate("Core::CGlobalHelper", "Yes", Q_NULLPTR, -1);
    m_strCancel = QApplication::translate("Core::CGlobalHelper", "Cancel", Q_NULLPTR, -1);
}


bool CGlobalHelper::IsErrorHandlingFailed()
{
    return m_IsErrorHandlingFailed;
}

void CGlobalHelper::SetErrorHandlingFailed(bool bSet)
{
    m_IsErrorHandlingFailed = bSet;
}
void CGlobalHelper::SetSystemErrorStatus(bool isError)
{
    m_IsSystemError = isError;
}

bool CGlobalHelper::GetSystemErrorStatus()
{
    return m_IsSystemError;
}

void CGlobalHelper::SetSystemBusyStatus(bool isBusy)
{
    m_IsSystemBusy = isBusy;
}

bool CGlobalHelper::GetSystemBusyStatus()
{
    return m_IsSystemBusy;
}

QString CGlobalHelper::TrimText(const QFontMetrics& fontMetrics, const QString& processString,
                                int expectedWidth, bool bNeedEllips)
{

    QString ellips;
    if (bNeedEllips)
    {
        ellips = "...";
    }
    QString tempString(processString);
    int stringWidth = fontMetrics.width(tempString);
    while (stringWidth > expectedWidth)
    {
        tempString = tempString.left(tempString.length() - 1);
        stringWidth = fontMetrics.width(tempString + ellips);
    }
    return tempString + ellips;
}

}
