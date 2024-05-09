#ifndef STRUCTURE_H
#define STRUCTURE_H
#include <stdint.h>
#include <bitset>
#include <QString>

constexpr static auto NUMBER_OF_UNITS = 3;

class Structure
{
public:

    enum class Type{
        NONE,
        CASTLE,
        GOLD_MINE,
        SHOOTING_RANGE,
        HORSE_STABLE
    };
    Structure(Type type);
    inline Type getType() const {return type;};
    inline QString genName() const {return name;};

private:
    QString name = "";
    Type type = Type::NONE;
    uint8_t earning = 0;
    std::bitset<NUMBER_OF_UNITS> availableUnits;
};

#endif // STRUCTURE_H
