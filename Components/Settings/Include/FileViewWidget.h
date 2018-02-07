/****************************************************************************/
/*! \file FileViewWidget.h
 *
 *  \brief FileViewWidget definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-12-16, 2012-11-07
 *   $Author:  $ N.Kamath, Raju
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

#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/FileView.h"
#include "Global/Include/Exception.h"

namespace Settings {

/****************************************************************************/
/**
 * \brief This class contains the Event View object
 */
/****************************************************************************/
class CFileViewWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

    friend class CTestSettings;
public:
    explicit CFileViewWidget(QWidget *p_Parent = 0);
    void SetPanelSize(const qint32 &Width, const qint32 &Height);
    void DisableButton();
    ~CFileViewWidget();

private:
    MainMenu::CFileView *mp_FileView;   //!< Content of this widget
    void RetranslateUI();

protected:
    void changeEvent(QEvent *p_Event);
signals:
    /****************************************************************************/
    /**
     * \brief This class contains the Event View object
     *
     * \param[in] FileName = Name of the file
     */
    /****************************************************************************/
    void SelectedLogFile(const QString &FileName);

    /****************************************************************************/
    /**
     * \brief Content of the Daily run file which is selected by the user
     *
     * \param[in] DataStream = Data stream for the file content
     */
    /****************************************************************************/
    void DayRunLogFileContent(const QDataStream &DataStream);

    /****************************************************************************/
    /**
     * \brief List of daily run log files
     *
     * \param[in] FileNames = List of the files
     */
    /****************************************************************************/
    void DayRunLogFileNames(const QStringList &FileNames);


};

}//End of nameSpace Settnigs
#endif // FILEVIEWWIDGET_H

