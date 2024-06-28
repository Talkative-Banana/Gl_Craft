#include "Renderer.h"
#include "block.h"

block::block(glm::ivec3 pos, GLboolean solid){
    position = pos;
    is_solid = solid;
}

block::~block(){}

GLuint block::Mask(GLuint X, GLuint Y, GLuint Z, GLuint cent){
    GLuint mask = 0;
    // ccczzzzzzyyyyyyxxxxxx 7 all add
    mask |= X | (Y << 6) | (Z << 12) | (cent << 18);
    return mask;   
}

void block::GenerateVerticies(){
    GLuint x = position.x, y = position.y, z = position.z;
    // Vertex Position
    // Back Face
    verticies.push_back({Mask(x, y, z, 7)}); // 0
    // verticies.push_back({x - hside, y - hside, z - hside, x, y, z}); // 0 
    verticies.push_back({Mask(x, y + 1, z, 5)}); // 1
    // verticies.push_back({x - hside, y + hside, z - hside, x, y, z}); // 1
    verticies.push_back({Mask(x + 1, y + 1, z, 1)}); // 2
    // verticies.push_back({x + hside, y + hside, z - hside, x, y, z}); // 2
    verticies.push_back({Mask(x + 1, y, z, 3)}); // 3
    // verticies.push_back({x + hside, y - hside, z - hside, x, y, z}); // 3
    // Front Face
    verticies.push_back({Mask(x, y, z + 1, 6)}); // 4
    // verticies.push_back({x - hside, y - hside, z + hside, x, y, z}); // 4
    verticies.push_back({Mask(x, y + 1, z + 1, 4)}); // 5
    // verticies.push_back({x - hside, y + hside, z + hside, x, y, z}); // 5
    verticies.push_back({Mask(x + 1, y + 1, z + 1, 0)}); // 6
    // verticies.push_back({x + hside, y + hside, z + hside, x, y, z}); // 6
    verticies.push_back({Mask(x + 1, y, z + 1, 2)}); // 7
    // verticies.push_back({x + hside, y - hside, z + hside, x, y, z}); // 7
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
        // Texture-Center-Position
        rendervert.push_back(verticies[i][0]);
    }

    GLuint idx = 0;
    while(mask != 0){
        if(mask&1) block::RenderFace(idx);
        mask /= 2, idx++;
    }
}