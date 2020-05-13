#ifndef SCALEWIDGET_H
#define SCALEWIDGET_H

#include <QWidget>

class ScaleWidget : public QWidget
{
    Q_OBJECT
private:
    double myScale; // pix per km
public:
    explicit ScaleWidget(QWidget *parent = nullptr);
    void setScale(double scale) { myScale = scale; repaint(); }
protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
};

#endif // SCALEWIDGET_H
