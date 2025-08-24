#include "CameraController.h"

CameraController::CameraController(float aspectratio) {
  m_AspectRatio = aspectratio;
  m_Camera = new Camera(-1.0, 1.0, -1.0, 1.0);
}

void CameraController::SetAspectRatio(float aspectratio) {
  m_AspectRatio = aspectratio;
  m_Camera->SetAspectRatio(m_AspectRatio);
}

Camera* CameraController::GetCamera() {
  return m_Camera;
}

void CameraController::UpdateCamera(glm::vec3 pos, glm::vec3 dir) {
  m_Camera->SetPosition(pos);
  m_Camera->SetOrientation(dir);
}
