#pragma once
#include <memory>

#include "chunk.h"
#include "constants.hpp"

class Biome {
 private:
  int type;
  glm::ivec3 Biomepos;
  GLboolean displaybiome;

 public:
  std::unique_ptr<chunk> chunks[CHUNK_COUNTX][CHUNK_COUNTY];
  Biome(int t, glm::ivec3 pos, GLboolean display);
  void RenderBiome();
};
