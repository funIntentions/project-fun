//
// Created by Dan on 2/19/2016.
//

#include <glew-1.13.0/include/GL/glew.h>
#include "Window.h"

Window::Window(unsigned width, unsigned height, const char* title) : _width(width), _height(height), _title(title)
{
}

bool Window::create()
{
    window = glfwCreateWindow(_width, _height, _title, NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    return true;
}

void Window::destroy()
{
    glfwSetWindowShouldClose(window, GL_TRUE);
    glfwDestroyWindow(window);
}