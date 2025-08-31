#pragma once
#include <memory>
#include <thread>

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
  std::array<std::array<std::unique_ptr<chunk>, CHUNK_COUNTZ>, CHUNK_COUNTX> chunks;
  Biome(int t, glm::ivec3 pos, GLboolean display);
  void RenderBiome(
      std::vector<std::unique_ptr<VertexArray>> &chunkva,
      std::vector<unsigned int> &cntblocks);
};
