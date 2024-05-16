#include "structure.h"
#include "unit.h"

Structure::Structure(Type type)
    :type(type)
{

    switch (type) {
    case Type::CASTLE:
    {
        earning = 3;
        availableUnits.set(static_cast<size_t>(Unit::Type::INFANTRY));
        name = "castle";
        break;
    }
    case Type::GOLD_MINE:
    {
        earning = 5;
        availableUnits.set(static_cast<size_t>(Unit::Type::NONE));
        name = "goldmine";
        break;
    }
    case Type::SHOOTING_RANGE:
    {
        earning = 0;
        availableUnits.set(static_cast<size_t>(Unit::Type::ARCHERS));
        name = "shootingrange";
        break;
    }
    case Type::HORSE_STABLE:
    {
        earning = 0;
        availableUnits.set(static_cast<size_t>(Unit::Type::CAVALRY));
        break;
        name = "horsestable";
    }
    default:
    {
        earning = 0;
        availableUnits.set(static_cast<size_t>(Unit::Type::NONE));
        break;
    }
    }
}
