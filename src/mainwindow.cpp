#include <QtCore>
#include <QtWidgets>
#include <QFile>
#include <QtPrintSupport>
#include "mainwindow.h"
#include "mapwidget.h"
#include "scalewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mySettings = new QSettings("knipling", "dtk-sachsen");
    createActions();
    createMenuBar();
    loadData();
    createMapWidget();
    myScroll = new QScrollArea();
    QWidget *mainWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainWidget->setLayout(mainLayout);
    myScroll->setWidget(myMapWidget);
    mainLayout->addWidget(myScroll);
    myScaleWidget = new ScaleWidget();
    mainLayout->addWidget(myScaleWidget);
    setCentralWidget(mainWidget);
    scale400Action->setChecked(true);
    setScale(400);
    myScroll->horizontalScrollBar()->setValue(myMapWidget->width()/2);
    myScroll->verticalScrollBar()->setValue(myMapWidget->height()/2);
    resize(1200, 800);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions() {
    QActionGroup *scaleGroup = new QActionGroup(this);
    scale400Action = new QAction(tr("400 pix"), scaleGroup);
    scale400Action->setCheckable(true);
    connect(scale400Action, &QAction::triggered, this, &MainWindow::scale400);
    scale200Action = new QAction(tr("200 pix"), scaleGroup);
    scale200Action->setCheckable(true);
    connect(scale200Action, &QAction::triggered, this, &MainWindow::scale200);
    scale100Action = new QAction(tr("100 pix"), scaleGroup);
    scale100Action->setCheckable(true);
    connect(scale100Action, &QAction::triggered, this, &MainWindow::scale100);
    printAction = new QAction("&Print", this);
    printAction->setShortcut(QKeySequence::Print);
    connect(printAction, &QAction::triggered, this, &MainWindow::print);
    settingsAction = new QAction("&Settings ...", this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::settings);
}

void MainWindow::createMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(printAction);
    fileMenu->addAction(settingsAction);
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(scale100Action);
    viewMenu->addAction(scale200Action);
    viewMenu->addAction(scale400Action);

}

void MainWindow::loadData() {

    QString fileName = mySettings->value("mapsettings", "coords.json").toString();
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning("Could not open file $1");
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument json = QJsonDocument::fromJson(data);
    qDebug()<<json["xtiles"];
    for (const QJsonValueRef& jTileRef: json["xtiles"].toArray()) {
        QJsonObject jTile = jTileRef.toObject();
        QString x = jTile["x"].toString();
        double utm0 = jTile["utm0"].toDouble();
        double utm1 = jTile["utm1"].toDouble();
        double wgs0 = jTile["lon0"].toDouble();
        double wgs1 = jTile["lon1"].toDouble();
        myTiles.addXTile(x, QPointF(utm0, utm1), QPointF(wgs0, wgs1));
    }
    for (const QJsonValueRef& jTileRef: json["ytiles"].toArray()) {
        QJsonObject jTile = jTileRef.toObject();
        QString y = jTile["y"].toString();
        double utm0 = jTile["utm0"].toDouble();
        double utm1 = jTile["utm1"].toDouble();
        double wgs0 = jTile["lat0"].toDouble();
        double wgs1 = jTile["lat1"].toDouble();
        myTiles.addYTile(y, QPointF(utm0, utm1), QPointF(wgs0, wgs1));
    }
    const QJsonObject& jtiles = json["tiles"].toObject();
    for (QJsonObject::const_iterator it = jtiles.begin(); it != jtiles.end(); ++it) {
        QJsonObject jtile = it.value().toObject();
        QJsonArray jutm = jtile["utm"].toArray();
        QJsonArray jwgs = jtile["wgs"].toArray();
        QPointF utm0(jutm[0].toArray()[0].toDouble(), jutm[0].toArray()[1].toDouble());
        QPointF utm1(jutm[1].toArray()[0].toDouble(), jutm[1].toArray()[1].toDouble());
        QPointF wgs0(jwgs[0].toArray()[0].toDouble(), jwgs[0].toArray()[1].toDouble());
        QPointF wgs1(jwgs[1].toArray()[0].toDouble(), jwgs[1].toArray()[1].toDouble());
        myTiles.addTile(it.key(), utm0, utm1, wgs0, wgs1);
    }
}

void MainWindow::createMapWidget() {
    myMapWidget = new MapWidget(&myTiles);
}

void MainWindow::setScale(int scale) {
    int xVal = myScroll->horizontalScrollBar()->value();
    int xMax = myScroll->horizontalScrollBar()->maximum();
    int yVal = myScroll->verticalScrollBar()->value();
    int yMax = myScroll->verticalScrollBar()->maximum();
    myMapWidget->setScale(scale);
    if (xMax == 0 || yMax == 0) return;
    double tileW = myTiles.sizeX()*myMapWidget->scale();
    double tileH = myTiles.sizeY()*myMapWidget->scale();
    qDebug()<<tileW<<tileH;
    double pixDiag = sqrt(tileW*tileW + tileH*tileH);
    double kmDiag = geoDist1(myTiles.wgs0(), myTiles.wgs1());
    qDebug()<<"Boundaries:"<<myTiles.wgs0()<<myTiles.wgs1();
    qDebug()<<"Diag"<<pixDiag<<" pixel "<<kmDiag<<" km "<<pixDiag/kmDiag<<" pix/km";
    myScaleWidget->setScale(pixDiag/kmDiag);
    myScroll->horizontalScrollBar()->setValue(xVal*myScroll->horizontalScrollBar()->maximum()/xMax);
    myScroll->verticalScrollBar()->setValue(yVal*myScroll->verticalScrollBar()->maximum()/yMax);
}

void MainWindow::print() {
    QPrinter printer(QPrinter::HighResolution);
    printer.setResolution(600);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setColorMode(QPrinter::Color);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() != QDialog::Accepted) return;
    QRegion mapReg = myMapWidget->visibleRegion();
    QRect bbox = mapReg.boundingRect();
    int mapScale = myMapWidget->scale();
    qDebug()<<bbox;
    int tx0 = (bbox.x()/mapScale);
    int ty1 = myTiles.sizeY() - (bbox.y()/mapScale);
    int tx1 = tx0 + bbox.width()/mapScale + 1;
    int ty0 = ty1 - bbox.height()/mapScale - 1;
    int x0 = (bbox.x()/mapScale)*mapScale;
    int y0 = (bbox.y()/mapScale)*mapScale;
    double offX = double(bbox.x()-x0)/mapScale;
    double offY = double(bbox.y()-y0)/mapScale;
    if (tx1 >= myTiles.sizeX()) tx1 = myTiles.sizeX()-1;
    if (ty1 >= myTiles.sizeY()) ty1 = myTiles.sizeY()-1;
    if (ty0 < 0) ty0 = 0;

    qDebug()<<"Region:"<<mapReg;
    qDebug()<<"Paper size:"<<printer.paperRect();
    qDebug()<<"Page size:"<<printer.pageRect();
    double dpCm = printer.resolution()/2.54;
    qDebug()<<"Printer units of 1 cm: "<<dpCm;
    double xScale = double(printer.pageRect().width())/bbox.width()*mapScale;
    double yScale = double(printer.pageRect().height()-dpCm)/bbox.height()*mapScale;
    double prScale = int((xScale < yScale)? xScale: yScale);
    double tileW = myTiles.sizeX()*prScale;
    double tileH = myTiles.sizeY()*prScale;
    qDebug()<<tileW<<tileH;
    double pixDiag = sqrt(tileW*tileW + tileH*tileH);
    double kmPr = geoDist1(myTiles.wgs0(), myTiles.wgs1());
    qDebug()<<"Boundaries:"<<myTiles.wgs0()<<myTiles.wgs1();
    qDebug()<<"Diag"<<pixDiag<<" pixel "<<kmPr<<" km "<<pixDiag/kmPr<<" pix/km";
    double prKmDist = pixDiag/kmPr;
    qDebug()<<"Scale"<<prScale<<xScale<<yScale;
    QPainter painter;
    QProgressDialog progressDlg(tr("Printing ..."), tr("Cancel"), 0, 100, this);
    progressDlg.setWindowModality(Qt::WindowModal);
    progressDlg.show();
    progressDlg.setValue(0);
    QCoreApplication::processEvents();
    painter.begin(&printer);
    double pr1cm = printer.paperRect().width()/29.7;
    double prH = printer.pageRect().height()-pr1cm;
    double prW = printer.pageRect().width();
    painter.setClipRect(0, 0, printer.pageRect().width(), prH);
    double x = -offX*prScale;
    for (int tx = tx0; x <= prW; tx++, x+=prScale) {
        double y = -offY*prScale;
        for (int ty = ty1; y < prH; ty--, y+=prScale) {
            QString keyX = myTiles.keyX(tx);
            QString keyY = myTiles.keyY(ty);
            QString fileName = QString("/data/dtk-sachsen/DTK25/pngs/%1_%2.png").arg(keyX).arg(keyY);
            qDebug()<<fileName;
            QPixmap pixmap(fileName);
            if (pixmap.isNull()) continue;
            painter.drawPixmap(x, y, prScale, prScale, pixmap.scaled(prScale, prScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            QCoreApplication::processEvents();
        }
        progressDlg.setValue((tx-tx0)*100/(tx1-tx0)-1);
    }
    double scXoff = 0.1*pr1cm;
    double scYm = prH + 0.5*pr1cm;
    double scYm0 = scYm - 0.1*pr1cm;
    double scYm1 = scYm + 0.2*pr1cm;
    double scYm2 = scYm + 0.4*pr1cm;
    painter.setClipRect(0, 0, printer.pageRect().width(), printer.pageRect().height());
    QPen pen(Qt::black);
    pen.setWidth(0.035*pr1cm);
    painter.setPen(pen);
    for (int i = 0; i*prKmDist <= printer.pageRect().width(); i++) {
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(QRectF(i*prKmDist, scYm0, 0.5*prKmDist, 0.1*pr1cm));
        painter.setBrush(QBrush(Qt::black));
        painter.drawRect(QRectF((i+0.5)*prKmDist, scYm0, 0.5*prKmDist, 0.1*pr1cm));
        if (i > 0) {
            painter.drawLine(i*prKmDist, scYm0, i*prKmDist, scYm1);
            painter.drawText(QPointF(scXoff + i*prKmDist, scYm2), tr("%1 km").arg(i));
        }
    }
    //painter.drawLine(0, 0, printer.pageRect().width(), 0);
    //painter.drawLine(0, printer.pageRect().height(), printer.pageRect().width(), printer.pageRect().height());
    painter.end();
    progressDlg.setValue(100);
}

void MainWindow::settings() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Map config file"), ".", "coords.json");
    if (fileName.isEmpty()) return;
    mySettings->setValue("mapsettings", fileName);
    loadData();
    myMapWidget->setTiles(&myTiles);
    myScroll->horizontalScrollBar()->setValue(myMapWidget->width()/2);
    myScroll->verticalScrollBar()->setValue(myMapWidget->height()/2);
}
