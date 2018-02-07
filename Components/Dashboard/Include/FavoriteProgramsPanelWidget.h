#ifndef FAVORITEPROGRAMSPANELWIDGET_H
#define FAVORITEPROGRAMSPANELWIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QMap>
#include <QDateTime>
#include "Dashboard/Include/ProgramLabel.h"

#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"

class QLabel;

namespace MainMenu
{
    class CMainWindow ;
}

namespace Core
{
    class CDataConnector;
}

namespace DataManager
{
    class CDataConnector;
    class CDataProgramList;
}

namespace Dashboard {
    namespace Ui {
    class CFavoriteProgramsPanelWidget;
    }

    class CStackedLabel;

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class Dashboard::CFavoriteProgramsPanelWidget
 */
/****************************************************************************/
    class CFavoriteProgramsPanelWidget : public QWidget
    {
        Q_OBJECT

    public:
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of function CFavoriteProgramsPanelWidget
         *
         *  \param parent = QWidget type parameter
         *
         *  \return from CFavoriteProgramsPanelWidget
         */
        /****************************************************************************/
        explicit CFavoriteProgramsPanelWidget(QWidget *parent = 0);
        ~CFavoriteProgramsPanelWidget();
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
         *  \brief  Definition/Declaration of function ResetInFavProgramButtonClicked
         *
         *  \return from ResetInFavProgramButtonClicked
         */
        /****************************************************************************/
        void ResetInFavProgramButtonClicked();
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of function SetInFavProgramButtonClicked
         *
         *  \return from SetInFavProgramButtonClicked
         */
        /****************************************************************************/
        void SetInFavProgramButtonClicked();
        static QString SELECTED_PROGRAM_NAME;       ///<  Definition/Declaration of variable SELECTED_PROGRAM_NAME

    public slots:
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of AddItemsToFavoritePanel
         *  \param bOnlyAddCleaningProgram = whether or not only show cleaning program
         */
        /****************************************************************************/
        void AddItemsToFavoritePanel(bool bOnlyAddCleaningProgram);
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of AddItemsToFavoritePanel
         *
         */
        /****************************************************************************/
        void AddItemsToFavoritePanel();
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
    private slots:
        void OnResetFocus(bool reset);
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of OnFavProgramButtonClicked
         */
        /****************************************************************************/
        void OnFavProgramButtonClicked(int);
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of OnProcessStateChanged
         */
        /****************************************************************************/
        void OnProcessStateChanged();
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of UndoProgramSelection
         */
        /****************************************************************************/
        void UndoProgramSelection();
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of UpdateProgram
         */
        /****************************************************************************/
        void UpdateProgram(DataManager::CProgram &Program);
    signals:
        /****************************************************************************/
        /*!
         *  \brief  Declaration of signal Checking the selected program
         *  \param selectedProgramId = the selected program id
         */
        /****************************************************************************/
        void PrepareSelectedProgramChecking(const QString& selectedProgramId);
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of signal OnSelectEndDateTime
         *  \param  endDateTime = end date time for selected program
         */
        /****************************************************************************/
        void OnSelectEndDateTime(const QDateTime & endDateTime);


        /****************************************************************************/
        /*!
         *  \brief  Declaration of signal for update the favorite Program
         */
        /****************************************************************************/
        void UpdateFavProgram();
    private:
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of function RetranslateUI
         *
         *  \return from RetranslateUI
         */
        /****************************************************************************/
        void RetranslateUI();
        /****************************************************************************/
        /*!
         *  \brief  Definition/Declaration of function SetButtonGroup
         *
         *  \return from SetButtonGroup
         */
        /****************************************************************************/
        void SetButtonGroup();

        QMap<int, CStackedLabel*> m_mapLabel;///< (label Id <--> QLabel)
        Ui::CFavoriteProgramsPanelWidget *ui;       ///<  Definition/Declaration of variable ui
        QButtonGroup m_ButtonGroup;       ///<  Definition/Declaration of variable m_ButtonGroup
        Core::CDataConnector *mp_DataConnector;                     //!< Global data container
        int m_LastCanBeSelectedButtonId;
        QDateTime m_lastSetAsapDatetime;
        DataManager::CDataProgramList* mp_ProgramList;       ///<  Definition/Declaration of variable mp_ProgramList
        bool m_ProcessRunning;       ///<  Definition/Declaration of variable m_ProcessRunning
        int m_LastSelectedButtonId;       ///<  Definition/Declaration of variable m_LastSelectedButtonId
        QDateTime m_ProgramEndDateTime;       ///<  Definition/Declaration of variable m_ProgramEndDateTime
        QStringList m_FavProgramIDs;       ///<  Definition/Declaration of variable m_FavProgramIDs
        QString m_NewSelectedProgramId;       ///<  Definition/Declaration of variable m_NewSelectedProgramId
        QDateTime m_EndDateTime;       ///<  Definition/Declaration of variable m_EndDateTime
        bool m_OnlyAddCleaningProgram;
        bool m_IsInFavProgramButtonClicked;

    };
}
#endif // FAVORITEPROGRAMSPANELWIDGET_H

