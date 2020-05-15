#include <QtCore>
#include <QtWidgets>
#include "mapwidget.h"

MapWidget::MapWidget(Tiles *tiles, QWidget *parent):
    QWidget(parent),
    myTiles(tiles),
    myScale(400)
{
    setScale(myScale);
}

void MapWidget::paintEvent(QPaintEvent *event) {
    qDebug()<<"mapWidget paint event";
    if (myTiles->sizeX() == 0 || myTiles->sizeY() == 0) return;
    QScrollArea *area = static_cast<QScrollArea*>(parent());
    qDebug()<<area->visibleRegion();
    QRect bbox = event->region().boundingRect();
    qDebug()<<bbox;
    int tx0 = (bbox.x()/myScale);
    int ty1 = myTiles->sizeY() - (bbox.y()/myScale);
    int tx1 = tx0 + bbox.width()/myScale + 1;
    int ty0 = ty1 - bbox.height()/myScale - 1;
    int x0 = (bbox.x()/myScale)*myScale;
    int y0 = (bbox.y()/myScale)*myScale;
    if (tx1 >= myTiles->sizeX()) tx1 = myTiles->sizeX()-1;
    if (ty1 >= myTiles->sizeY()) ty1 = myTiles->sizeY()-1;
    if (ty0 < 0) ty0 = 0;
    qDebug()<<tx0<<ty0<<tx1<<ty1<<myTiles->sizeX()<<myTiles->sizeY();
    QPainter painter;
    //painter.setRenderHint(QPainter::Antialiasing, true);
    painter.begin(this);
    int x = x0;
    for (int tx = tx0; tx <= tx1; tx++, x+=myScale) {
        int y = y0;
        for (int ty = ty1; ty >= ty0; ty--, y+=myScale) {
            QString keyX = myTiles->keyX(tx);
            QString keyY = myTiles->keyY(ty);
            QString fileName = QDir(myTiles->tileDir()).absoluteFilePath(QString("%1_%2.png").arg(keyX).arg(keyY));
            //qDebug()<<fileName;
            QPixmap pixmap(fileName);
            if (pixmap.isNull()) continue;
            painter.drawPixmap(x, y, myScale, myScale, pixmap.scaled(myScale, myScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
    }
    painter.end();
}

void MapWidget::setTiles(Tiles *tiles) {
    myTiles = tiles;
    setScale(myScale);
}

void MapWidget::setScale(int scale) {
    myScale = scale;
    int width = myTiles->sizeX()*myScale;
    int height = myTiles->sizeY()*myScale;
    if (width == 0 || height == 0) {
        height = width = myScale;
    }
    setGeometry(0, 0, width, height);
    repaint();
}
