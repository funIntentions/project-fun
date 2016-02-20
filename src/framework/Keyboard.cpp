//
// Created by Dan on 2/19/2016.
//

#include "Keyboard.h"

bool Keyboard::keyStates[GLFW_KEY_LAST];
std::vector<KeyCallbackFunction> Keyboard::keyPressedCallbackFunctions;
std::vector<KeyCallbackFunction> Keyboard::keyReleasedCallbackFunctions;
std::vector<KeyCallbackFunction> Keyboard::keyRepeatedCallbackFunctions;
