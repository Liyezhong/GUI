#include "Dashboard/Include/ProgramPanelWidget.h"
#include "ui_ProgramPanelWidget.h"

namespace Dashboard {
CProgramPanelWidget::CProgramPanelWidget(QWidget *parent) :
    CPanelFrame(parent),
    ui(new Ui::CProgramPanelWidget)
{
    ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("Programs"));
    connect(ui->pushButton, SIGNAL(clicked(bool)),
                this, SLOT(on_pushButton_clicked()));

    connect(ui->pushButton_2, SIGNAL(clicked(bool)),
                this, SLOT(on_pushButton_2_clicked()));
}

CProgramPanelWidget::~CProgramPanelWidget()
{
    try {
            delete ui;
        }
        catch (...) {
            // to please Lint.
        }
}

void CProgramPanelWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

void CProgramPanelWidget::RetranslateUI()
{
    SetPanelTitle(QApplication::translate("Dashboard::CProgramPanelWidget", "Programs",
                                                                 0, QApplication::UnicodeUTF8));
}

void CProgramPanelWidget::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->programRunningPanel->SetPanelTitle(tr("Test"));
}

void CProgramPanelWidget::on_pushButton_2_clicked()
{
   ui->stackedWidget->setCurrentIndex(0);
}

}
