#include "Biome.h"

#include "Renderer.h"

static void load_p(decltype(Biome::chunks) &chunks, glm::ivec3 &Biomepos, int i, bool display) {
  for (int j = 0; j < CHUNK_COUNTZ; j++) {
    int idx = CHUNK_COUNTZ * i + j;
    chunks[i][j] = std::make_shared<chunk>(idx, Biomepos, glm::ivec3(i, 0, j), true);
  }
}

static void render_p(decltype(Biome::chunks) &chunks, int i, bool firstRun) {
  for (int j = 0; j < CHUNK_COUNTZ; j++) {
    int idx = CHUNK_COUNTZ * i + j;
    auto &_chunk = chunks[i][j];
    if (firstRun)
      _chunk->Render(1, firstRun, nullptr, nullptr, nullptr, nullptr);
    else {
      if (i == 0 || i == CHUNK_COUNTX - 1 || j == 0 || j == CHUNK_COUNTZ - 1) {
        // Have to check neigbouring biome
        std::shared_ptr<chunk> left;
        std::shared_ptr<chunk> front;
        std::shared_ptr<chunk> right;
        std::shared_ptr<chunk> back;
        if (i != CHUNK_COUNTX - 1) left = chunks[i + 1][j];
        if (i != 0) right = chunks[i - 1][j];
        if (j != CHUNK_COUNTZ - 1) front = chunks[i][j + 1];
        if (j != 0) back = chunks[i][j - 1];
        _chunk->Render(1, firstRun, left, front, right, back);
      } else {
        // Within currenct chunk
        _chunk->Render(
            1, firstRun, chunks[i + 1][j], chunks[i][j + 1], chunks[i - 1][j], chunks[i][j - 1]);
      }
    }
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

void Biome::RenderBiome(bool firstRun) {
  if (!displaybiome) return;

  GLuint idx = 0;
  std::array<std::thread, CHUNK_COUNTX> threads;
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    threads[i] = std::thread(render_p, std::ref(chunks), i, firstRun);
  }

  for (auto &thread : threads) thread.join();

  // All openGL calls are to be made from main thread
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    for (int j = 0; j < CHUNK_COUNTZ; j++) {
      auto &chunk = chunks[i][j];
      chunk->chunkva = std::make_unique<VertexArray>();
      chunk->chunkva->Bind();
      VertexBufferLayout layout;
      layout.Push(GL_UNSIGNED_INT, 1);
      VertexBuffer vb(chunk->cube_vertices.data(), chunk->cube_vertices.size() * sizeof(GLuint));
      chunk->chunkva->AddBuffer(vb, layout);
      IndexBuffer ib(chunk->cube_indices.data(), chunk->cube_indices.size());
      //  glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      render_queue.insert(chunk);
    }
  }
}

void Biome::Draw() {
  for (auto &chunk : render_queue) {
    chunk->Draw();
  }
}

void Biome::Update_queue(glm::vec3 playerpos) {
  for (int i = 0; i < CHUNK_COUNTX; i++) {
    for (int j = 0; j < CHUNK_COUNTZ; j++) {
      auto &chunk = chunks[i][j];
      glm::vec3 cpos = chunk->chunkpos;

      // If not displaying currently
      if (render_queue.find(chunk) == render_queue.end()) {
        int X = playerpos.x - cpos.x, Z = playerpos.z - cpos.z;
        if (((X * X) + (Z * Z)) <= RENDER_DISTANCE * RENDER_DISTANCE) {
          chunk->displaychunk = 1;
        } else {
          chunk->displaychunk = 0;  // Do not display farther than RENDER_DISTANCE
        }
      } else {  // If diplaying currently
        int X = playerpos.x - cpos.x, Z = playerpos.z - cpos.z;
        if (((X * X) + (Z * Z)) > RENDER_DISTANCE * RENDER_DISTANCE) {
          chunk->displaychunk = 0;
        } else {
          chunk->displaychunk = 1;  // Keep displaying near than RENDER_DISTANCE
        }
      }
    }
  }
}
