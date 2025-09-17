#include "World.h"

World::World(int seed, const glm::ivec3 &pos) : m_seed(seed), m_worldpos(pos) {
  worker = std::thread(&World::workerLoop, this);
};

Block *World::get_block_by_center(const glm::ivec3 &pos) {
  // get the biome
  // get x and z cords
  glm::ivec3 pos_cpy = pos - glm::ivec3(HALF_BLOCK_SIZE);
  int x = pos_cpy.x / (BLOCK_SIZE), y = pos_cpy.y / (BLOCK_SIZE), z = pos_cpy.z / (BLOCK_SIZE);

  if (pos_cpy.x < 0 || pos_cpy.y < 0 || pos_cpy.z < 0) return nullptr;

  if (y < 0 || y >= CHUNK_BLOCK_COUNT) return nullptr;

  // get the biome
  int biomex = x / (CHUNK_BLOCK_COUNT * CHUNK_COUNTX),
      biomez = z / (CHUNK_BLOCK_COUNT * CHUNK_COUNTZ);

  if (biomex >= BIOME_COUNTX || biomez >= BIOME_COUNTZ) return nullptr;

  auto biome = biomes[biomex][biomez];
  if (!biome) return nullptr;
  // get the chunk

  // get the chunk
  int chunkx = (x / CHUNK_BLOCK_COUNT) % CHUNK_COUNTX,
      chunkz = (z / CHUNK_BLOCK_COUNT) % CHUNK_COUNTZ;

  auto chunk = biome->chunks[chunkx][chunkz];

  if (!chunk) return nullptr;
  // get the block
  if (y >= CHUNK_BLOCK_COUNT) return nullptr;
  auto &block = chunk->blocks[x % CHUNK_BLOCK_COUNT][y][z % CHUNK_BLOCK_COUNT];
  return &block;
}

std::shared_ptr<Chunk> World::get_chunk_by_center(const glm::ivec3 &pos) {
  // get the biome
  // get x and z cords
  glm::ivec3 pos_cpy = pos - glm::ivec3(HALF_BLOCK_SIZE);
  int x = pos_cpy.x / (BLOCK_SIZE), y = pos_cpy.y / (BLOCK_SIZE), z = pos_cpy.z / (BLOCK_SIZE);

  if (pos_cpy.x < 0 || pos_cpy.y < 0 || pos_cpy.z < 0) return nullptr;

  if (y < 0 || y >= CHUNK_BLOCK_COUNT) return nullptr;

  // get the biome
  int biomex = x / (CHUNK_BLOCK_COUNT * CHUNK_COUNTX),
      biomez = z / (CHUNK_BLOCK_COUNT * CHUNK_COUNTZ);

  if (biomex >= BIOME_COUNTX || biomez >= BIOME_COUNTZ) return nullptr;

  auto biome = biomes[biomex][biomez];
  if (!biome) return nullptr;
  // get the chunk

  // get the chunk
  int chunkx = (x / CHUNK_BLOCK_COUNT) % CHUNK_COUNTX,
      chunkz = (z / CHUNK_BLOCK_COUNT) % CHUNK_COUNTZ;

  auto chunk = biome->chunks[chunkx][chunkz];

  return chunk ? chunk : nullptr;
}

std::shared_ptr<Biome> World::get_biome_by_center(const glm::ivec3 &pos) {
  // get the biome
  // get x and z cords
  glm::ivec3 pos_cpy = pos - glm::ivec3(HALF_BLOCK_SIZE);
  int x = pos_cpy.x / (BLOCK_SIZE), y = pos_cpy.y / (BLOCK_SIZE), z = pos_cpy.z / (BLOCK_SIZE);

  if (pos_cpy.x < 0 || pos_cpy.y < 0 || pos_cpy.z < 0) return nullptr;

  if (y < 0 || y >= CHUNK_BLOCK_COUNT) return nullptr;

  // get the biome
  int biomex = x / (CHUNK_BLOCK_COUNT * CHUNK_COUNTX),
      biomez = z / (CHUNK_BLOCK_COUNT * CHUNK_COUNTZ);

  if (biomex >= BIOME_COUNTX || biomez >= BIOME_COUNTZ) return nullptr;

  auto biome = biomes[biomex][biomez];
  if (!biome) return nullptr;

  return biome ? biome : nullptr;
}

bool World::isSolid(const glm::ivec3 &pos) {
  Block *b = get_block_by_center(pos);
  if (!b) return false;
  return (b && (((b->blmask) >> 15) & 1) == 1);
}

bool World::isVisible(const glm::ivec3 &pos) {
  Block *b = get_block_by_center(pos);
  return (b && (((b->blmask) >> 16) & 1) == 1);
}

void World::workerLoop() {
  while (running) {
    std::unique_lock<std::mutex> lock(setup_mutex);
    setup_cv.wait(lock, [this] { return !job_queue.empty() || !running; });

    if (!running) break;

    auto [i, j, pos] = job_queue.front();
    job_queue.pop();
    lock.unlock();

    // heavy work outside lock
    int idx = BIOME_COUNTX * i + j;
    auto biome = std::make_shared<Biome>(idx, pos, true);

    {
      std::lock_guard<std::mutex> g(setup_mutex);
      biomes[i][j] = biome;
      setup_queue.push(biome);
    }
  }
}

void World::SetupWorld(glm::vec3 playerpos) {
  // Do not set up for all the biomes
  std::thread thread;
  for (int i = 0; i < BIOME_COUNTX; i++) {
    for (int j = 0; j < BIOME_COUNTZ; j++) {
      int idx = BIOME_COUNTX * i + j;
      float common = CHUNK_COUNTX * CHUNK_BLOCK_COUNT * BLOCK_SIZE;
      glm::ivec3 biome_pos = glm::ivec3(common * i, 0, common * j);
      int X = biome_pos.x - playerpos.x, Z = biome_pos.z - playerpos.z;
      auto biome = biomes[i][j];
      if ((((X * X) + (Z * Z)) <= (RENDER_DISTANCE * RENDER_DISTANCE)) &&
          (job_scheduled.find(idx) == job_scheduled.end())) {
        // Costly move it to a seprate thread
        {
          std::lock_guard<std::mutex> lock(setup_mutex);
          job_queue.emplace(i, j, m_worldpos + biome_pos);
          job_scheduled.insert(idx);
        }
        setup_cv.notify_one();
      } else if (Z > RENDER_DISTANCE) {
        break;
      } else if (X > RENDER_DISTANCE) {
        i = BIOME_COUNTX;
        break;
      }
    }
  }

  auto q = setup_queue;
  // Render World with Inter Chunk walls
  RenderWorld(true);
  // Remove the inter chunk walls
  setup_queue = q;
  RenderWorld(false);
}

void World::RenderWorld(bool firstRun) {
  std::lock_guard<std::mutex> lock(setup_mutex);
  while (!setup_queue.empty()) {
    auto b = setup_queue.front();
    setup_queue.pop();
    b->RenderBiome(firstRun);  // firstRun
    if (render_queue.find(b) == render_queue.end()) render_queue.insert(b);
  }
}

void World::Draw() {
  // Do not render all the chunks just what biome wants to using its render_queue
  for (auto &biome : render_queue) {
    biome->Draw();
  }
}

void World::Update_queue(glm::vec3 playerpos) {
  for (auto &biome : render_queue) biome->Update_queue(playerpos);
}
