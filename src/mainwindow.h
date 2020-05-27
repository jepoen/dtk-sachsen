#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QList>
#include <QString>

#include "tiles.h"

class QAction;
class QScrollArea;
class QSettings;
class MapWidget;
class ScaleWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QSettings *mySettings;
    QJsonDocument myData;
    Tiles myTiles;
    QScrollArea *myScroll;
    MapWidget *myMapWidget;
    ScaleWidget *myScaleWidget;
    QList<QString> myMaps;

    QAction *scale400Action;
    QAction *scale200Action;
    QAction *scale100Action;
    QAction *printAction;
    QAction *settingsAction;

    void createActions();
    void createMenuBar();
    void loadData();
    void createMapWidget();
    void createMap(int idx);
    void settings();


public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void setScale(int scale);
    void scale400() { setScale(400); }
    void scale200() { setScale(200); }
    void scale100() { setScale(100); }
    void print();
    void scroll();
};

#endif // MAINWINDOW_H
