//
// Created by Dan on 2/19/2016.
//

#ifndef PARTIALORDERPLANNER_GRAPHICS_H
#define PARTIALORDERPLANNER_GRAPHICS_H

#include <stdio.h>
#include "Window.h"

class Graphics {
public:
    Window* window;

    Graphics() : window(new Window())
    {

    }

    bool initialize();

    static void errorCallback(int error, const char* description)
    {
        fputs(description, stderr);
    }

    bool windowIsOpen()
    {
        return !glfwWindowShouldClose(window->window);
    }

    void clearScreen()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void swapBuffers()
    {
        glfwSwapBuffers(window->window);
    }

    void shutdown()
    {
        window->destroy();
        delete window;
        glfwTerminate();
    }
};


#endif //PARTIALORDERPLANNER_GRAPHICS_H
