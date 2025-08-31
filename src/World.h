#pragma once

#include <array>
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
  std::array<std::array<std::unique_ptr<Biome>, BIOME_COUNTZ>, BIOME_COUNTX> biomes;
  World(int seed, const glm::ivec3 &pos);
  void SetupWorld();
  bool isSolid(const glm::ivec3 &pos);
  bool isVisible(const glm::ivec3 &pos);
  block *get_block_by_center(const glm::ivec3 &pos);
  chunk *get_chunk_by_center(const glm::ivec3 &pos);
  void RenderWorld();
  void Draw();
};
