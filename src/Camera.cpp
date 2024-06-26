#include "Camera.h"
#include<iostream>
Camera::Camera(float left, float right, float bottom, float top)
:m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 5000.0f)), m_ViewMatrix(1.0f){
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::RecalculateViewMatrix(){
    glm::mat4 transform = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);

    // Perspective Addition
    m_ProjectionMatrix  = glm::perspective(45.0f, 1.0f, 1.0f, 1000.0f);

    m_ViewMatrix = glm::inverse(transform);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}