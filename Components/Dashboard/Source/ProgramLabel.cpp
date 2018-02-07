#include "Dashboard/Include/ProgramLabel.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

namespace Dashboard {

CProgramLabel::CProgramLabel(QWidget *p_Parent) :
    QLabel(p_Parent)
  , m_Content("")
  , m_PixmapContent(152, 62)
{

}

void CProgramLabel::setText(const QString &txt, bool highlight)
{
    m_Content = txt;
    if (highlight == false) {
        QLabel::setText(m_Content);
    }
    else {
        QLabel::setText(m_Content);
        m_PixmapContent.fill(Qt::transparent);
        QPainter painter;
        (void)painter.begin(&m_PixmapContent);
        painter.setRenderHint(QPainter::Antialiasing);

        QFont textFont;
        textFont.setPointSize(10);
        textFont.setBold(true);

        QRect r(0, 0, 152, 62);
        QPainterPath textPath;
        QFontMetrics fm(textFont);

        textPath.addText(r.x(), r.y()+ fm.height() -1 - fm.descent() + 20, textFont, m_Content);

        QPainterPathStroker pps;
        pps.setWidth(5);
        pps.setDashPattern(Qt::SolidLine);
        pps.setCurveThreshold(0.001);
        pps.setCapStyle(Qt::RoundCap);
        pps.setJoinStyle(Qt::RoundJoin);
        QPainterPath strokePath = pps.createStroke(textPath);

        painter.fillPath(strokePath, QBrush(QColor(255, 128, 128)));
        painter.setPen(Qt::black);
        painter.drawText(r.x(), r.y() + fm.height() -1 - fm.descent() + 20, m_Content);
        painter.end();
    }
}

void CProgramLabel::paintEvent(QPaintEvent * e)
{
    QPainter painter(this);
    painter.drawPixmap(e->rect(), m_PixmapContent, e->rect());
}

CProgramLabel::~CProgramLabel()
{
}

} // namespace

