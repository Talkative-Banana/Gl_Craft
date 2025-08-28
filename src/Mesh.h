#include <chrono>
#include <memory>
#include <vector>

#include "AssetVertex.h"
#include "CameraController.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "string.h"
#include "utils.h"

extern GLint vModel_uniform, vView_uniform, vProjection_uniform, side_uniform, worldpos_uniform,
    vColor_uniform;
extern glm::mat4 modelT, viewT, projectionT;

class Mesh {
 public:
  Mesh(std::vector<AssetVertex> &&vertex, unsigned int shaderprogram, float _scale, glm::vec3 _pos)
      : m_vertex(vertex), shaderProgram(shaderprogram), scale(_scale), pos(_pos) {};
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&) noexcept = default;
  Mesh &operator=(Mesh &&) noexcept = default;

  std::vector<AssetVertex> get_vertex();

  void setup();
  void render(std::unique_ptr<CameraController> &camera_controller);

 public:
  float scale = 0.025;
  glm::vec3 pos = glm::vec3(70.0f, 100.0f, 85.0f);

 private:
  unsigned int shaderProgram;
  std::shared_ptr<VertexArray> vao;
  std::shared_ptr<VertexBuffer> vbo;
  std::vector<AssetVertex> m_vertex;
  void setupModelTransformationCube();
  void setupViewTransformation(std::unique_ptr<CameraController> &occ);
  void setupProjectionTransformation(std::unique_ptr<CameraController> &occ);
};
