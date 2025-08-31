#include "World.h"
World::World(int seed, const glm::ivec3 &pos) : m_seed(seed), m_worldpos(pos) {};

void World::SetupWorld() {

  GLuint idx = 0;
  for (int i = 0; i < BIOME_COUNTX; i++) {
    for (int j = 0; j < BIOME_COUNTZ; j++) {
      int idx = 1 * i + j;
      biomes[i][j] = std::make_unique<Biome>(idx, m_worldpos, true);
    }
  }
}

void World::RenderWorld() {
  // Cant go much beyond due to the way blocks store cords
  for (int bi = 0; bi < BIOME_COUNTX; bi++) {
    for (int bj = 0; bj < BIOME_COUNTZ; bj++) {
      auto &b = biomes[bi][bj];
      b->RenderBiome();
    }
  }
}

block *World::get_block_by_center(const glm::ivec3 &pos) {
  // get the biome
  // get x and z cords
  glm::ivec3 pos_cpy = pos - glm::ivec3(HALF_BLOCK_SIZE);
  int x = pos_cpy.x / (BLOCK_SIZE), y = pos_cpy.y / (BLOCK_SIZE), z = pos_cpy.z / (BLOCK_SIZE);

  if (pos_cpy.x < 0 || pos_cpy.y < 0 || pos_cpy.z < 0) return nullptr;

  // get the biome
  int biomex = x / (CHUNK_BLOCK_COUNT * CHUNK_COUNTX),
      biomez = z / (CHUNK_BLOCK_COUNT * CHUNK_COUNTZ);

  if (biomex >= BIOME_COUNTX || biomez >= BIOME_COUNTZ) return nullptr;

  auto &biome = biomes[biomex][biomez];
  if (!biome) return nullptr;
  // get the chunk

  // get the chunk
  int chunkx = (x / CHUNK_BLOCK_COUNT), chunkz = (z / CHUNK_BLOCK_COUNT);

  auto &chunk = biome->chunks[chunkx][chunkz];

  if (!chunk) return nullptr;
  // get the block
  if (y >= CHUNK_BLOCK_COUNT) return nullptr;
  auto &block = chunk->blocks[x % CHUNK_BLOCK_COUNT][y][z % CHUNK_BLOCK_COUNT];
  return &block;
}

chunk *World::get_chunk_by_center(const glm::ivec3 &pos) {
  // get the biome
  // get x and z cords
  glm::ivec3 pos_cpy = pos - glm::ivec3(HALF_BLOCK_SIZE);
  int x = pos_cpy.x / (BLOCK_SIZE), y = pos_cpy.y / (BLOCK_SIZE), z = pos_cpy.z / (BLOCK_SIZE);

  if (pos_cpy.x < 0 || pos_cpy.y < 0 || pos_cpy.z < 0) return nullptr;

  // get the biome
  int biomex = x / (CHUNK_BLOCK_COUNT * CHUNK_COUNTX),
      biomez = z / (CHUNK_BLOCK_COUNT * CHUNK_COUNTZ);

  if (biomex >= BIOME_COUNTX || biomez >= BIOME_COUNTZ) return nullptr;

  auto &biome = biomes[biomex][biomez];
  if (!biome) return nullptr;
  // get the chunk

  // get the chunk
  int chunkx = (x / CHUNK_BLOCK_COUNT), chunkz = (z / CHUNK_BLOCK_COUNT);

  auto &chunk = biome->chunks[chunkx][chunkz];

  if (!chunk) return nullptr;
  return chunk ? chunk.get() : nullptr;
}

bool World::isSolid(const glm::ivec3 &pos) {
  block *b = get_block_by_center(pos);
  if (!b) return false;
  return (b && (((b->blmask) >> 15) & 1) == 1);
}

bool World::isVisible(const glm::ivec3 &pos) {
  block *b = get_block_by_center(pos);
  return (b && (((b->blmask) >> 16) & 1) == 1);
}

void World::Draw() {
  for (auto &arr_biomes : biomes) {
    for (auto &biome : arr_biomes) {
      biome->Draw();
    }
  }
}
