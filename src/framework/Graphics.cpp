//
// Created by Dan on 2/19/2016.
//

#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include "Graphics.h"

bool Graphics::initialize()
{
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
        std::cout << "Graphics::initialize - Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    if (!window->create())
    {
        std::cout << "Graphics::initialize - Failed to create window" << std::endl;
        exit(EXIT_FAILURE);
    }

    // TODO: initialize GLEW

    // Set GL settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    glDisable(GL_DEPTH_TEST);

    return true;
}