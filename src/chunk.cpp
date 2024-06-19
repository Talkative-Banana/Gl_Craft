#include "Renderer.h"
#include "chunk.h"

chunk::chunk(GLuint s, glm::vec3 position, GLboolean display){
    side = s;
    count = 0;
    chunkpos = position;
    displaychunk = display;
    memset(filled, 0, sizeof filled);
}

void chunk::Render(){
    if(!displaychunk) return;
    GLuint idx = 0;
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j++){
            for(int k = 0; k < 128; k++){
                filled[i][0][k] = 1;
                if(!filled[i][j][k]) continue;
                glm::vec3 ofs = {i * side, j * side, k * side};
                glm::vec3 pos = chunkpos + ofs;
                block b = block(side, pos, true); b.Render();
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