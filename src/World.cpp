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

void World::RenderWorld(
    std::vector<std::unique_ptr<VertexArray>> &chunkva,
    std::vector<unsigned int> &cntblocks) {
  // Cant go much beyond due to the way blocks store cords
  for (int bi = 0; bi < BIOME_COUNTX; bi++) {
    for (int bj = 0; bj < BIOME_COUNTZ; bj++) {
      auto &b = biomes[bi][bj];
      b->RenderBiome();

      for (int _chunkx = 0; _chunkx < CHUNK_COUNTX; ++_chunkx) {
        for (int _chunkz = 0; _chunkz < CHUNK_COUNTZ; ++_chunkz) {
          chunkva.push_back(std::make_unique<VertexArray>());
          chunkva.back()->Bind();

          auto &tmp = b->chunks[_chunkx][_chunkz];
          const GLuint cnt = tmp->count;
          const GLuint rsize = static_cast<GLuint>(tmp->rendervert.size());

          // std::vector<int> FrustumCull(rsize, 0);
          GLuint vcnt = 0, icnt = 0;

          for (GLuint i = 0; i < rsize; ++i) {
            auto &vert_ind = tmp->rendervert[i];
            const auto &verts = vert_ind.first;
            const auto &inds = vert_ind.second;

            vcnt += static_cast<GLuint>(verts.size());
            // Diabled for now //  TODO:
            // if (FrustumCulling(verts[0])) {
            //  FrustumCull[i] = 1;
            //  std::cout << "Frustum Culled!\n";
            //  continue;
            // }
            icnt += static_cast<GLuint>(inds.size());
          }

          std::vector<GLuint> cube_vertices;
          cube_vertices.reserve(vcnt);

          std::vector<GLuint> cube_indices;
          cube_indices.reserve(icnt);

          for (GLuint i = 0; i < rsize; ++i) {
            // if (FrustumCull[i]) continue;
            auto &vert_ind = tmp->rendervert[i];
            const auto &verts = vert_ind.first;
            const auto &inds = vert_ind.second;

            cube_vertices.insert(cube_vertices.end(), verts.begin(), verts.end());
            cube_indices.insert(cube_indices.end(), inds.begin(), inds.end());
          }

          // std::cout << "[Memory Usage per chunk: "
          //          << ((cube_vertices.size() + cube_indices.size()) * sizeof(GLuint)) / 1e6
          //          << " MB]" << std::endl;

          cntblocks.push_back(icnt);

          VertexBufferLayout layout;
          layout.Push(GL_UNSIGNED_INT, 1);

          VertexBuffer vb(cube_vertices.data(), cube_vertices.size() * sizeof(GLuint));
          chunkva.back()->AddBuffer(vb, layout);

          IndexBuffer ib(cube_indices.data(), cube_indices.size());

          glBindBuffer(GL_ARRAY_BUFFER, 0);
          glBindVertexArray(0);
        }
      }
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

bool World::isSolid(const glm::ivec3 &pos) {
  block *b = get_block_by_center(pos);
  if (!b) return false;
  return (b && (((b->blmask) >> 15) & 1) == 1);
}

bool World::isVisible(const glm::ivec3 &pos) {
  block *b = get_block_by_center(pos);
  return (b && (((b->blmask) >> 16) & 1) == 1);
}
