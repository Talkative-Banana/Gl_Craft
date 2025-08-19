#include "Player.h"

Player::Player(const glm::vec3& pos, const glm::vec3& dir) : m_position(pos), m_forward(dir) {
  m_cameracontroller = std::make_unique<CameraController>(SCREEN_HEIGHT / SCREEN_WIDTH);
}

Player::Player() : m_position(0), m_forward(0) {
  m_cameracontroller = std::make_unique<CameraController>(SCREEN_HEIGHT / SCREEN_WIDTH);
}

void Player::handle_input() {
  m_cameracontroller->CameraInputs();
}
