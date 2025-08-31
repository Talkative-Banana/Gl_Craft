#include "Biome.h"

#include "Renderer.h"

static void load_p(decltype(Biome::chunks) &chunks, glm::ivec3 &Biomepos, int i, bool display) {
  for (int j = 0; j < CHUNK_COUNTZ; j++) {
    int idx = CHUNK_COUNTZ * i + j;
    chunks[i][j] = std::make_unique<chunk>(idx, Biomepos, glm::ivec3(i, 0, j), true);
  }
}

static void render_p(
    decltype(Biome::chunks) &chunks,
    std::vector<std::unique_ptr<VertexArray>> &chunkva,
    std::vector<unsigned int> &cntblocks,
    int i) {
  for (int j = 0; j < CHUNK_COUNTZ; j++) {
    int idx = CHUNK_COUNTZ * i + j;
    auto &chunk = chunks[i][j];
    chunk->Render(chunkva, cntblocks);
  }
}

Biome::Biome(int t, glm::ivec3 pos, GLboolean display) {
  type = t;
  Biomepos = pos;
  displaybiome = display;

  GLuint idx = 0;
  std::array<std::thread, CHUNK_COUNTX> threads;
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    threads[i] = std::thread(load_p, std::ref(chunks), std::ref(Biomepos), i, true);
  }
  for (auto &thread : threads) thread.join();
}

void Biome::RenderBiome(
    std::vector<std::unique_ptr<VertexArray>> &chunkva,
    std::vector<unsigned int> &cntblocks) {
  if (!displaybiome) return;

  GLuint idx = 0;
  std::array<std::thread, CHUNK_COUNTX> threads;
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    threads[i] = std::thread(render_p, std::ref(chunks), std::ref(chunkva), std::ref(cntblocks), i);
  }

  for (auto &thread : threads) thread.join();

  // All openGL calls are to be made from main thread
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    for (int j = 0; j < CHUNK_COUNTZ; j++) {
      auto &chunk = chunks[i][j];
      VertexBufferLayout layout;
      layout.Push(GL_UNSIGNED_INT, 1);
      VertexBuffer vb(chunk->cube_vertices.data(), chunk->cube_vertices.size() * sizeof(GLuint));
      chunkva[chunk->id]->AddBuffer(vb, layout);
      IndexBuffer ib(chunk->cube_indices.data(), chunk->cube_indices.size());
      //  glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }
  }
}
