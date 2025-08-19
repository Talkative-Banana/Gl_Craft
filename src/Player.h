#include <array>
#include <memory>

#include "CameraController.h"
#include "utils.h"
#pragma once

class Player {
 private:
  glm::vec3 m_position;
  glm::vec3 m_forward;

 public:
  std::unique_ptr<CameraController> m_cameracontroller;

 public:
  Player(const glm::vec3& pos, const glm::vec3& dir);
  Player();

  void handle_input();
};
