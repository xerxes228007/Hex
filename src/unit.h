#ifndef UNIT_H
#define UNIT_H

#include <stdint.h>

class Unit
{
public:

    enum class Type
    {
        NONE,
        INFANTRY,
        ARCHERS,
        CAVALRY
    };

    Unit(Type type);

    inline uint8_t getHealth() const {return health;};
    inline uint8_t getDamage() const {return damage;};
    inline uint8_t getProtection() const {return protection;};
    inline uint8_t getSpeed() const {return speed;};
    inline Type getType() const {return type;};


private:

    Type type = Type::NONE;

    uint8_t health      = 0;
    uint8_t damage      = 0;
    uint8_t protection  = 0;
    uint8_t speed       = 0;

};

#endif // UNIT_H
