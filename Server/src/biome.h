#ifndef BIOME_H
#define BIOME_H
#include <QImage>

class Biome
{
public:
    enum BiomeName : int
    {
        PLAIN       = 0,
        FOREST      = 1,
        DESERT      = 2,
        RIVER       = 3,
        MOUNTAINS   = 4,
        BLANK       = 5
    };
    Biome(BiomeName);
    inline QString getName() const {return name;};
    inline BiomeName getBiome() const {return biome;};
private:
    QString name = "";
    BiomeName biome;







};

#endif // BIOME_H
