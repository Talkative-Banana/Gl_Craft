# pragma once
#include "chunk.h"

class Biome{
private:
    int side;
    int type;
    glm::vec3 Biomepos;
    GLboolean displaybiome;
    chunk* chunks[2][2];
public:
    GLuint count;
    std::vector<GLuint>  bindices;
    std::vector<GLfloat> brendervert;

    Biome(int s, int t, glm::vec3 pos, GLboolean display);
    void RenderBiome();
};