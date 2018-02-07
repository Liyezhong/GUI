#ifndef SCREENSAVERWIDGET_H
#define SCREENSAVERWIDGET_H

#include "MainMenu/Include/PanelFrame.h"

namespace Ui {
class ScreenSaverWidget;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class ScreenSaverWidget
 */
/****************************************************************************/
class ScreenSaverWidget : public QWidget
{
    Q_OBJECT
    
public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ScreenSaverWidget
     *
     *  \param parent
     *
     *  \return from ScreenSaverWidget
     */
    /****************************************************************************/
    explicit ScreenSaverWidget(QWidget *parent = 0);
    ~ScreenSaverWidget();
protected:
    /****************************************************************************/
    /*!
     *  \brief  Declaration of function event
     *
     *  \param event = Reimplemented from QWidget::event().
     *
     *  \returns true if the event was recognized, otherwise it returns false.
     */
    /****************************************************************************/
    virtual bool event ( QEvent * event );

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
    Ui::ScreenSaverWidget *ui;       ///<  Definition/Declaration of variable ui
    QTimer* m_timer;       ///<  Definition/Declaration of variable m_timer
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of AppIdleForLongTime
     */
    /****************************************************************************/
    void AppIdleForLongTime();

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of OnInteractStart
     */
    /****************************************************************************/
    void OnInteractStart();
};

#endif // SCREENSAVERWIDGET_H

