#ifndef GAME_H
#define GAME_H
#include <vector>
#include <stdint.h>
#include "../src/field.h"

class Game
{
public:
    Game(uint8_t numberOfPlayers, uint8_t xMapSize, uint8_t yMapSize);
    inline QVector<QVector<Field>> getMap() const {return map;};
private:
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y);
    double perlin(double x, double y);
    void generateMap();
    std::vector<int> permutation;
    uint8_t countPlayers;
    uint8_t mapSizeX;
    uint8_t mapSizeY;
    QVector<QVector<Field>> map;
};

#endif // GAME_H
