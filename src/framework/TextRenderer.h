//
// Created by Dan on 3/23/2016.
//

#ifndef PARTIALORDERPLANNER_TEXTRENDERER_H
#define PARTIALORDERPLANNER_TEXTRENDERER_H

#include <glm/glm.hpp>
#include <unordered_map>
#include "Shader.h"

struct Char
{
    GLuint     textureID;  // ID handle of the glyph texture
    glm::ivec2 size;       // Size of glyph
    glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
    GLint64    advance;    // Offset to advance to next glyph
};

class TextRenderer {
public:
    // Holds characters
    std::unordered_map<GLchar, Char> characters;
    // Shader for rendering text
    Shader* textShader;
    // Constructor
    TextRenderer(GLuint width, GLuint height);
    // Deconstructor
    ~TextRenderer() {delete textShader;}
    // Pre-compiles characters from the given font
    void loadFont(std::string font, GLuint fontSize);
    // renders a string of text
    void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour = glm::vec3(1.0f));
private:
    // render state
    GLuint VAO, VBO;
};


#endif //PARTIALORDERPLANNER_TEXTRENDERER_H
