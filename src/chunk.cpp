#include "Renderer.h"
#include "chunk.h"

chunk::chunk(GLuint s, glm::vec3 position, GLboolean display){
    side = s;
    count = 0;
    chunkpos = position;
    displaychunk = display;
    memset(filled, 0, sizeof filled);
}

 GLboolean chunk::isSolid(glm::vec3 position){
    if((position.x >= 0) && (position.x < 64) && (position.y >= 0) && (position.y < 64) && (position.z >= 0) && (position.z < 128)){
        return filled[(GLuint)position.x][(GLuint)position.y][(GLuint)position.z];
    }
    return false;
}
// k blue i red j green
// ctrl x -> red facing me

GLuint chunk::RenderFace(glm::vec3 position){
    // 1 -> back face: 2 -> front face: 3 -> left face: 4 -> right face: 5 -> top face: 6 -> bottom face
    GLuint mask = 0;
    glm::vec3 tmp = position;
    for(GLuint face = 1; face <= 6; face++){
        if(face == 1){
            // No Need to draw back face if block behind is solid
            tmp.x -= 1.0;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp.x += 1.0;
        } else if(face == 2){
            // No Need to draw front face if block in front is solid
            tmp.x += 1.0;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp.x -= 1.0;
        } else if(face == 3){
            // No Need to draw left face if block in left is solid
            tmp.y -= 1.0;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp.y += 1.0;
        } else if(face == 4){
            // No Need to draw right face if block in right is solid
            tmp.y += 1.0;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp.y -= 1.0;
        } else if(face == 5){
            // No Need to draw top face if block on top is solid
            tmp.z += 1.0;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp.z -= 1.0;
        } else if(face == 6){
            // No Need to draw bottom face if block on bottom is solid
            tmp.z -= 1.0;
            if(!isSolid(tmp)) mask |= (1 << (face - 1));
            tmp.z += 1.0;
        }
    }
    return mask;
}

void chunk::Render(){
    if(!displaychunk) return;
    GLuint idx = 0;
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 128; j++){
            for(int k = 0; k < 64; k++){
                filled[0][0][0] = 1;
                filled[0][0][0] = 1;
                if(!filled[i][j][k]) continue;
                glm::vec3 ofs = {i * side, j * side, k * side};
                glm::vec3 pos = chunkpos + ofs;
                
                GLuint mask = chunk::RenderFace(pos);
                block b = block(side, pos, true); b.Render(mask);
                std::vector<GLfloat> blockrendervert = b.rendervert;
                std::vector<GLuint> blockindices = b.indices;
                for(auto x : blockrendervert) rendervert.push_back(x);
                for(auto x : blockindices) indices.push_back(idx + x);
                idx += 8, count++;
            }
        }
    }
}

chunk::~chunk(){}