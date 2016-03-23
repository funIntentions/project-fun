//
// Created by Dan on 3/23/2016.
//

#ifndef PARTIALORDERPLANNER_SHADER_H
#define PARTIALORDERPLANNER_SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader {
public:
    // program id
    GLuint program;

    // reads and builds the shader
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

    // use the program
    void use();
};


#endif //PARTIALORDERPLANNER_SHADER_H
