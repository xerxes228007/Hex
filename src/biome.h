#ifndef BIOME_H
#define BIOME_H
#include <QImage>

class Biome
{
public:
    enum class BiomeName{PLAIN, FOREST, DESERT, RIVER, BLANK};
    Biome(BiomeName);
private:

};

#endif // BIOME_H
