#include <array>
#include <memory>

#include "CameraController.h"
#include "utils.h"
#pragma once

class Player {
 private:
  glm::vec3 m_position = glm::vec3(0.0, 0.0, -80.0);
  glm::vec3 m_forward = glm::vec3(0.0, 0.0, 1.0);
  glm::vec3 m_up = glm::vec3(0.0, 1.0, 0.0);
  float m_speed = 1.0f;
  float m_sensitivity = 0.002f;
  glm::vec2 MousePos = {0.0f, 0.0f};


 public:
  std::unique_ptr<CameraController> m_cameracontroller;

 public:
  Player(const glm::vec3& pos, const glm::vec3& dir);
  Player();

  void handle_input();
};
