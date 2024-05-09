#ifndef BIOME_H
#define BIOME_H
#include <QImage>

class Biome
{
public:
    enum class BiomeName
    {
        PLAIN,
        FOREST,
        DESERT,
        RIVER,
        MOUNTAINS,
        BLANK
    };
    Biome(BiomeName);
    inline QString getName() const {return name;};
private:
    QString name = "";







};

#endif // BIOME_H
