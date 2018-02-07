#include "Dashboard/Include/ScreenSaverWidget.h"
#include "ui_ScreenSaverWidget.h"
#include <QTimer>
#include "MainMenu/Include/MainWindow.h"
#include "Core/Include/GlobalHelper.h"

ScreenSaverWidget::ScreenSaverWidget(QWidget *p) :
    QWidget(p),
    ui(new Ui::ScreenSaverWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    this->move(0, 0);
    m_timer = new QTimer(this);
    m_timer->setInterval(1800000);//, here should be 30 mintues, 1800000
    (void)connect(m_timer, SIGNAL(timeout()), this, SLOT(AppIdleForLongTime()));
    m_timer->start();
    ui->lblWarning->setVisible(false);
}

ScreenSaverWidget::~ScreenSaverWidget()
{
    try
    {
        delete ui;
        delete m_timer;
    }
    catch(...)
    {}
}

bool ScreenSaverWidget::event ( QEvent * event )
{
    if (event->type() == QEvent::WindowBlocked)
    {
        OnInteractStart();
    }
    return QWidget::event(event);
}

void ScreenSaverWidget::AppIdleForLongTime()
{
    m_timer->stop();

    if (MainMenu::CMainWindow::GetProcessRunningStatus() || Core::CGlobalHelper::GetProgramPaused())
    {
        ui->lblWarning->setVisible(true);
    }
    else
    {
        ui->lblWarning->setVisible(false);
    }

    this->show();
}

void ScreenSaverWidget::OnInteractStart()
{
    m_timer->stop();
    m_timer->start();

    if (this->isVisible())
    this->hide();
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void ScreenSaverWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
