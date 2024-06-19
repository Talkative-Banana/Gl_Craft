#include "Renderer.h"
#include "block.h"

block::block(GLuint s, glm::vec3 pos, GLboolean solid){
    side = s;
    position = pos;
    is_solid = solid;
}

block::~block(){}

void block::GenerateVerticies(){
    GLfloat hside = side / 2.0;
    GLfloat x = position.x, y = position.y, z = position.z;
    // Vertex Position
    // Back Face
    verticies.push_back({x - hside, y - hside, z - hside}); // 0
    verticies.push_back({x - hside, y + hside, z - hside}); // 1
    verticies.push_back({x + hside, y + hside, z - hside}); // 2
    verticies.push_back({x + hside, y - hside, z - hside}); // 3
    // Front Face
    verticies.push_back({x - hside, y - hside, z + hside}); // 4
    verticies.push_back({x - hside, y + hside, z + hside}); // 5
    verticies.push_back({x + hside, y + hside, z + hside}); // 6
    verticies.push_back({x + hside, y - hside, z + hside}); // 7
    // Vertex Normal
}

void block::Render(){
    if(!is_solid) return;
    GenerateVerticies();
    for(int i = 0; i < 8; i++){
        rendervert.push_back(verticies[i].x);
        rendervert.push_back(verticies[i].y);
        rendervert.push_back(verticies[i].z);
    }
}