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
    GameWindow(Client* client,QVector<QVector<Biome::BiomeName>>, QWidget *parent = nullptr);
    ~GameWindow();

private:
    Client* client;
    QTcpSocket* socket;
    QByteArray Data;
    const QVector2D NO_CLICK {-1, -1};
    void generateMap();
    void moveUnit(int i1, int j1, int i2,int j2);
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
    QVector<QString> namesOfOther;
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
    QVector<QPolygonF> otherPolygons;
    void fieldsToPolygons();
    void refreshUnits();
    void otherToPolygons();
    QPolygonF getUnitInMap(int i, int j);
private slots:
    void ReadSlot();

};
#endif // GAMEWINDOW_H
