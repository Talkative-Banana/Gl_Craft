#include "Biome.h"

#include "Renderer.h"

Biome::Biome(int t, glm::ivec3 pos, GLboolean display) {
  type = t;
  Biomepos = pos;
  displaybiome = display;
}

void Biome::RenderBiome() {
  if (!displaybiome) return;

  GLuint idx = 0;
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    for (int j = 0; j < CHUNK_COUNTY; j++) {
      int idx = CHUNK_COUNTY * i + j;
      chunks[i][j] = std::make_unique<chunk>(idx, Biomepos + glm::ivec3(i, 0, j), true);
      chunks[i][j]->Render();
    }
  }
}
