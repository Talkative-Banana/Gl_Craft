#include "WindowsInput.h"
#include "Input.h"
#include "main.h"

Input* Input::s_Instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode){
    auto window = static_cast<GLFWwindow*>(_window->GetWindow());
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseButtonPressedImpl(int button){
    auto window = static_cast<GLFWwindow*>(_window->GetWindow());
    auto state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::GetMousePositionImpl(){
    auto window = static_cast<GLFWwindow*>(_window->GetWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return {(float)xpos, (float)ypos};
}

float WindowsInput::GetMouseXImpl(){
    auto [x, y] = GetMousePositionImpl();
    return x;
}

float WindowsInput::GetMouseYImpl(){
    auto [x, y] = GetMousePositionImpl();
    return y;
}