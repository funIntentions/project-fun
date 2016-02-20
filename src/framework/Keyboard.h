//
// Created by Dan on 2/19/2016.
//

#ifndef PARTIALORDERPLANNER_KEYBOARD_H
#define PARTIALORDERPLANNER_KEYBOARD_H

#include <GLFW/glfw3.h>
#include <iostream>

class Keyboard {
public:
    static bool keyStates[GLFW_KEY_LAST];

    Keyboard()
    {
        for (bool& key: keyStates)
        {
            key = false;
        }
    }

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        switch (action)
        {
            case GLFW_PRESS:
            {
                keyStates[key] = true;
            } break;
            case GLFW_RELEASE:
            {
                keyStates[key] = false;
            } break;
            case GLFW_REPEAT:
            {
                std::cout << "Keyboard::KeyCallback GLFW_REPEAT not handled" << std::endl;
            }
            default:
            {
                std::cout << "Keyboard::KeyCallback action: " << action << "not handled" << std::endl;
            }
        }
    }
};


#endif //PARTIALORDERPLANNER_KEYBOARD_H
