# pragma once
#include "chunk.h"

class Biome{
private:
    int type;
    glm::ivec3 Biomepos;
    GLboolean displaybiome;
public:
    chunk* chunks[4][4];
    Biome(int t, glm::ivec3 pos, GLboolean display);
    void RenderBiome();
};