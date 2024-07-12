#include "CameraController.h"


CameraController::CameraController(float aspectratio){
    m_AspectRatio = aspectratio;
    m_Camera = new Camera(-1.0, 1.0, -1.0, 1.0);
}

Camera* CameraController::GetCamera(){
    return m_Camera;
}

void CameraController::CameraInputs(){
    if (Input::IsKeyPressed(GLFW_KEY_W)) {
        m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetOrientation() * m_speed);
    } else if (Input::IsKeyPressed(GLFW_KEY_S)) {
        m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetOrientation() * m_speed);
    }
    
    if (Input::IsKeyPressed(GLFW_KEY_A)) {
        m_Camera->SetPosition(m_Camera->GetPosition() - m_speed * glm::normalize(glm::cross(m_Camera->GetOrientation(), m_Camera->GetUp())));
    } else if (Input::IsKeyPressed(GLFW_KEY_D)) {
        m_Camera->SetPosition(m_Camera->GetPosition() + m_speed * glm::normalize(glm::cross(m_Camera->GetOrientation(), m_Camera->GetUp())));
    }

    if (Input::IsKeyPressed(GLFW_KEY_UP)) {
        m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetUp() * m_speed);
    } else if (Input::IsKeyPressed(GLFW_KEY_DOWN)) {
        m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetUp() * m_speed);
    } 

    if (Input::IsKeyPressed(GLFW_KEY_X)) {
        m_Camera->SetPosition(glm::vec3(-80.0f, 0.0f, 0.0f));
        m_Camera->SetOrientation(glm::vec3(1.0f, 0.0f, 0.0f));
    } 

    if (Input::IsKeyPressed(GLFW_KEY_Y)) {
        m_Camera->SetPosition(glm::vec3(0.0f, -80.0f, 1.0f));
        m_Camera->SetOrientation(glm::vec3(0.0f, 1.0f, 0.0f));
    }

    if (Input::IsKeyPressed(GLFW_KEY_Z)) {
        m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, -80.0f));
        m_Camera->SetOrientation(glm::vec3(0.0f, 0.0f, 1.0f));
    }
    
    auto [x, y] = Input::GetMousePosition();
    float rotx = m_sensitivity * (float)(y - MousePos.y);
    float roty = m_sensitivity * (float)(x - MousePos.x);
    MousePos = glm::vec2(x, y);

    if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec3 right = glm::normalize(glm::cross(m_Camera->GetOrientation(), m_Camera->GetUp()));
        
        glm::quat qx = glm::normalize(glm::cross(glm::angleAxis(-rotx, right), glm::angleAxis(roty, m_Camera->GetUp())));
        m_Camera->SetOrientation(glm::normalize(qx * m_Camera->GetOrientation()));
    } 
}