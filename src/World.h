#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <unordered_set>

#include "Biome.h"
#include "Constants.hpp"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "Utils.h"
#include "VertexArray.h"

class World {
 private:
  int m_seed;
  glm::ivec3 m_worldpos;
  std::unordered_set<std::shared_ptr<Biome>> render_queue;
  std::queue<std::shared_ptr<Biome>> setup_queue;
  std::set<GLuint64> job_scheduled;
  void workerLoop();
  std::thread worker;
  std::atomic<bool> running{true};

  std::mutex setup_mutex;
  std::condition_variable setup_cv;

  std::queue<std::tuple<int, int, glm::ivec3>> job_queue;

 public:
  std::array<std::array<std::shared_ptr<Biome>, BIOME_COUNTZ>, BIOME_COUNTX> biomes;
  World(int seed, const glm::ivec3 &pos);
  void SetupWorld(glm::vec3 playerpos);
  bool isSolid(const glm::ivec3 &pos);
  bool isVisible(const glm::ivec3 &pos);
  Block *get_block_by_center(const glm::ivec3 &pos);
  std::shared_ptr<Chunk> get_chunk_by_center(const glm::ivec3 &pos);
  std::shared_ptr<Biome> get_biome_by_center(const glm::ivec3 &pos);
  void RenderWorld(bool firstRun);
  void Draw();
  void Update_queue(glm::vec3 playerpos);
};
