#include "World.h"
World::World(int seed, const glm::ivec3 &pos)
    : m_seed(seed), m_worldpos(pos) {};

void World::SetupWorld() {

  GLuint idx = 0;
  for (int i = 0; i < BIOME_COUNTX; i++) {
    for (int j = 0; j < BIOME_COUNTY; j++) {
      int idx = 1 * i + j;
      biomes[i][j] = std::make_unique<Biome>(idx, m_worldpos, true);
    }
  }
}

void World::RenderWorld(std::vector<std::unique_ptr<VertexArray>> &chunkva,
                        std::vector<unsigned int> &cntblocks) {
  // Cant go much beyond due to the way blocks store cords
  for (int bi = 0; bi < BIOME_COUNTX; bi++) {
    for (int bj = 0; bj < BIOME_COUNTY; bj++) {
      auto &b = biomes[bi][bj];
      b->RenderBiome();

      for (int _chunkx = 0; _chunkx < CHUNK_COUNTX; ++_chunkx) {
        for (int _chunky = 0; _chunky < CHUNK_COUNTY; ++_chunky) {
          int idx = CHUNK_COUNTY * _chunkx + _chunky;
          chunkva[idx]->Bind();

          auto &tmp = b->chunks[_chunkx][_chunky];
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

            cube_vertices.insert(cube_vertices.end(), verts.begin(),
                                 verts.end());
            cube_indices.insert(cube_indices.end(), inds.begin(), inds.end());
          }

          std::cout << "[Memory Usage per chunk: "
                    << ((cube_vertices.size() + cube_indices.size()) *
                        sizeof(GLuint)) /
                           1e6
                    << " MB]" << std::endl;

          cntblocks.push_back(icnt);

          VertexBufferLayout layout;
          layout.Push(GL_UNSIGNED_INT, 1);

          VertexBuffer vb(cube_vertices.data(),
                          cube_vertices.size() * sizeof(GLuint));
          chunkva[idx]->AddBuffer(vb, layout);

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
  // get x and y cords
  int x = pos.x / BLOCK_SIZE, y = pos.y / BLOCK_SIZE, z = pos.z / BLOCK_SIZE;

  if (pos.x < 0 || pos.y < 0 || pos.z < 0)
    return nullptr;

  // get the biome
  int biomex = x / (CHUNK_BLOCK_COUNT * CHUNK_COUNTX),
      biomey = y / (CHUNK_BLOCK_COUNT * CHUNK_COUNTY);

  if (biomex >= BIOME_COUNTX || biomey >= BIOME_COUNTY)
    return nullptr;

  auto &biome = biomes[biomex][biomey];
  if (!biome)
    return nullptr;
  // get the chunk
  auto &chunk = biome->chunks[(x / CHUNK_BLOCK_COUNT) % CHUNK_COUNTX]
                             [(y / CHUNK_BLOCK_COUNT) % CHUNK_COUNTY];

  if (!chunk)
    return nullptr;
  // get the block
  auto &block = chunk->blocks[x % CHUNK_BLOCK_COUNT][y % CHUNK_BLOCK_COUNT]
                             [z % CHUNK_BLOCK_COUNT];
  return &block;
}

bool World::isSolid(const glm::ivec3 &pos) {
  block *b = get_block_by_center(pos);
  return (b && b->is_solid);
}
