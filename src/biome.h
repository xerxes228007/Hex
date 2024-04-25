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
    inline std::string_view getName() const {return name;};
private:
    std::string_view name = "";







};

#endif // BIOME_H
