//
// Created by Dan on 2/19/2016.
//

#ifndef PARTIALORDERPLANNER_KEYBOARD_H
#define PARTIALORDERPLANNER_KEYBOARD_H

#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include <vector>

typedef std::function<void(int)> KeyCallbackFunction;

class Keyboard {
public:
    static bool keyStates[GLFW_KEY_LAST];
    static std::vector<KeyCallbackFunction> keyPressedCallbackFunctions;
    static std::vector<KeyCallbackFunction> keyReleasedCallbackFunctions;
    static std::vector<KeyCallbackFunction> keyRepeatedCallbackFunctions;

    Keyboard()
    {
        for (bool& key: keyStates)
        {
            key = false;
        }
    }

    // TODO: queue callbacks instead of immediately calling them?
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        switch (action)
        {
            case GLFW_PRESS:
            {
                keyStates[key] = true;
                for (auto callBack : keyPressedCallbackFunctions) { callBack(key); }
            } break;
            case GLFW_RELEASE:
            {
                keyStates[key] = false;
                for (auto callBack : keyReleasedCallbackFunctions) { callBack(key); }
            } break;
            case GLFW_REPEAT:
            {
                for (auto callBack : keyRepeatedCallbackFunctions) { callBack(key); }
            } break;
            default:
            {
                std::cout << "Keyboard::KeyCallback action: " << action << "not handled" << std::endl;
            }
        }
    }
};


#endif //PARTIALORDERPLANNER_KEYBOARD_H
