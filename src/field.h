#ifndef FIELD_H
#define FIELD_H
#include <QVector2D>
#include <QVector>
#include <QColor>
#include <QPolygonF>
#include <QImage>

class Field
{
public:
    Field(int, int, int);
    QVector<QVector2D> getCoordinates(){return coordinates;};
    QPolygonF getHexagon(){return hexagon;};
    bool isInside(int x, int y);
    void setColor(QColor a){color = a;};
    QColor getColor(){return color;};
    QImage getTexture(){return image;};
private:
    int x;
    int y;
    int r;
    QPolygonF hexagon;
    QImage image;
    QVector<QVector2D> coordinates;
    QColor color = Qt::black;
};

#endif // FIELD_H
