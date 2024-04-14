#include "unit.h"
#include <QtMath>
#include <QDebug>

Unit::Unit(uint16_t centerX, uint16_t centerY,Type type)
    :type(type)
{
    // TODO: add json serealization


    if(type!=Type::NONE)
    {
        qDebug()<<centerX;
        qDebug()<<centerY;

        for (int i = 0; i < 4; i++)
        {
            float currentAngle = 90 * i;
            float x = 10 * sin(qDegreesToRadians(currentAngle));
            float y = 10 * cos(qDegreesToRadians(currentAngle));
            Polygon << QPointF(x+ centerX, y+ centerY);

        }
    }

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
