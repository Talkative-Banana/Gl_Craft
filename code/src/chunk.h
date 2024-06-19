#include "utils.h"
#pragma once

class chunk{
private:
    GLboolean displaychunk;
    glm::vec3 chunkpos;
    GLuint filled[64][64][128];

public:
    chunk(bool display, glm::vec3 position);
    ~chunk();
};