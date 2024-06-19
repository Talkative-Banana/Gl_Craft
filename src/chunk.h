#include "utils.h"
#include "block.h"
#pragma once

class chunk{
private:
    GLuint side;
    GLboolean displaychunk;
    glm::vec3 chunkpos;
    block* blocks[64][128][64];
public:
    GLuint count;
    GLuint filled[64][128][64];
    std::vector<GLuint>  indices;
    std::vector<GLfloat> rendervert;

    chunk(GLuint s, glm::vec3 position, GLboolean display);
    ~chunk();

    void Render();
    GLuint RenderFace(glm::vec3 position);
    GLboolean isSolid(glm::vec3 postion);
};