#ifndef FIELD_H
#define FIELD_H
#include <QVector2D>
#include <QVector>
#include <QColor>
#include <QPolygonF>
#include "biome.h"
#include "structure.h"
#include "unit.h"


class Field
{
public:
    Field(int, int, int, Biome);
    QVector<QVector2D> getCoordinates(){return coordinates;};
    QPolygonF getHexagon(){return hexagon;};
    bool isInside(int x, int y);
    void setColor(QColor a){color = a;};
    QColor getColor(){return color;};
    QImage getTexture(){return image;};
    inline bool isHereStructere()const{return hereStructure.getType()!=Structure::Type::NONE;};
    inline bool isHereUnit()const{return hereUnit.getType()!=Unit::Type::NONE;};
    void addStructure(Structure::Type type){hereStructure = Structure(type);};
    void removeStructure(){hereStructure = Structure(Structure::Type::NONE);};
    void addUnit(Unit::Type type){hereUnit = Unit(type);};
    void removeUnit(){hereUnit = Unit(Unit::Type::NONE);};
private:
    int x;
    int y;
    int r;
    Structure hereStructure;
    Unit hereUnit;
    QPolygonF hexagon;
    QImage image;
    Biome biome;
    QVector<QVector2D> coordinates;
    QColor color = Qt::black;
};

#endif // FIELD_H
