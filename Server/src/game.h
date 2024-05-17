#ifndef GAME_H
#define GAME_H
#include <vector>
#include <stdint.h>
#include "field.h"
#include <QVector2D>

class Game
{
public:
    const QVector<QColor> PLAYER_COLORS {Qt::magenta, Qt::yellow, Qt::cyan, Qt::darkGreen};
    Game(uint8_t numberOfPlayers, uint8_t xMapSize, uint8_t yMapSize);
    inline QVector<QVector<Field>> getMap() const {return map;};
    inline QVector<QVector2D> getStructures() const {return structures;};
    QVector<QVector<Field>> map;
private:
    QVector<QVector2D> structures;
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y);
    double perlin(double x, double y);
    void generateMap();
    std::vector<int> permutation;
    uint8_t countPlayers;
    uint8_t mapSizeX;
    uint8_t mapSizeY;
    bool isFreeField(int, int);

};

#endif // GAME_H
