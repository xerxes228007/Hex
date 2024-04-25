#ifndef FIELD_H
#define FIELD_H
#include <QColor>
#include <QPolygonF>
#include "biome.h"
#include "structure.h"
#include "unit.h"
#include <qDebug>


class Field
{
public:
    Field(int, int, int, Biome);
    QPolygonF getHexagon(){return hexagon;};
    bool isInside(int x, int y);
    void setColor(QColor a){color = a;};
    QColor getColor(){return color;};
    inline bool isHereStructere()const{return hereStructure.getType()!=Structure::Type::NONE;};
    inline bool isHereUnit()const{return hereUnit.getType()!=Unit::Type::NONE;};
    void addStructure(Structure::Type type){hereStructure = Structure(type);};
    void removeStructure(){hereStructure = Structure(Structure::Type::NONE);};
    void addUnit(Unit::Type type){  hereUnit = Unit(x, y+10, type);};
    void removeUnit(){hereUnit = Unit(x, y, Unit::Type::NONE);};
    inline Unit getUnit() const {return hereUnit;};
    inline Structure getStructure() const {return hereStructure;};
    inline std::string_view getName()const {return biome.getName();};
private:
    int x;
    int y;
    int r;
    std::string_view name = "";
    Structure hereStructure;
    Unit hereUnit;
    QPolygonF hexagon;
    QImage image;
    Biome biome;
    QColor color = Qt::lightGray;
};

#endif // FIELD_H
