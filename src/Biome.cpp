#include "Biome.h"

#include "Renderer.h"

Biome::Biome(int t, glm::ivec3 pos, GLboolean display) {
  type = t;
  Biomepos = pos;
  displaybiome = display;

  GLuint idx = 0;
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    for (int j = 0; j < CHUNK_COUNTZ; j++) {
      int idx = CHUNK_COUNTZ * i + j;
      chunks[i][j] = std::make_unique<chunk>(idx, Biomepos, glm::ivec3(i, 0, j), true);
    }
  }
}

void Biome::RenderBiome(
    std::vector<std::unique_ptr<VertexArray>> &chunkva,
    std::vector<unsigned int> &cntblocks) {
  if (!displaybiome) return;

  GLuint idx = 0;
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    for (int j = 0; j < CHUNK_COUNTZ; j++) {
      int idx = CHUNK_COUNTZ * i + j;
      chunks[i][j]->Render(chunkva, cntblocks);
    }
  }
}
