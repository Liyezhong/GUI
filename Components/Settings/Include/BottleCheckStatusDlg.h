/****************************************************************************/
/*! \file BottleCheckStatusDlg.h
 *
 *  \brief BottleCheckStatusDlg definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2015-05-20
 *   $Author:  $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2015 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef BOTTLECHECKSTATUSDLG_H
#define BOTTLECHECKSTATUSDLG_H

#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/BaseTable.h"
#include "Settings/Include/BottleCheckStatusModel.h"

namespace Core
{
    class CDataConnector;
}

namespace Settings {

namespace Ui {
    class CBottleCheckStatusDlg;
}
/****************************************************************************/
/**
 * \brief This is the dialog for bottle check status
 */
/****************************************************************************/
class CBottleCheckStatusDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT
    
public:
    /****************************************************************************/
    /*!
     *  \brief constructor
     *
     *  \param parent = pointers to the parent window
     *  \param pDataCon = Data Connector
     */
    /****************************************************************************/
    explicit CBottleCheckStatusDlg(QWidget *parent, Core::CDataConnector* pDataCon);
    ~CBottleCheckStatusDlg();
    void UpdateCurrentStationStatus(const QString& stationID, DataManager::BottleCheckStatusType_t bottleCheckStatusType);
protected:
    void changeEvent(QEvent *p_Event);
    virtual void keyPressEvent ( QKeyEvent * e );
private slots:
    void OnClose();
private:
    void RetranslateUI();
    void Inilialize();
    void UpdateStationNotProcess(const QString& stationID);
    Ui::CBottleCheckStatusDlg *ui;
    MainMenu::CBaseTable *mp_TableWidget;
    Settings::CBottleCheckStatusModel m_BottleCheckStatusModel;
    QString m_Started;
    QString m_Waiting;
    QString m_Empty;
    QString m_Passed;
    QString m_Blockage;
    QString m_Checking;
    QString m_WillNotProcess;
    QString m_BuildPressureFailed;
    QString m_Failed;
    QMap<DataManager::BottleCheckStatusType_t, QString> m_BottleCheckStatusMap;
    QMap<QString, int> m_StationIDRowMap;
    QMap<int, QString> m_RowStationIDMap;
    QMap<QString, QString> m_StationStatusMap;
    bool m_bNoneUpdated;
    bool m_bShouldReportError;
    QString m_strReportMsg;
    QString m_strWarning;
    QString m_strOK;
};
}
#endif // BOTTLECHECKSTATUSDLG_H
