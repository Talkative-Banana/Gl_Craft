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

void block::RenderFace(GLuint face){
    if(face == 0){
        // Need to draw back face
        for(int i = 0; i < 6; i++) indices.push_back(faceindices[0][i]);
    } else if(face == 1){
        // Need to draw front face
        for(int i = 0; i < 6; i++) indices.push_back(faceindices[1][i]);
    } else if(face == 2){
        // Need to draw left face
        for(int i = 0; i < 6; i++) indices.push_back(faceindices[2][i]);
    } else if(face == 3){
        // Need to draw right face
        for(int i = 0; i < 6; i++) indices.push_back(faceindices[3][i]);
    } else if(face == 4){
        // Need to draw top face
        for(int i = 0; i < 6; i++) indices.push_back(faceindices[4][i]);
    } else{
        // Need to draw bottom face
        for(int i = 0; i < 6; i++) indices.push_back(faceindices[5][i]);
    }
}

void block::Render(GLuint mask){
    if(!is_solid) return;
    GenerateVerticies();
    for(int i = 0; i < 8; i++){
        rendervert.push_back(verticies[i].x);
        rendervert.push_back(verticies[i].y);
        rendervert.push_back(verticies[i].z);
    }

    GLuint idx = 0;
    while(mask != 0){
        if(mask&1) block::RenderFace(idx);
        mask /= 2, idx++;
    }
}