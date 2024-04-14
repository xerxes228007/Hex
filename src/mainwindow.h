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
    void generateMap();
    void moveUnit(int i1, int j1, int i2,int j2);
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    QVector2D firstCkick{-1,-1};
    Ui::MainWindow *ui;
    QVector<QVector<Field>> map;
};
#endif // MAINWINDOW_H
