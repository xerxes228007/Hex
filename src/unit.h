#ifndef UNIT_H
#define UNIT_H

#include <stdint.h>
#include <QPolygonF>

class Unit
{
public:

    enum class Type:uint8_t
    {
        NONE = 0b000,
        INFANTRY = 0b001,
        ARCHERS = 0b010,
        CAVALRY = 0b100
    };

    Unit(uint16_t, uint16_t, Type type);

    inline uint8_t getHealth() const {return health;};
    inline uint8_t getDamage() const {return damage;};
    inline uint8_t getProtection() const {return protection;};
    inline uint8_t getSpeed() const {return speed;};
    inline Type getType() const {return type;};
    inline QPolygonF getPolygon() const {return Polygon;};
    inline std::string_view getName() const{return name;};


private:
    std::string_view name = "";
    Type type;
    QPolygonF Polygon;
    uint8_t health      = 0;
    uint8_t damage      = 0;
    uint8_t protection  = 0;
    uint8_t speed       = 0;

};

#endif // UNIT_H
