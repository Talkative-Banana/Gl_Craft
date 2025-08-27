#include <chrono>
#include <memory>
#include <vector>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "string.h"

class Mesh {
 public:
  Mesh(
      std::vector<float> &&positions,
      std::vector<int> &&uv_indices,
      std::vector<int> &&normal_indices)
      : m_positions(positions), m_uv_indices(uv_indices), m_normal_indices(normal_indices) {};
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&) noexcept = default;
  Mesh &operator=(Mesh &&) noexcept = default;

  std::vector<float> get_positions();
  std::vector<int> get_uv_indices();
  std::vector<int> get_normal_indices();

  void setup();
  void render();

 private:
  std::shared_ptr<VertexBuffer> vbo;
  std::shared_ptr<VertexArray> vao;
  std::vector<float> m_positions;
  std::vector<int> m_uv_indices;
  std::vector<int> m_normal_indices;
};
