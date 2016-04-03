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
typedef std::function<void(unsigned)> CharCallbackFunction;

class Keyboard {
public:
    static bool keyStates[GLFW_KEY_LAST];
    static std::vector<CharCallbackFunction> charPressedCallbackFunctions;
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

    static void CharCallback(GLFWwindow* window, unsigned codepoint)
    {
        for (auto callBack : charPressedCallbackFunctions) { callBack(codepoint); }
    }
};


#endif //PARTIALORDERPLANNER_KEYBOARD_H
