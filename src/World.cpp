#include "World.h"

#include <fstream>

World::World(int seed, const glm::ivec3 &pos) : m_seed(seed), m_worldpos(pos) {
  // Read saved files if any
  worker = std::thread(&World::workerLoop, this);
  std::ifstream input_bin_file("save/save.bin", std::ios::binary);
  if (!input_bin_file) {
    std::cerr << "Failed to open save file.\n";
    return;
  }

  int count = 1;
  input_bin_file.read(reinterpret_cast<char *>(&count), sizeof(count));
  for (int i = 0; i < count; i++) {
    Chunk chunk;
    if (!chunk.Deserialize(input_bin_file)) break;
    std::cout << "Loaded a chunk with ID: " << chunk.save_id << std::endl;
    load_map[chunk.save_id] = std::move(chunk);
  }
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
    if (biomes[i][j]) continue;
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
  for (int i = 0; i < BIOME_COUNTX; i++) {
    for (int j = 0; j < BIOME_COUNTZ; j++) {
      int idx = BIOME_COUNTX * i + j;
      float common = CHUNK_COUNTX * CHUNK_BLOCK_COUNT * BLOCK_SIZE;
      glm::ivec3 biome_pos = glm::ivec3(common * i, 0, common * j);
      int X = biome_pos.x - playerpos.x, Z = biome_pos.z - playerpos.z;
      auto biome = biomes[i][j];
      if (((X <= RENDER_DISTANCE) && (Z <= RENDER_DISTANCE)) &&
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

void World::save_model(std::shared_ptr<Chunk> chunk) {
  std::vector<glm::ivec3> ref_array;
  for (int i = 0; i < CHUNK_BLOCK_COUNT; i++) {
    for (int k = 0; k < CHUNK_BLOCK_COUNT; k++) {
      for (int j = 0; j < CHUNK_BLOCK_COUNT; j++) {
        auto blk = chunk->blocks[i][j][k];
        if (blk.isSolid() && blk.is_ref()) {
          ref_array.push_back({i, j, k});
        }
      }
    }
  }

  if (ref_array.size() != 2) {
    std::cerr << "Reference blocks are not 2, ignoring model save\n";
  } else {
    std::cout << "Saving Model\n";
    std::ofstream save_model("models/tree.bin", std::ios::binary | std::ios::trunc);
    int countx = (ref_array[1].x - ref_array[0].x + 1),
        county = (ref_array[1].y - ref_array[0].y + 1),
        countz = (ref_array[1].z - ref_array[0].z + 1);

    std::cout << "Saved " << countx * county * countz << " blocks\n";
    save_model.write(reinterpret_cast<char *>(&countx), sizeof(countx));
    save_model.write(reinterpret_cast<char *>(&county), sizeof(county));
    save_model.write(reinterpret_cast<char *>(&countz), sizeof(countz));
    for (int i = ref_array[0].x; i <= ref_array[1].x; i++) {
      for (int k = ref_array[0].z; k <= ref_array[1].z; k++) {
        for (int j = ref_array[0].y; j <= ref_array[1].y; j++) {
          auto blk = chunk->blocks[i][j][k];
          if (blk.is_ref()) continue;
          save_model.write(reinterpret_cast<char *>(&blk.blmask), sizeof(blk.blmask));
        }
      }
    }
  }
}

void World::load_model(glm::ivec3 pos, std::string model) {
  // Read saved models files if any
  std::ifstream input_model_bin_file(model, std::ios::binary);
  if (!input_model_bin_file) {
    std::cerr << "Failed to open saved model file.\n";
    return;
  }

  auto get_neighbors = [this](glm::ivec3 vec) -> std::vector<std::shared_ptr<Chunk>> {
    std::shared_ptr<Chunk> left, front, right, back;
    left = get_chunk_by_center(
        vec + glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
    front = get_chunk_by_center(
        vec + glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
    right = get_chunk_by_center(
        vec - glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
    back = get_chunk_by_center(
        vec - glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
    return {left, front, right, back};
  };


  int countx = 0, county = 0, countz = 0;
  input_model_bin_file.read(reinterpret_cast<char *>(&countx), sizeof(countx));
  input_model_bin_file.read(reinterpret_cast<char *>(&county), sizeof(county));
  input_model_bin_file.read(reinterpret_cast<char *>(&countz), sizeof(countz));
  get_biome_by_center(pos)->dirtybit = 1;
  std::cout << "Loaded " << countx * county * countz << " blocks\n";
  std::shared_ptr<Chunk> _chunk;
  for (int i = 0; i < countx; i++) {
    for (int k = 0; k < countz; k++) {
      for (int j = 0; j < county; j++) {
        if ((i == 0 && j == 0 && k == 0) || (i == countx - 1 && j == county - 1 && k == countz - 1))
          continue;
        Block block;
        input_model_bin_file.read(reinterpret_cast<char *>(&block), sizeof(block));
        if (!block.isSolid()) continue;
        _chunk = get_chunk_by_center(
            {pos.x + i * BLOCK_SIZE, pos.y + j * BLOCK_SIZE, pos.z + k * BLOCK_SIZE});  // 63 1 63
        _chunk->dirtybit = 1;
        uint32_t preserve_mask = ((1 << 15) - 1);  // binary: 0000...01111111111111111 (15 bits set)
        uint32_t overwrite_mask = ~preserve_mask;

        Block &existing =
            _chunk->blocks[(pos.x / static_cast<int>(BLOCK_SIZE) + i) % CHUNK_BLOCK_COUNT]
                          [(pos.y / static_cast<int>(BLOCK_SIZE) + j) % CHUNK_BLOCK_COUNT]
                          [(pos.z / static_cast<int>(BLOCK_SIZE) + k) % CHUNK_BLOCK_COUNT];

        // Keep lower 15 bits of existing, replace rest from new
        existing.blmask = (existing.blmask & preserve_mask) | (block.blmask & overwrite_mask);
      }
    }
  }

  auto vec = pos;

  // If last block update adjacent chunk
  auto chunks1 = get_neighbors(vec);
  _chunk->Render(0, true, nullptr, nullptr, nullptr, nullptr);

  bool update_boundary = false;
  if (true) {
    auto chunks2 =
        get_neighbors(vec + glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
    if (chunks1[1]) {
      std::cout << "[FRONT] Updating neighbouring chunk\n";
      chunks1[1]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
      chunks1[1]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
    }
  }

  if (true) {
    auto chunks2 =
        get_neighbors(vec + glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
    if (chunks1[0]) {
      std::cout << "[LEFT] Updating neighbouring chunk\n";
      chunks1[0]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
      chunks1[0]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
    }
  }

  if (true) {
    auto chunks2 =
        get_neighbors(vec - glm::ivec3(0, 0, static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE)));
    if (chunks1[3]) {
      std::cout << "[BACK] Updating neighbouring chunk\n";
      chunks1[3]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
      chunks1[3]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
    }
  }

  if (true) {
    auto chunks2 =
        get_neighbors(vec - glm::ivec3(static_cast<int>(CHUNK_BLOCK_COUNT * BLOCK_SIZE), 0, 0));
    if (chunks1[2]) {
      std::cout << "[RIGHT] Updating neighbouring chunk\n";
      chunks1[2]->Render(0, true, nullptr, nullptr, nullptr, nullptr);
      chunks1[2]->Render(0, false, chunks2[0], chunks2[1], chunks2[2], chunks2[3]);
    }
  }
  _chunk->Render(0, false, chunks1[0], chunks1[1], chunks1[2], chunks1[3]);
}

// Saving Scope
void World::save() {
  std::ofstream save_file("save/save.bin", std::ios::binary | std::ios::trunc);
  // Save All the dirty chunks
  for (int i = 0; i < BIOME_COUNTZ; i++) {
    for (int j = 0; j < BIOME_COUNTX; j++) {
      auto biome = biomes[i][j];
      if (!biome || !biome->dirtybit) continue;
      for (int k = 0; k < CHUNK_COUNTZ; k++) {
        for (int l = 0; l < CHUNK_COUNTX; l++) {
          auto chunk = biome->chunks[k][l];
          if (!chunk || !chunk->dirtybit) continue;
          std::cout << "Saving chunk with ID: " << i << " " << j << " " << k << " " << l
                    << std::endl;
          save_map[chunk->save_id] = chunk;
        }
      }
    }
  }
  int count = save_map.size();
  save_file.write(reinterpret_cast<char *>(&count), sizeof(count));
  std::cout << "Saving " << count << " chunks\n";
  for (auto [id, chunk] : save_map) {
    std::cout << "Saving chunk with ID: " << id << std::endl;
    chunk->Serialize(save_file);
  }
  std::cout << "Game Saved\n";
}
