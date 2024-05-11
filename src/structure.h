#ifndef STRUCTURE_H
#define STRUCTURE_H
#include <stdint.h>
#include <bitset>
#include <QString>

constexpr static auto NUMBER_OF_UNITS = 3;

class Structure
{
public:

    enum Type : int
    {
        NONE            = 0x00,
        CASTLE          = 0x01,
        GOLD_MINE       = 0x02,
        SHOOTING_RANGE  = 0x03,
        HORSE_STABLE    = 0x04
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
