# pragma once
#include "main.h"
#include "Input.h"
#include "Camera.h"

class CameraController{
private:
    float m_AspectRatio;
    float m_speed = 2.5f;
    float m_sensitivity = 2.5f;
    Camera *m_Camera = nullptr;
    bool firstClick = true;
public:
    void CameraInputs();
    Camera *GetCamera();
    CameraController(float aspectratio);
};