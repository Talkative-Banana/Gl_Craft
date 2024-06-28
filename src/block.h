#include "utils.h"
#pragma once

struct Normals {
    glm::vec3 l = {-1.0,  0.0,  0.0};
    glm::vec3 r = { 1.0,  0.0,  0.0};
    glm::vec3 d = { 0.0,  0.0, -1.0};
    glm::vec3 u = { 0.0,  0.0,  1.0};
    glm::vec3 f = { 0.0,  1.0,  0.0};
    glm::vec3 b = { 0.0, -1.0,  0.0};
};

class block {
private:
    void GenerateVerticies();
public:
    glm::ivec3 position;
    GLboolean is_solid;
    std::vector<std::vector<GLuint>> verticies;
    std::vector<GLuint> rendervert;
    std::vector<GLuint> indices;
    std::vector<std::vector<GLuint>> faceindices = {{2, 3, 0, 2, 0, 1}, // Back
                                                    {7, 6, 5, 7, 5, 4}, // Front
                                                    {4, 5, 1, 4, 1, 0}, // Left
                                                    {3, 2, 6, 3, 6, 7}, // Right
                                                    {6, 2, 1, 6, 1, 5}, // Top
                                                    {3, 7, 4, 3, 4, 0}};// Bottom


    block(glm::ivec3 pos, GLboolean solid);
    ~block();

    void Render(GLuint mask);
    void RenderFace(GLuint face);
    GLuint Mask(GLuint X, GLuint Y, GLuint Z, GLuint cent);
};