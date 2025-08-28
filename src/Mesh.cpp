#include "Mesh.h"

std::vector<AssetVertex> Mesh::get_vertex() {
  return m_vertex;
}

void Mesh::setup() {
  vao = std::make_shared<VertexArray>();
  vbo = std::make_shared<VertexBuffer>(m_vertex.data(), m_vertex.size() * sizeof(AssetVertex));
  VertexBufferLayout Layout;
  Layout.Push(GL_FLOAT, 3);  // positions
  Layout.Push(GL_FLOAT, 3);  // normals
  vao->AddBuffer(*vbo, Layout);
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
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertex.size()));
}
