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
  // Gravity
  if (enable_gravity) {
    glm::vec3 v = glm::floor(m_position / BLOCK_SIZE) * BLOCK_SIZE + glm::vec3(HALF_BLOCK_SIZE);
    bool adjusted = false;
    if (world && (!(world->isSolid(v - glm::vec3(0, PLAYER_HEIGHT, 0)))) && (v.y > 1.0f)) {
      v.y -= BLOCK_SIZE;
      adjusted = true;
    }

    float height = v.y + BLOCK_SIZE - OFFSET;
    // Case 2: If we're inside a block -> snap up
    if (world && world->isSolid(v - glm::vec3(0, PLAYER_HEIGHT, 0)) && (v.y < height)) {
      v.y += BLOCK_SIZE;  // step up until clear
      adjusted = true;
    }

    if (adjusted) m_position.y = v.y;
  }

  glm::vec3 planarvec = glm::vec3(m_forward.x, 0.0, m_forward.z);
  if (Input::IsKeyPressed(GLFW_KEY_W)) {
    m_position = m_position + planarvec * m_speed;
  } else if (Input::IsKeyPressed(GLFW_KEY_S)) {
    m_position = m_position - planarvec * m_speed;
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

  if (Input::IsKeyPressed(GLFW_KEY_E)) {
    enable_gravity = false;
  }

  if (Input::IsKeyPressed(GLFW_KEY_R)) {
    enable_gravity = true;
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
  }


  // Update the camera as well
  m_cameracontroller->UpdateCamera(m_position, m_forward);
}
