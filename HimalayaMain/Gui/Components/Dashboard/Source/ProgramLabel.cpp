#include "Dashboard/Include/ProgramLabel.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

namespace Dashboard {

CProgramLabel::CProgramLabel(QWidget *p_Parent) :
    QLabel(p_Parent)
  , content("")
  , textHighlight(false)
{

}

void CProgramLabel::setText(const QString &text, bool highlight)
{
    content = text;
    textHighlight = highlight;

    if (highlight == false) {
        QLabel::setText(content);
    }
    else {
        QLabel::setText(QString(""));
    }
    repaint();
}

void CProgramLabel::setHighlight(bool flag)
{
    textHighlight = flag;
    if (flag == false) {
        QLabel::setText(content);
    }
    repaint();
}

void CProgramLabel::paintEvent(QPaintEvent * event)
{
    QLabel::paintEvent(event);

    if (textHighlight) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QFont textFont;
        textFont.setPointSize(10);
        textFont.setBold(true);

        QRect rect = event->rect();
        QPainterPath textPath;
        QFontMetrics fm(textFont);

        textPath.addText(rect.x(), rect.y()+ fm.height() -1 - fm.descent() + 20, textFont, content);

        QPainterPathStroker pps;
        pps.setWidth(3);
        pps.setDashPattern(Qt::SolidLine);
        pps.setCurveThreshold(0.001);
        pps.setCapStyle(Qt::RoundCap);
        pps.setJoinStyle(Qt::RoundJoin);
        QPainterPath strokePath = pps.createStroke(textPath);

        painter.fillPath(strokePath, QBrush(QColor(255, 128, 128)));
        painter.setPen(Qt::black);
        painter.drawText(rect.x(), rect.y() + fm.height() -1 - fm.descent() + 20, content);
    }
}

CProgramLabel::~CProgramLabel()
{
}

} // namespace

