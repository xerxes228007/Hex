#include "biome.h"

Biome::Biome(BiomeName b)
    :biome(b)
{
  switch (b) {
  case BiomeName::RIVER:
    name = "river";
    break;
  case BiomeName::DESERT:
    name = "desert";
    break;
  case BiomeName::PLAIN:
    name = "plain";
    break;
  case BiomeName::FOREST:
    name = "forest";
    break;
  case BiomeName::MOUNTAINS:
    name = "mountain";
    break;
  default:
    break;
  }
}
