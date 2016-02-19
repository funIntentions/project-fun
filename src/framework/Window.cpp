//
// Created by Dan on 2/19/2016.
//

#include "Window.h"

Window::Window()
{
    _width = 800;
    _height = 600;
    _title = "<{-}_{-}>";
}

bool Window::create()
{
    window = glfwCreateWindow(_width, _height, _title, NULL, NULL);

    if (!window)
    {
        glfwTerminate(); // TODO: do I need to call this in destructor
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