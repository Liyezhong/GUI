#include "Dashboard/Include/CommonString.h"
#include <QApplication>

namespace CommonString
{
    QString strOK;
    QString strYes;
    QString strNo;
    QString strClose;
    QString strInforMsg;
    QString strCancel;
    QString strConfirmMsg;
    QString strWarning;
    void RetranslateUIString()
    {
        strOK = QApplication::translate("CommonString", "OK", Q_NULLPTR, -1);
        strYes = QApplication::translate("CommonString", "Yes", Q_NULLPTR, -1);
        strNo  =  QApplication::translate("CommonString", "No", Q_NULLPTR, -1);
        strClose = QApplication::translate("CommonString", "Close", Q_NULLPTR, -1);
        strCancel = QApplication::translate("CommonString", "Cancel", Q_NULLPTR, -1);
        strInforMsg = QApplication::translate("CommonString", "Information Message", Q_NULLPTR, -1);
        strConfirmMsg = QApplication::translate("CommonString", "Confirmation Message", Q_NULLPTR, -1);
        strWarning =  QApplication::translate("CommonString", "Warning", Q_NULLPTR, -1);
    }
}
