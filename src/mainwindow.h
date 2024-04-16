#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "field.h"
#include <QVector2D>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    const QVector2D NO_CLICK {-1, -1};
    void generateMap();
    void moveUnit(int i1, int j1, int i2,int j2);
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    QPolygonF scalePolygon(QPolygonF);
    QPolygonF scaleOtherPolygon(QPolygonF);
    QVector2D firstClick{-1,-1};
    double scale = 1;
    double mas = 1;
    QVector<QVector2D> scaleCoordinates;
    Ui::MainWindow *ui;
    QVector<QVector<Field>> map;
    QVector<QVector<QPolygonF>> mapPolygons;
    QVector<QPolygonF> otherPolygons;
    void fieldsToPolygons();
    void otherToPolygons();
};
#endif // MAINWINDOW_H
