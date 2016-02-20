//
// Created by Dan on 2/19/2016.
//

#ifndef PARTIALORDERPLANNER_GRAPHICS_H
#define PARTIALORDERPLANNER_GRAPHICS_H

#include <stdio.h>
#include "Window.h"

class Graphics {
private:
    Window* window;
public:
    Graphics() : window(new Window())
    {

    }

    bool initialize();

    static void errorCallback(int error, const char* description)
    {
        fputs(description, stderr);
    }

    void update() {}

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
        // TODO: should I call window shutdown here?
        glfwTerminate();
    }

    //void handle()
};


#endif //PARTIALORDERPLANNER_GRAPHICS_H