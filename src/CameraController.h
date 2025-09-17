#pragma once
#include "Camera.h"
#include "Input.h"
#include "Main.h"

class CameraController {
 private:
  float m_AspectRatio;
  Camera *m_Camera = nullptr;

 public:
  void UpdateCamera(glm::vec3 pos, glm::vec3 dir);
  Camera *GetCamera();
  CameraController(float aspectratio);
  void SetAspectRatio(float aspectratio);
};
