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

void Mesh::setupModelTransformationCube() {
  // Modelling transformations (Model -> World coordinates)
  modelT = glm::mat4(1.0f);
  modelT = glm::translate(modelT, pos);
  modelT = glm::scale(modelT, glm::vec3(scale, scale, scale));
  // Rotate along y axis
  // modelT = glm::rotate(modelT, glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));

  // Pass on the modelling matrix to the vertex shader
  vModel_uniform = glGetUniformLocation(shaderProgram, "vModel");
  if (vModel_uniform == -1) {
    fprintf(stderr, "Could not bind location: vModel\n");
    exit(0);
  }
  glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void Mesh::setupViewTransformation(std::unique_ptr<CameraController> &occ) {
  // Viewing transformations (World -> Camera coordinates
  //  viewT = glm::lookAt(glm::vec3(camPosition), glm::vec3(0.0, 0.0, 0.0),
  //  glm::vec3(0.0, 1.0, 0.0));
  viewT = occ->GetCamera()->GetViewMatrix();

  // Pass-on the viewing matrix to the vertex shader
  vView_uniform = glGetUniformLocation(shaderProgram, "vView");
  if (vView_uniform == -1) {
    fprintf(stderr, "Could not bind location: vView\n");
    exit(0);
  }
  glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void Mesh::setupProjectionTransformation(std::unique_ptr<CameraController> &occ) {
  // Projection transformation
  projectionT = occ->GetCamera()->GetProjectionMatrix();

  // Pass on the projection matrix to the vertex shader
  vProjection_uniform = glGetUniformLocation(shaderProgram, "vProjection");
  if (vProjection_uniform == -1) {
    fprintf(stderr, "Could not bind location: vProjection\n");
    exit(0);
  }
  glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}

void Mesh::render(std::unique_ptr<CameraController> &camera_controller) {
  glUseProgram(shaderProgram);
  // Setup MVP matrix
  setupModelTransformationCube();
  setupViewTransformation(camera_controller);
  setupProjectionTransformation(camera_controller);

  vao->Bind();
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_positions.size()));
}
