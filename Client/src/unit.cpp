#include "unit.h"
#include <QtMath>
#include <QDebug>

Unit::Unit(uint16_t centerX, uint16_t centerY,Type type, uint8_t c)
    :type(type)
    ,id(c)
{


    switch(type)
    {
        case Type::INFANTRY:
        {
            name = "infantry";
            health = 3;
            speed = 1;
            damage = 1;
            protection = 3;

            for (int i = 0; i < 4; i++)
            {
                float currentAngle = 90 * i;
                float x = 10 * sin(qDegreesToRadians(currentAngle));
                float y = 10 * cos(qDegreesToRadians(currentAngle));
                Polygon << QPointF(x+ centerX, y+ centerY);

            }
            break;
        }
        case Type::ARCHERS:
        {
            name = "archer";
            health = 1;
            speed = 1;
            damage = 3;
            protection = 1;

            for (int i = 0; i < 3; i++)
            {
                float currentAngle = 120 * i;
                float x = 13 * sin(qDegreesToRadians(currentAngle));
                float y = 13 * cos(qDegreesToRadians(currentAngle));
                Polygon << QPointF(x+ centerX, y+ centerY+3);

            }
            break;
        }
        case Type::CAVALRY:
        {
            name = "cavalry";
            health = 2;
            speed = 2;
            damage = 2;
            protection = 3;

            for (int i = 0; i < 36; i++)
            {
                float currentAngle = 10 * i;
                float x = 10 * sin(qDegreesToRadians(currentAngle));
                float y = 10 * cos(qDegreesToRadians(currentAngle));
                Polygon << QPointF(x+ centerX, y+ centerY);

            }
            break;
        }
        default:
        {
            break;
        }
    }
}
