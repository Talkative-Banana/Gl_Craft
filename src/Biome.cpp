#include "Renderer.h"
#include "Biome.h"

Biome::Biome(int t, glm::ivec3 pos, GLboolean display){
    type = t;
    Biomepos = pos;
    displaybiome = display;
}

void Biome::RenderBiome(){
    if(!displaybiome) return;

    GLuint idx = 0;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            chunks[i][j] = new chunk(4 * i + j, Biomepos + glm::ivec3(0, 0, 0), true);
            chunks[i][j] -> Render();
        }
    }
}