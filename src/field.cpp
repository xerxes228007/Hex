#include "field.h"
#include <QDebug>

#include "texture.h"

Field::Field(int centerX, int centerY, int r, Biome b)
    :biome(b)
{

    image = *Texture::getTexture("sand");

    qDebug() << image;

    for (int i = 0; i < 6; i++)
    {
        float currentAngle = 60 * i;
        float x = r * sin(qDegreesToRadians(currentAngle));
        float y = r * cos(qDegreesToRadians(currentAngle));
        hexagon << QPointF(x+ centerX, y+ centerY);
        coordinates.push_back(QVector2D(x + centerX, y + centerY));
    }
}

bool Field::isInside(int x, int y)
{
    return hexagon.containsPoint(QPoint(x, y),Qt::FillRule::OddEvenFill);
};
