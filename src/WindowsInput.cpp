#include "WindowsInput.h"

#include "Input.h"
#include "main.h"

Input* Input::s_Instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode) {
  auto window = static_cast<GLFWwindow*>(_window->GetWindow());
  auto state = glfwGetKey(window, keycode);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::WasKeyPressedImpl(int keycode) {
  static int oldStates[GLFW_KEY_LAST + 1] = {GLFW_RELEASE};

  auto window = static_cast<GLFWwindow*>(_window->GetWindow());
  int newState = glfwGetKey(window, keycode);

  bool pressed = (newState == GLFW_PRESS && oldStates[keycode] == GLFW_RELEASE);
  oldStates[keycode] = newState;  // update for next frame
  return pressed;
}

bool WindowsInput::IsMouseButtonPressedImpl(int button) {
  auto window = static_cast<GLFWwindow*>(_window->GetWindow());
  auto state = glfwGetMouseButton(window, button);
  return state == GLFW_PRESS;
}

bool WindowsInput::WasMouseButtonPressedImpl(int button) {
  static int oldStates[GLFW_MOUSE_BUTTON_LAST + 1] = {GLFW_RELEASE};

  auto window = static_cast<GLFWwindow*>(_window->GetWindow());
  int newState = glfwGetMouseButton(window, button);

  bool pressed = (newState == GLFW_PRESS && oldStates[button] == GLFW_RELEASE);

  oldStates[button] = newState;  // update for next frame
  return pressed;
}

std::pair<float, float> WindowsInput::GetMousePositionImpl() {
  auto window = static_cast<GLFWwindow*>(_window->GetWindow());
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  return {(float)xpos, (float)ypos};
}

float WindowsInput::GetMouseXImpl() {
  auto [x, y] = GetMousePositionImpl();
  return x;
}

float WindowsInput::GetMouseYImpl() {
  auto [x, y] = GetMousePositionImpl();
  return y;
}
