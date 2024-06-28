#include "utils.h"
#include "block.h"
#pragma once

class chunk{
private:
    GLboolean displaychunk;
    glm::ivec3 chunkpos;
    block *blocks[32][32][32];
public:
    GLuint id;
    GLuint count;
    GLuint filled[32][32][32];
    std::vector<GLuint>  indices;
    std::vector<GLuint> rendervert;

    chunk(uint _id, glm::ivec3 position, GLboolean display);
    ~chunk();

    void Render();
    GLuint RenderFace(std::vector<GLint> position);
    GLboolean isSolid(std::vector<GLint> postion);
};
