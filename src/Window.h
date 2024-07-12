#pragma once
#include "utils.h"

class Window{
private:
    GLFWwindow* window;
    int screen_height, screen_width;
public:
    Window(int sh, int sw);
    GLFWwindow* GetWindow() const;
    void setup(int sw, int sh);
    const int GetWidth() const;
    const int GetHeight() const;
    void SetWidth(int display_w);
    void SetHeight(int display_h);
};