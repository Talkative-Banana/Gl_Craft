# pragma once

class OrthographicCameraController{
private:
    float m_AspectRatio;
    float m_ZoomLevel = 1.0;
    OrthographicCamera m_Camera;
public:
    void OnUpdate(Timestep ts);

    OrthographicCameraController(flaot aspectratio);
};