#include "Renderer.h"
#include "chunk.h"

chunk::chunk(uint _id, glm::ivec3 position, GLboolean display){
    id = _id;
    count = 0;
    chunkpos = position;
    displaychunk = display;
    memset(filled, 0, sizeof filled);
}

 GLboolean chunk::isSolid(std::vector<GLint> position){
    if((position[0] >= 0) && (position[0] < 32) && (position[1] >= 0) && (position[1] < 32) && (position[2] >= 0) && (position[2] < 32)){
        return filled[position[0]][position[1]][position[2]];
    }
    return false;
}
// k blue i red j green
// ctrl x -> red facing me

GLuint chunk::RenderFace(std::vector<GLint> position){
    // 1 -> back face: 2 -> front face: 3 -> left face: 4 -> right face: 5 -> top face: 6 -> bottom face
    GLuint mask = 0;
    std::vector<GLint> tmp = position;
    for(GLuint face = 1; face <= 6; face++){
        if(face == 1){
            // No Need to draw back face if block behind is solid
            tmp[2] -= 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[2] += 1;
        } else if(face == 2){
            // No Need to draw front face if block in front is solid
            tmp[2] += 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[2] -= 1;
        } else if(face == 3){
            // No Need to draw left face if block in left is solid
            tmp[0] -= 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[0] += 1;
        } else if(face == 4){
            // No Need to draw right face if block in right is solid
            tmp[0] += 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[0] -= 1;
        } else if(face == 5){
            // No Need to draw top face if block on top is solid
            tmp[1] += 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[1] -= 1;
        } else if(face == 6){
            // No Need to draw bottom face if block on bottom is solid
            tmp[1] -= 1;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp[1] += 1;
        }
    }
    return mask;
}

void chunk::Render(){
    if(!displaychunk) return;
    GLuint idx = 0;
    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 32; j++){
            for(int k = 0; k < 32; k++){
                filled[i][j][k] = 1;
            }
        }
    }

    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 32; j++){
            for(int k = 0; k < 32; k++){
                // filled[0][0][0] = 1;
                if(!filled[i][j][k]) continue;
                glm::ivec3 ofs = {i, j, k};
                glm::ivec3 pos = chunkpos + ofs;
                std::vector<GLint> Idx = {i, j, k};
                GLuint mask = chunk::RenderFace(Idx);
                blocks[i][j][k] = new block(pos, true); blocks[i][j][k]->Render(mask);
                std::vector<GLuint> blockrendervert = blocks[i][j][k]->rendervert;
                std::vector<GLuint> blockindices = blocks[i][j][k]->indices;
                for(auto x : blockrendervert) rendervert.push_back(x);
                for(auto x : blockindices) indices.push_back(idx + x);
                idx += 8, count++;
            }
        }
    }
}

chunk::~chunk() {}
