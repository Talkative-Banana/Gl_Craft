# pragma once
#include "main.h"
#include "Input.h"
#include "Camera.h"

class CameraController{
private:
    float m_AspectRatio;
    float m_speed = 1.0f;
    float m_sensitivity = 0.002f;
    Camera *m_Camera = nullptr;
    glm::vec2 MousePos = {0.0f, 0.0f};
public:
    void CameraInputs();
    Camera *GetCamera();
    CameraController(float aspectratio);
};