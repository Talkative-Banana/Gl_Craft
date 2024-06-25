#include "Window.h"

Window::Window(int sw, int sh) : screen_width(sw), screen_height(sh){
    window = setupWindow(sw, sh);
}

GLFWwindow* Window::GetWindow() const{
    return window;
}

const int Window::GetWidth() const{
    return screen_width;
}

const int Window::GetHeight() const{
    return screen_height;
}