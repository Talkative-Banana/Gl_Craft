#pragma once
#include <chrono>
#include <memory>
#include <vector>

#include "AssetVertex.h"
#include "CameraController.h"
#include "IndexBuffer.h"
#include "Utils.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "string.h"

extern GLint vModel_uniform, vView_uniform, vProjection_uniform, side_uniform, worldpos_uniform,
    vColor_uniform;
extern glm::mat4 modelT, viewT, projectionT;

class Mesh {
 public:
  Mesh(
      std::vector<AssetVertex> &&vertex,
      unsigned int _shaderprogram,
      float _scale,
      float _angle,
      glm::vec3 _pos,
      glm::vec3 _axis)
      : m_vertex(vertex),
        shaderProgram(_shaderprogram),
        scale(_scale),
        angle_of_rot(_angle),
        pos(_pos),
        axis_of_rot(_axis) {};
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&) noexcept = default;
  Mesh &operator=(Mesh &&) noexcept = default;

  std::vector<AssetVertex> get_vertex();

  void setup();
  void render(std::unique_ptr<CameraController> &camera_controller);

 public:
  float scale = 0.025;
  float angle_of_rot = 180.0;
  glm::vec3 pos = glm::vec3(70.0f, 100.0f, 85.0f);
  glm::vec3 axis_of_rot = glm::vec3(0.0f, 1.0f, 1.0f);

 private:
  unsigned int shaderProgram;
  std::shared_ptr<VertexArray> vao;
  std::shared_ptr<VertexBuffer> vbo;
  std::vector<AssetVertex> m_vertex;
  void setupModelTransformationCube();
  void setupViewTransformation(std::unique_ptr<CameraController> &occ);
  void setupProjectionTransformation(std::unique_ptr<CameraController> &occ);
};
