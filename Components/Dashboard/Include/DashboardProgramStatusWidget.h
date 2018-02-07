#ifndef DASHBOARDPROGRAMSTATUS_H
#define DASHBOARDPROGRAMSTATUS_H

#include "MainMenu/Include/DialogFrame.h"
#include "Programs/Include/StepModel.h"

namespace MainMenu
{
    class CBaseTable;
}

namespace Core
{
    class CDataConnector;
}

namespace DataManager
{
    class CProgram ;
}

namespace Dashboard {

namespace Ui {
    class CDashboardProgramStatusWidget;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class Dashboard::CDashboardProgramStatusWidget
 */
/****************************************************************************/
class CDashboardProgramStatusWidget : public MainMenu::CDialogFrame
{
    Q_OBJECT
    
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CDashboardProgramStatusWidget
     *
     *  \param parent = QWidget type parameter
     *
     *  \return from CDashboardProgramStatusWidget
     */
    /****************************************************************************/
    explicit CDashboardProgramStatusWidget(QWidget *parent = 0);
    ~CDashboardProgramStatusWidget();
     /****************************************************************************/
     /*!
      *  \brief  Definition/Declaration of function InitDialog
      *
      *  \param p_Program
      *  \param pDataConnector
      *  \param StationNameList
      *  \param CurProgramStepIndex
      *  \param StepRemainingTime
      *  \param ProgramRemainingTime
      *  \param endDateTime
      *  \param bAbortButtonEnabled
      *  \param strStation
      *  \param strTemprature
      *  \return from InitDialog
      */
     /****************************************************************************/
     void InitDialog(DataManager::CProgram*, const Core::CDataConnector*, QList<QString>&, int, int,
                     int, const QString& endDateTime, bool bAbortButtonEnabled,
                     const QString& strStation,
                     const QString& strTemprature);
     /****************************************************************************/
     /*!
      *  \brief  Disable the Abort button
      *
      */
     /****************************************************************************/
     void DisableAbortButton();
protected:
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

private:
    Ui::CDashboardProgramStatusWidget *ui;       ///<  Definition/Declaration of variable ui
    MainMenu::CBaseTable *mp_TableWidget;       ///<  Definition/Declaration of variable mp_TableWidget
    Programs::CStepModel m_StepModel;       ///<  Definition/Declaration of variable m_StepModel
    void ResizeHorizontalSection();
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnClose
     */
    /****************************************************************************/
    void OnClose();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnAbort
     */
    /****************************************************************************/
    void OnAbort();
signals:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal AbortClicked
     */
    /****************************************************************************/
    void AbortClicked(int);
};
}
#endif // DASHBOARDPROGRAMSTATUS_H

