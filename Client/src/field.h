#ifndef FIELD_H
#define FIELD_H

#include <QColor>
#include <QPolygonF>
#include "biome.h"
#include "structure.h"
#include "unit.h"
#include <QDebug>
#include <stdint.h>

class Field
{
public:
    Field(int, int, int, Biome::BiomeName);
    QPolygonF getHexagon(){return hexagon;};
    bool isInside(int x, int y);
    void setColor(QColor a){color = a;};
    inline QColor getColor() const {return color;};
    inline QColor getDefColor() const {return defaultColor;};
    inline bool isHereStructere()const{return hereStructure.getType()!=Structure::Type::NONE;};
    inline bool isHereUnit()const{return hereUnit.getType()!=Unit::Type::NONE;};
    void addStructure(Structure::Type type){hereStructure = Structure(type);};
    void removeStructure(){hereStructure = Structure(Structure::Type::NONE);};
    void addUnit(Unit::Type type, uint8_t id){  hereUnit = Unit(x, y+10, type, id);};
    void removeUnit(){hereUnit = Unit(x, y, Unit::Type::NONE, 0);};
    inline Unit getUnit() const {return hereUnit;};
    inline Structure getStructure() const {return hereStructure;};
    inline QString getName()const {return biome.getName();};
    inline Biome::BiomeName getBiome()const {return biome.getBiome();};
    inline bool isVisible() const {return visionStatus;};
    void setVisible() {visionStatus = true;};
    void setDefaultColor(QColor color) {defaultColor = color;};
    inline uint8_t getUnitCount() const { return hereUnitCount;};
    void setUnitCount(uint8_t c) {hereUnitCount = c;};
private:
    int x;
    int y;
    int r;
    QString name = "";
    Structure hereStructure;
    Unit hereUnit;
    QPolygonF hexagon;
    QImage image;
    Biome biome;
    QColor color = Qt::lightGray;
    QColor defaultColor = Qt::lightGray;
    bool visionStatus = 0;
    uint8_t hereUnitCount = 0;

};

#endif // FIELD_H
