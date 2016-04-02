//
// Created by Dan on 2/19/2016.
//

#ifndef PARTIALORDERPLANNER_GRAPHICS_H
#define PARTIALORDERPLANNER_GRAPHICS_H

#include "Window.h"
#include "glm/glm.hpp"
#include "TextRenderer.h"
#include <stdio.h>
#include <unordered_map>

class Graphics {
public:
    Window* window;
    TextRenderer* textRenderer;

    Graphics(unsigned width, unsigned height, const char* title) : window(new Window(width, height, title))
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
        glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
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
        delete textRenderer;
        glfwTerminate();
    }
};


#endif //PARTIALORDERPLANNER_GRAPHICS_H
