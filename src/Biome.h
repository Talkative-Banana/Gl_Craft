#pragma once
#include <memory>
#include <thread>
#include <unordered_set>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "chunk.h"
#include "constants.hpp"

class Biome {
 private:
  int type;
  glm::ivec3 Biomepos;
  GLboolean displaybiome;
  std::unordered_set<std::shared_ptr<chunk>> render_queue;

 public:
  std::array<std::array<std::shared_ptr<chunk>, CHUNK_COUNTZ>, CHUNK_COUNTX> chunks;
  Biome(int t, glm::ivec3 pos, GLboolean display);
  void RenderBiome();
  void Draw();
  void Update_queue(glm::vec3 playerpos);
};
