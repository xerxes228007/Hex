#include "unit.h"

Unit::Unit(Type type)
{
    // TODO: add json serealization
    switch(type)
    {
        case Type::INFANTRY:
        {
            health = 3;
            speed = 1;
            damage = 1;
            protection = 3;
            break;
        }
        case Type::ARCHERS:
        {
            health = 1;
            speed = 1;
            damage = 3;
            protection = 1;
            break;
        }
        case Type::CAVALRY:
        {
            health = 2;
            speed = 2;
            damage = 2;
            protection = 3;
            break;
        }
        default:
        {
            break;
        }
    }
}
