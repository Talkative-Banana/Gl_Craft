#include "Renderer.h"
#include "Biome.h"

Biome::Biome(int s, int t, glm::vec3 pos, GLboolean display){
    side = s;
    type = t;
    count = 0;
    Biomepos = pos;
    displaybiome = display;
}

void Biome::RenderBiome(){
    if(!displaybiome) return;

    GLuint idx = 0;
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            chunks[i][j] = new chunk(side, Biomepos + glm::vec3(32 * side * i, 0.0, 32 * side * j), true);
            chunks[i][j] -> Render();
            for(auto vert : chunks[i][j]->rendervert) brendervert.push_back(vert);
            for(auto indx : chunks[i][j]->indices)    bindices.push_back(idx + indx);
            idx += 8 * chunks[i][j]->count, count++;
        }
    }
}