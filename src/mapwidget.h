#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>

#include "tiles.h"

class MapWidget : public QWidget
{
    Q_OBJECT
private:
    Tiles *myTiles;
    int myScale;
protected:
    void paintEvent(QPaintEvent *event);
public:
    explicit MapWidget(Tiles *tiles, QWidget *parent = nullptr);
    int scale() const { return myScale; }
    void setTiles(Tiles *tiles);
    void setScale(int scale);
signals:

public slots:

};

#endif // MAPWIDGET_H
