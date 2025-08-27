#include "Mesh.h"

std::vector<float> Mesh::get_positions() {
  return m_positions;
}

std::vector<int> Mesh::get_uv_indices() {
  return m_uv_indices;
}

std::vector<int> Mesh::get_normal_indices() {
  return m_normal_indices;
};

void Mesh::setup() {
  vao = std::make_shared<VertexArray>();
  vbo = std::make_shared<VertexBuffer>(
      m_positions.data(), static_cast<unsigned int>(m_positions.size() * sizeof(float)));

  VertexBufferLayout layout;
  layout.Push(GL_FLOAT, 3);  // just position (x, y, z)
  vao->AddBuffer(*vbo, layout);
}

void Mesh::render() {
  vbo->Bind();
  vao->Bind();
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_positions.size()));
}
