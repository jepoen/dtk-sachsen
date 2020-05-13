#include <QtWidgets>
#include "scalewidget.h"

ScaleWidget::ScaleWidget(QWidget *parent):
    QWidget(parent),
    myScale(0)
{
    setMinimumSize(3*myScale, 30);
    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Preferred);
    setSizePolicy(policy);
}

void ScaleWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    QFont scaleFont(font().defaultFamily(), 8);
    painter.begin(this);
    painter.setFont(scaleFont);
    painter.fillRect(QRectF(0, 0, width(), 30), Qt::white);
    painter.setPen(Qt::black);
    painter.drawLine(QPointF(10, 15), QPointF(width()-10, 15));
    //qDebug()<<"width "<<width()<<" scale "<<myScale;
    if (myScale == 0) return;
    for (int i = 0; i*myScale <= width()-20; i++) {
        painter.drawLine(QPointF(10+i*myScale, 5), QPointF(10+i*myScale, 25));
        if (i > 0) {
            painter.drawText(QPointF(11+i*myScale, 26), tr("%1 km").arg(i));
        }
    }
    // Tile size
    //painter.setPen(Qt::blue);
    //painter.drawLine(410, 5, 410, 25);
    painter.end();
}
