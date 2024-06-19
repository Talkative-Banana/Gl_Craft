#include "utils.h"
#include "block.h"
#pragma once

class chunk{
private:
    GLuint side;
    GLboolean displaychunk;
    glm::vec3 chunkpos;
    GLuint filled[64][64][128];
    block *blocks[64][64][128];
public:
    GLuint count;
    std::vector<GLuint>  indices;
    std::vector<GLfloat> rendervert;

    chunk(GLuint s, glm::vec3 position, GLboolean display);
    ~chunk();

    void Render();
};
