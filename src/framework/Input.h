//
// Created by Dan on 2/19/2016.
//

#ifndef PARTIALORDERPLANNER_INPUT_H
#define PARTIALORDERPLANNER_INPUT_H


#include "Keyboard.h"

class Input {
private:
    Keyboard _keyboard;
public:
    Input()
    {
    }

    ~Input() {}

    void initialize(GLFWwindow* window)
    {
        glfwSetKeyCallback(window, Keyboard::KeyCallback);
    }

    void pollForInput()
    {
        glfwPollEvents();
    }
};


#endif //PARTIALORDERPLANNER_INPUT_H
