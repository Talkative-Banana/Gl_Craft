#include "Window.h"

Window::Window(int sw, int sh) : screen_width(sw), screen_height(sh){
    setup(sw, sh);
}

void Window::setup(int sw, int sh){
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
void Window::SetWidth(int display_w){
    screen_width = display_w;
}

void Window::SetHeight(int display_h){
    screen_height = display_h;
}