#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "field.h"
#include <QVector2D>
#include "client.h"
#include <QByteArray>

QT_BEGIN_NAMESPACE
namespace Ui {
class GameWindow;
}
QT_END_NAMESPACE

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    const QVector<QColor> PLAYER_COLORS {Qt::magenta, Qt::yellow, Qt::cyan, Qt::darkGreen};
    GameWindow(Client* client, QVector<QVector2D>, QVector<QVector<Biome::BiomeName>>, QWidget *parent = nullptr);
    ~GameWindow();
    void setData(QByteArray a) {Data = a;};
private:
    Client* client;
    QTcpSocket* socket;
    QByteArray Data;
    const QVector2D NO_CLICK {-1, -1};
    void generateMap();
    void moveUnit(int, int, int, int);
    void battleUnit(int, int, int, int);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    QPolygonF scalePolygon(QPolygonF);
    QPolygonF scaleOtherPolygon(QPolygonF);
    void refreshAvailiableFields(QVector2D);
    void moveMap(int dx, int dy);
    QVector<QVector2D> availiableFields;
    QVector<QVector<QString>> namesOfOther;
    bool wasClickedOnTheMap = 0;
    bool isMovingMap = 0;
    QVector2D firstClick{-1,-1};
    QVector2D lastMoveCordinate{-1, -1};
    QVector2D selectedFieldIndex{-1, -1};
    double scale = 1;
    double mas = 1;
    QVector<QVector2D> scaleCoordinates;
    Ui::GameWindow *ui;
    QVector<QVector<Field>> map;
    QVector<QVector<QPolygonF>> mapPolygons;
    QVector<QVector<QPolygonF>> unitPolygons;
    void fieldsToPolygons();
    void refreshUnits();
    void otherToPolygons();
    QPolygonF getUnitInMap(int i, int j);
    void turnButtons(bool);
    bool isMyTurn = false;
    bool wasMoved = 0;
    int myMoney = 5;
    int availiableUnitsT1 = 1;
    int availiableUnitsT2 = 0;
    int availiableUnitsT3 = 0;
    bool isLose = false;
    bool isWin = false;
    void updateLabels();
private slots:
    void ReadSlot();
    void SendSlot();
    void on_endTurn_pushButtin_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
};
#endif // GAMEWINDOW_H
