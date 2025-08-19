#include "Player.h"

Player::Player(const glm::vec3& pos, const glm::vec3& dir) : m_position(pos), m_forward(dir) {
  m_cameracontroller = std::make_unique<CameraController>(SCREEN_HEIGHT / SCREEN_WIDTH);
  m_cameracontroller->UpdateCamera(m_position, m_forward);
}

Player::Player() {
  m_cameracontroller = std::make_unique<CameraController>(SCREEN_HEIGHT / SCREEN_WIDTH);
  m_cameracontroller->UpdateCamera(m_position, m_forward);
}

void Player::handle_input() {
  if (Input::IsKeyPressed(GLFW_KEY_W)) {
    m_position = m_position + m_forward * m_speed;
  } else if (Input::IsKeyPressed(GLFW_KEY_S)) {
    m_position = m_position - m_forward * m_speed;
  }

  if (Input::IsKeyPressed(GLFW_KEY_A)) {
    m_position = m_position - m_speed * glm::normalize(glm::cross(m_forward, m_up));
  } else if (Input::IsKeyPressed(GLFW_KEY_D)) {
    m_position = m_position + m_speed * glm::normalize(glm::cross(m_forward, m_up));
  }

  if (Input::IsKeyPressed(GLFW_KEY_UP)) {
    m_position = m_position + m_up * m_speed;
  } else if (Input::IsKeyPressed(GLFW_KEY_DOWN)) {
    m_position = m_position - m_up * m_speed;
  }

  if (Input::IsKeyPressed(GLFW_KEY_X)) {
    m_position = glm::vec3(0.0f, 0.0f, -80.0f);
    m_forward = glm::vec3(-1.0f, 0.0f, 0.0f);
  }

  if (Input::IsKeyPressed(GLFW_KEY_Y)) {
    m_position = glm::vec3(0.0f, -80.0f, 1.0f);
    m_forward = glm::vec3(0.0f, 1.0f, 0.0f);
  }

  if (Input::IsKeyPressed(GLFW_KEY_Z)) {
    m_position = glm::vec3(0.0f, 0.0f, -80.0f);
    m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
  }

  auto [x, y] = Input::GetMousePosition();
  float rotx = m_sensitivity * (float)(y - MousePos.y);
  float roty = m_sensitivity * (float)(x - MousePos.x);
  MousePos = glm::vec2(x, y);

  if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    glm::vec3 right = glm::normalize(glm::cross(m_forward, m_up));

    // glm::quat qx = glm::normalize(
    //    glm::cross(glm::angleAxis(-rotx, right), glm::angleAxis(roty, m_Camera->GetUp())));
    // m_Camera->SetOrientation(glm::normalize(qx * m_Camera->GetOrientation()));

    glm::quat qx = glm::angleAxis(-rotx, right);
    glm::quat qy = glm::angleAxis(-roty, m_up);
    glm::quat rotation = glm::normalize(qy * qx);
    m_forward = glm::normalize(rotation * m_forward);
    m_up = glm::normalize(rotation * m_up);
  }


  // Update the camera as well
  m_cameracontroller->UpdateCamera(m_position, m_forward);
}
