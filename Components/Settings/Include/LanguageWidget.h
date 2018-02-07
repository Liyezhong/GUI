/****************************************************************************/
/*! \file LanguageWidget.h
 *
 *  \brief LanguageWidget definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-29
 *   $Author:  $ M.Scherer, C.Adaragunchi
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

#ifndef SETTINGS_LANGUAGEWIDGET_H
#define SETTINGS_LANGUAGEWIDGET_H

#include "HimalayaDataContainer/Containers/UserSettings/Include/HimalayaUserSettings.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/PanelFrame.h"
#include "Settings/Include/LanguageModel.h"
#include "Core/Include/DataConnector.h"

namespace Settings {

namespace Ui {
    class CLanguageWidget;
}

/****************************************************************************/
/**
 * \brief This class contains the language selection widget
 */
/****************************************************************************/
class CLanguageWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

    friend class CTestSettings;
private:
    Ui::CLanguageWidget *mp_Ui;                     //!< User interface
    MainMenu::CBaseTable *mp_TableWidget;           //!< Language table
    DataManager::CHimalayaUserSettings *mp_UserSettings;    //!< Data object
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    //Flags
    bool m_ProcessRunning;                          //!< Process running state
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    Settings::CLanguageModel m_LanguageModel;       //!< Language Model object
    Core::CDataConnector *mp_DataConnector;         //!< DataConnector object
    QStringList m_LanguageList;                     //!< List of Languages
    QString m_SelectedLanguage;                     //!< Currently selected language
    QHash<QString,QString> m_LanguageNativeNames;   //!< Hash of Languages and thier native names
    QStringList m_LanguageNativeNameList;           //!< List of native language names

public:
    explicit CLanguageWidget(QWidget *p_Parent = NULL);
    ~CLanguageWidget();
    void SetUserSettings(DataManager::CHimalayaUserSettings *p_UserSettings);
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow, Core::CDataConnector *p_DataConnector);
    void SetLanguages(QStringList LanguageList);

private:
    void PopulateLanguageList();
    void RetranslateUI();
    void ResetButtons();
    void InitDialog();

protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnBtnApplyClicked
     */
    /****************************************************************************/
    void OnBtnApplyClicked();
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
     *  \brief  Definition/Declaration of SelectionChanged
     */
    /****************************************************************************/
    void SelectionChanged(QModelIndex Index);

signals:
    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when the langauge is changed.
     *
     *  \iparam UserSettings = UserSettings with selected language.
     */
    /****************************************************************************/
    void UserSettingsChanged(DataManager::CUserSettings &UserSettings);

};

} // end namespace Settings

#endif // SETTINGS_LANGUAGEWIDGET_H

