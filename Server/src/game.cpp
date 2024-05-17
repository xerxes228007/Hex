#include "game.h"
#include <QRandomGenerator64>



Game::Game(uint8_t numberOfPlayers, uint8_t xMapSize, uint8_t yMapSize)
    :countPlayers(numberOfPlayers)
    ,mapSizeX(xMapSize)
    ,mapSizeY(yMapSize)
{
    generateMap();
}


double Game::lerp(double t, double a, double b)
{
    return a + t * (b - a);
}

double Game::grad(int hash, double x, double y)
{
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double Game::perlin(double x, double y)
{
    int X = static_cast<int>(x) & 255;
    int Y = static_cast<int>(y) & 255;

    double xf = x - static_cast<int>(x);
    double yf = y - static_cast<int>(y);

    double u = xf * xf * xf * (xf * (xf * 6 - 15) + 10);
    double v = yf * yf * yf * (yf * (yf * 6 - 15) + 10);

    int A = permutation[X] + Y;
    int AA = permutation[A];
    int AB = permutation[A + 1];
    int B = permutation[X + 1] + Y;
    int BA = permutation[B];
    int BB = permutation[B + 1];

    return lerp(v, lerp(u, grad(AA, xf, yf),
                grad(BA, xf - 1, yf)),
                lerp(u, grad(AB, xf, yf - 1),
                grad(BB, xf - 1, yf - 1)));
}

void Game::generateMap(){

    for(int i =0; i<256; ++i){
        int a = QRandomGenerator::global()->bounded(0, 256);
        while (std::find(permutation.begin(),permutation.end(), a) != permutation.end()) {
            a = QRandomGenerator::global()->bounded(0, 256);
        }
        permutation.push_back(a);
        qDebug()<<a;
    }
    int a = 0;
    for(int i = 0; i < mapSizeY; i++){
        int offset = (i%2==0 ? 20 : 0);
        QVector<Field> row;
        for(int j = 0; j < mapSizeX; ++j){
            int p = static_cast<int>(perlin(j / 10.0, i / 10.0) * 10);
            a++;
            if(p < -3)
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::RIVER));
            }
            else if(p<-2)
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::DESERT));
            }
            else if(p<1)
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::PLAIN));
            }
            else if(p<4)
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::FOREST));
            }
            else
            {
                row.push_back(Field(offset+30+j*40, 30 + i*35, 20, Biome::BiomeName::MOUNTAINS));
            }
        }
        map.push_back(row);
    }

    permutation.clear();


    int x, y;

    for (int i = 0; i < countPlayers; ++i) {
        do {
            x = QRandomGenerator::global()->bounded(0, mapSizeY);
            y = QRandomGenerator::global()->bounded(0, mapSizeX);
        } while (map[x][y].getBiome() == Biome::BiomeName::RIVER || !isFreeField(x, y));
        structures.push_back(QVector2D(x,y));
    }


    for (int i = 0; i < 3*countPlayers; i++) {

        do {
            x = QRandomGenerator::global()->bounded(0, mapSizeY);
            y = QRandomGenerator::global()->bounded(0, mapSizeX);
        } while (map[x][y].getBiome() == Biome::BiomeName::RIVER || !isFreeField(x, y));
        structures.push_back(QVector2D(x,y));
    }

    for (int i = 0; i < 2*countPlayers; i++) {
        do {
            x = QRandomGenerator::global()->bounded(0, mapSizeY);
            y = QRandomGenerator::global()->bounded(0, mapSizeX);
        } while (map[x][y].getBiome() != Biome::BiomeName::FOREST || !isFreeField(x, y));
        structures.push_back(QVector2D(x,y));
    }

    for (int i = 0; i < countPlayers; i++) {
        do {
            x = QRandomGenerator::global()->bounded(0, mapSizeY);
            y = QRandomGenerator::global()->bounded(0, mapSizeX);
        } while (map[x][y].getBiome() != Biome::BiomeName::PLAIN || !isFreeField(x, y));
        structures.push_back(QVector2D(x,y));
    }


    for(int i = 0; i<countPlayers; ++i)
    {
        map[structures[7*i].x()][structures[7*i].y()].addStructure(Structure::Type::CASTLE);
        map[structures[7*i].x()][structures[7*i].y()].setColor(PLAYER_COLORS[i]);
        map[structures[7*i+1].x()][structures[7*i+1].y()].addStructure(Structure::Type::GOLD_MINE);
        map[structures[7*i+2].x()][structures[7*i+2].y()].addStructure(Structure::Type::GOLD_MINE);
        map[structures[7*i+3].x()][structures[7*i+3].y()].addStructure(Structure::Type::GOLD_MINE);
        map[structures[7*i+4].x()][structures[7*i+4].y()].addStructure(Structure::Type::SHOOTING_RANGE);
        map[structures[7*i+5].x()][structures[7*i+5].y()].addStructure(Structure::Type::SHOOTING_RANGE);
        map[structures[7*i+6].x()][structures[7*i+6].y()].addStructure(Structure::Type::HORSE_STABLE);
    }
    foreach (auto a, structures) {
        qDebug()<<map[a.x()][a.y()].getName().append(map[a.x()][a.y()].getStructure().genName());
    }
}

bool Game::isFreeField(int x, int y)
{
    if(map[x][y].isHereStructere()) return false;
    QVector<QVector2D> availiableFields;
    if(static_cast<int>(x) % 2==0){
        availiableFields.push_back(QVector2D(x-1, y));
        availiableFields.push_back(QVector2D(x-1, y+1));
        availiableFields.push_back(QVector2D(x, y-1));
        availiableFields.push_back(QVector2D(x, y+1));
        availiableFields.push_back(QVector2D(x+1, y));
        availiableFields.push_back(QVector2D(x+1, y+1));
    }
    else{

        availiableFields.push_back(QVector2D(x-1, y-1));
        availiableFields.push_back(QVector2D(x-1, y));
        availiableFields.push_back(QVector2D(x, y-1));
        availiableFields.push_back(QVector2D(x, y+1));
        availiableFields.push_back(QVector2D(x+1, y-1));
        availiableFields.push_back(QVector2D(x+1, y));
    }

    for(int i = availiableFields.size()-1; i>=0; --i){

        if(availiableFields[i].x()>mapSizeY-1 || availiableFields[i].x()<0||availiableFields[i].y()>mapSizeX-1||availiableFields[i].y()<0) availiableFields.remove(i);

    }

    foreach (auto c, availiableFields) {
        if(map[c.x()][c.y()].isHereStructere()) return false;
    }
    return true;
}
