#pragma once

#include <memory>

#include "Biome.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "constants.hpp"
#include "utils.h"

class World {
 private:
  int m_seed;
  glm::ivec3 m_worldpos;

 public:
  std::unique_ptr<Biome> biomes[BIOME_COUNTX][BIOME_COUNTZ];
  World(int seed, const glm::ivec3 &pos);
  void SetupWorld();
  bool isSolid(const glm::ivec3 &pos);
  bool isVisible(const glm::ivec3 &pos);
  block *get_block_by_center(const glm::ivec3 &pos);
  chunk *get_chunk_by_center(const glm::ivec3 &pos);
  void RenderWorld(
      std::vector<std::unique_ptr<VertexArray>> &chunkva,
      std::vector<unsigned int> &cntblocks);
};
