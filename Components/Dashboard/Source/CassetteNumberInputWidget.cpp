#include "Dashboard/Include/CassetteNumberInputWidget.h"
#include "ui_CassetteNumberInputWidget.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/MessageDlg.h"
#include <QRect>

using namespace Dashboard;

CCassetteNumberInputWidget::CCassetteNumberInputWidget(QWidget *pParent, QWidget *pMainWindow) :
    MainMenu::CDialogFrame(pParent, pMainWindow),
    ui(new Ui::CCassetteNumberInputWidget),
    m_CassetteNumber(0),
    m_strWarning(tr("Warning")),
    m_strOK(tr("OK")),
    m_CurrentTotalCassette(0)

{
    ui->setupUi(GetContentFrame());
    RetranslateUI();//As the overrided function changeEvent(QEvent *p_Event) cannot be revoked in this class, we should call it here

    ui->totalcassettelbl->setText(m_strTotalCassette);
    ui->inputhintlbl->setText(m_strInputCassetteHint);

    ShowLabel(false);

    this->setWindowModality(Qt::ApplicationModal);
    CONNECTSIGNALSLOT(ui->btnOK, clicked(), this, OnOK());
    CONNECTSIGNALSLOT(ui->btnCancel, clicked(), this, OnCancel());


    mp_singleWheel = new MainMenu::CScrollWheel();
    mp_tenWheel = new MainMenu::CScrollWheel();
    mp_hundredWheel = new MainMenu::CScrollWheel();

    for (int i = 0; i <= 9; i++) {
        mp_singleWheel->AddItem(QString("%1").arg(i, 1, 10, QChar('0')), i);
    }

    for (int i = 0; i <= 9; i++) {
        mp_tenWheel->AddItem(QString("%1").arg(i, 1, 10, QChar('0')), i);
    }

    for (int i = 0; i <= 2; i++) {
        mp_hundredWheel->AddItem(QString("%1").arg(i, 1, 10, QChar('0')), i);
    }

    ui->scrollPanelCassetteNumber->Init(3);
    ui->scrollPanelCassetteNumber->AddScrollWheel(mp_hundredWheel, 0);
    ui->scrollPanelCassetteNumber->AddScrollWheel(mp_tenWheel, 1);
    ui->scrollPanelCassetteNumber->AddScrollWheel(mp_singleWheel, 2);
}

CCassetteNumberInputWidget::~CCassetteNumberInputWidget()
{
    try
    {
        delete ui;
        delete mp_singleWheel;
        delete mp_tenWheel;
        delete mp_hundredWheel;
    }
    catch(...)
    {}
}

void CCassetteNumberInputWidget::OnOK()
{
    int hundred = mp_hundredWheel->GetCurrentData().toInt() * 100;
    int ten = mp_tenWheel->GetCurrentData().toInt() * 10;
    m_CassetteNumber = hundred + ten + mp_singleWheel->GetCurrentData().toInt();
    if (m_CassetteNumber == 0 || m_CassetteNumber > 200 || (m_CurrentTotalCassette + m_CassetteNumber) > 200)
    {
          MainMenu::CMessageDlg* pMessageDlg = new MainMenu::CMessageDlg();
          pMessageDlg->SetIcon(QMessageBox::Warning);
          pMessageDlg->SetTitle(m_strWarning);
          pMessageDlg->SetText(m_strMsg);
          pMessageDlg->SetButtonText(1, m_strOK);
          pMessageDlg->HideButtons(); 

          if (pMessageDlg->exec())
          {
              delete pMessageDlg;
              return;
          }
          delete pMessageDlg;
    }
    (void)this->close();
}

void CCassetteNumberInputWidget::OnCancel()
{
    m_CassetteNumber = -1;
    (void)this->close();
}

int CCassetteNumberInputWidget::CassetteNumber()
{
    return m_CassetteNumber;
}

void CCassetteNumberInputWidget::ShowLabel(bool on)
{
    if (on) {
        ui->totalcassettelbl->show();
        ui->inputhintlbl->show();
    }
    else {
        ui->totalcassettelbl->hide();
        ui->inputhintlbl->hide();
    }
}

void CCassetteNumberInputWidget::SetCurrentCassette(int count)
{
    QString text = m_strTotalCassette;
    text += QString::number(count);
    ui->totalcassettelbl->setText(text);
    m_CurrentTotalCassette = count;
}

void CCassetteNumberInputWidget::RetranslateUI()
{
    m_strMsg = QApplication::translate("CCassetteNumberInputWidget", "The total number of cassettes should be between 1 and 200.", Q_NULLPTR, -1);
    m_strWarning = QApplication::translate("CCassetteNumberInputWidget", "Warning", Q_NULLPTR, -1);
    m_strOK = QApplication::translate("CCassetteNumberInputWidget", "OK", Q_NULLPTR, -1);
    m_strTotalCassette = QApplication::translate("CCassetteNumberInputWidget", "Original cassette number: ", Q_NULLPTR, -1);
    m_strInputCassetteHint = QApplication::translate("CCassetteNumberInputWidget", "Add more cassettes:", Q_NULLPTR, -1);
}

