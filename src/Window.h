#pragma once
#include "utils.h"

class Window{
private:
    GLFWwindow* window;
    int screen_height, screen_width;
public:
    Window(int sh, int sw);
    GLFWwindow* GetWindow() const;
    const int GetWidth() const;
    const int GetHeight() const;
};