# pragma once
#include "main.h"
#include "Input.h"
#include "Camera.h"

class CameraController{
private:
    float m_AspectRatio;
    float m_speed = 1.0f;
    float m_sensitivity = 1.0f;
    Camera *m_Camera = nullptr;
    bool firstClick = true;
public:
    void CameraInputs();
    Camera *GetCamera();
    CameraController(float aspectratio);
};