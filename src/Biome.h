#pragma once
#include <memory>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "chunk.h"
#include "constants.hpp"

class Biome {
 private:
  int type;
  glm::ivec3 Biomepos;
  GLboolean displaybiome;

 public:
  std::unique_ptr<chunk> chunks[CHUNK_COUNTX][CHUNK_COUNTZ];
  Biome(int t, glm::ivec3 pos, GLboolean display);
  void RenderBiome(
      std::vector<std::unique_ptr<VertexArray>> &chunkva,
      std::vector<unsigned int> &cntblocks);
};
