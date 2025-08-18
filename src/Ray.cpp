#include "Ray.h"

glm::vec3 toBlockCenter(const glm::vec3 &p) {
  return glm::floor(p / 2.0f) * 2.0f + glm::vec3(1.0f);
}

bool Ray::did_hit(std::unique_ptr<World> &world) {
  // step size of 1 is fine as block are of 2 size
  // will cast for 32 steps
  glm::vec3 point = m_pos;
  for (size_t step = 0; step <= TOTAL_STEPS; step++) {
    // check if its inside a solid block
    glm::ivec3 center = toBlockCenter(point);
    if (world->isSolid(center)) {
      m_hitcords = center;
      return m_hit = true;
    }
    point += STEP_SIZE * m_dir;
  }
  return m_hit = false;
}

Ray screenPosToWorldRay(GLFWwindow *window, double mouseX, double mouseY,
                        const glm::mat4 &view, const glm::mat4 &projection) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  glm::ivec4 viewport(0, 0, width, height);

  // OpenGL expects Y flipped (0 is bottom)
  glm::vec3 screenNear(mouseX, height - mouseY,
                       0.0f);                         // depth = 0 -> near plane
  glm::vec3 screenFar(mouseX, height - mouseY, 1.0f); // depth = 1 -> far plane

  glm::vec3 worldNear = glm::unProject(screenNear, view, projection, viewport);
  glm::vec3 worldFar = glm::unProject(screenFar, view, projection, viewport);

  Ray ray(worldNear, glm::normalize(worldFar - worldNear));
  return ray;
}
