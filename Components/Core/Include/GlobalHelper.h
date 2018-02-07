/****************************************************************************/
/*! \file GlobalHelper.h
 *
 *  \brief GlobalHelper definition.
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

#ifndef CORE_GLOBALHELPER_H
#define CORE_GLOBALHELPER_H

#include <QObject>
#include <QFontMetrics>

namespace Core {
class CDataConnector;
/****************************************************************************/
/**
 * \brief This class supplies some helpful utilities.
 */
/****************************************************************************/
class CGlobalHelper : public QObject
{
    Q_OBJECT

public:
    /****************************************************************************/
    /*!
     *  \brief Constructor
     *  \param p_DataConnector = data connector object
     */
    /****************************************************************************/
    CGlobalHelper(Core::CDataConnector *p_DataConnector);
    virtual ~CGlobalHelper();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CheckSelectedProgram
     *
     *  \param bRevertSelectProgram
     *  \param operatedProgramID
     *
     *  \return from CheckSelectedProgram
     */
    /****************************************************************************/
    static bool CheckSelectedProgram(bool& bRevertSelectProgram,
                              QString operatedProgramID = "");
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function TimeToString
     *
     *  \param seconds = int type parameter
     *  \param showSeconds =  bool type parameter
     *
     *  \return from TimeToString
     */
    /****************************************************************************/
    static QString TimeToString(int seconds, bool showSeconds=false) ;
    static void RetranslateUI();
    /****************************************************************************/
    /*!
     *  \brief  by global helper to get program id
     *  \return the selected prorgam id
     */
    /****************************************************************************/
    static QString GetSelectedProgramId();
    /****************************************************************************/
    /*!
     *  \brief  by global helper to get program pause status
     *  \return the status value
     */
    /****************************************************************************/
    static bool GetProgramPaused();
    /****************************************************************************/
    /*!
     *  \brief  by global helper to get program pause status
     *  \param  pauseFlag = flag for whether pause or not
     *  \return the status value
     */
    /****************************************************************************/
    static void SetProgramPaused(bool pauseFlag);
    /****************************************************************************/
    /*!
     *  \brief unselect the program
     *
     */
    /****************************************************************************/
    static void UnselectProgram();
    /****************************************************************************/
    /*!
     *  \brief  by global helper to set all used station
     *  \param  list = store the used station Ids.
     */
    /****************************************************************************/
    static void SetStationList(const QStringList& list);
    /****************************************************************************/
    /*!
     *  \brief  by global helper to check whether something could be edit or not
     *  \param  name = program ID
     *  \param  CheckType = 0: No Id 1:Program Name 2: Reagent Name 3: Station 4:ReagentGroup
     *  \return the check value
     */
    /****************************************************************************/
    static bool CheckIfCanEdit(const QString& name = "", int CheckType = 0);
    /****************************************************************************/
    /*!
     *  \brief  by global helper to check whether ErrorHandling is failed or not
     *  \return the check value
     */
    /****************************************************************************/
    static bool IsErrorHandlingFailed();
    /****************************************************************************/
    /*!
     *  \brief  by global helper to set the status to m_ErrorHandlingFailed
     *  \param  bSet = set bool value
     */
    /****************************************************************************/
    static void SetErrorHandlingFailed(bool bSet);
    /****************************************************************************/
    /*!
     *  \brief  by global helper to set the error status
     *  \param  isError = set bool value
     */
    /****************************************************************************/
    static void SetSystemErrorStatus(bool isError);
    /****************************************************************************/
    /*!
     *  \brief  by global helper to get the system error status
     *
     *  \return the system error status.
     */
    /****************************************************************************/
    static bool GetSystemErrorStatus();
    /****************************************************************************/
    /*!
     *  \brief  by global helper to set the busy status
     *  \param  isBusy = set bool value
     */
    /****************************************************************************/
    static void SetSystemBusyStatus(bool isBusy);
    /****************************************************************************/
    /*!
     *  \brief  by global helper to get the system busy status
     *
     *  \return the system busy status.
     */
    /****************************************************************************/
    static bool GetSystemBusyStatus();
    /****************************************************************************/
    /*!
     *  \brief  by global helper to trim a string to the expected length
     *  \param  fontMetrics = QFontMetrics
     *  \param  processString = string to be processed
     *  \param  expectedWidth = the expected Width
     *  \param  bNeedEllips = whether need "..." or not
     *  \return the trimmed string.
     */
    /****************************************************************************/
    static QString TrimText(const QFontMetrics& fontMetrics, const QString& processString,
                            int expectedWidth, bool bNeedEllips = false);

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnProgramSelected
     */
    /****************************************************************************/
    void OnProgramSelected(QString&, QList<QString>&);
private:
    static QString m_selectedProgramId;
    static QString m_strMsgUnselect;
    static QString m_strWarning;
    static QString m_strCancel;
    static QString m_strYes;
    static QString SELECTED_PROGRAM_NAME;
    static bool m_programIsPaused;
    static QStringList m_StationList;
    static Core::CDataConnector *p_StaticDataConnector;
    Core::CDataConnector *mp_DataConnector;
    static bool m_IsErrorHandlingFailed;
    static bool m_IsSystemError;                //!< system is in Error state
    static bool m_IsSystemBusy;                //!<  system is in Busy state

};

} // end namespace Core

#endif // CORE_GLOBALHELPER_H

