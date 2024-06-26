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
        m_Camera->SetPosition(glm::vec3(-1000.0f, 0.0f, 0.0f));
    } 

    if (Input::IsKeyPressed(GLFW_KEY_Y)) {
        m_Camera->SetPosition(glm::vec3(0.0f, -1000.0f, 0.0f));
    }

    if (Input::IsKeyPressed(GLFW_KEY_Z)) {
        m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, -1000.0f));
    }

    if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        // Prevents camera from jumping on the first click
        if (firstClick) {
            glfwSetCursorPos(_window->GetWindow(), _window->GetWidth() / 2, _window->GetHeight() / 2);
            firstClick = false;
        }

        auto [x, y] = Input::GetMousePosition();

        float rotx = m_sensitivity * (float)(y - (_window->GetHeight() / 2)) / _window->GetHeight();
        float roty = m_sensitivity * (float)(x - (_window->GetWidth() / 2)) / _window->GetWidth();

        // Ensure the sensitivity is consistent by normalizing the direction vectors
        glm::vec3 right = glm::normalize(glm::cross(m_Camera->GetUp(), m_Camera->GetOrientation()));
        
        glm::quat qx = glm::angleAxis(glm::radians(-rotx), right);
        glm::vec3 newOrientation = glm::normalize(qx * m_Camera->GetOrientation());

        // Calculate the angle between the new orientation and the up vector
        float angleUp = glm::degrees(acos(glm::dot(glm::normalize(newOrientation), glm::normalize(m_Camera->GetUp()))));
        float angleDown = glm::degrees(acos(glm::dot(glm::normalize(newOrientation), glm::normalize(-m_Camera->GetUp()))));

        if (!((angleUp <= 5.0f) || (angleDown <= 5.0f))) {
            m_Camera->SetOrientation(newOrientation);
        }

        // Rotate around the up axis (world y-axis)
        glm::quat qy = glm::angleAxis(glm::radians(-roty), glm::normalize(m_Camera->GetUp()));
        m_Camera->SetOrientation(glm::normalize(qy * m_Camera->GetOrientation()));

        // Reset cursor position to the center of the window
        glfwSetCursorPos(_window->GetWindow(), _window->GetWidth() / 2, _window->GetHeight() / 2);
    } else if (glfwGetMouseButton(_window->GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        // Unhide cursor since camera is not looking around anymore
        glfwSetInputMode(_window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Make sure the next time the camera looks around it doesn't jump
        firstClick = true;
    }
}