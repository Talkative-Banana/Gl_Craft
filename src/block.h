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
    GLuint side;
    glm::vec3 position;
    GLboolean is_solid;
    std::vector<glm::vec3> verticies;
    std::vector<GLfloat> rendervert;
    std::vector<GLuint> indices;
    std::vector<std::vector<GLuint>> faceindices = {{2, 3, 0, 0, 1, 2}, // Back
                                                    {4, 7, 6, 4, 6, 5}, // Front
                                                    {0, 4, 5, 0, 5, 1}, // Left
                                                    {3, 2, 6, 3, 6, 7}, // Right
                                                    {1, 5, 2, 5, 6, 2}, // Top
                                                    {4, 0, 3, 4, 3, 7}};// Bottom


    block(GLuint s, glm::vec3 pos, GLboolean solid);
    ~block();

    void Render(GLuint mask);
    void RenderFace(GLuint face);
};