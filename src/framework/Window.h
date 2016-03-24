//
// Created by Dan on 2/19/2016.
//

#ifndef PARTIALORDERPLANNER_WINDOW_H
#define PARTIALORDERPLANNER_WINDOW_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
protected:
    unsigned int _width;
    unsigned int _height;
    const char* _title;
public:
    GLFWwindow* window;

    Window();
    virtual ~Window() {}
    bool create();
    void destroy();

    unsigned getWidth()
    {
        return _width;
    }

    unsigned getHeight()
    {
        return _height;
    }
};


#endif //PARTIALORDERPLANNER_WINDOW_H
