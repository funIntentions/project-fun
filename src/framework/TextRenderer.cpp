//
// Created by Dan on 3/23/2016.
//

#include <iostream>
#include "TextRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H


TextRenderer::TextRenderer(GLuint width, GLuint height)
{
    textShader = new Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    textShader->use();
    // Init Shader
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(textShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // Setup vertex buffer object and array object for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::loadFont(std::string font, GLuint fontSize)
{
    characters.clear();

    FT_Library library;
    if (FT_Init_FreeType(&library))
        std::cout << "Error::FreeType: Could not init FreeType Library" << std::endl;

    FT_Face face;
    if (FT_New_Face(library, font.c_str(), 0, &face))
        std::cout << "Error::FreeType: Could not load font" << std::endl;

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Pre-load the first 128 ASCII characters from the font face
    for (GLubyte c = 0; c < 128; ++c)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "Error:FreeType: Failed to load glyph" << std::endl;
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Char character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
        };

        characters.insert({c, character});
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    // Cleanup
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

void TextRenderer::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour)
{
    // Activate render state
    textShader->use();
    glUniform3f(glGetUniformLocation(textShader->program, "textColour"), colour.r, colour.g, colour.b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (auto c = text.begin(); c != text.end(); ++c)
    {
        Char character = characters[*c];

        GLfloat xPos = x + character.bearing.x * scale;
        GLfloat yPos = y + (characters['H'].bearing.y - character.bearing.y) * scale;

        GLfloat w = character.size.x * scale;
        GLfloat h = character.size.y * scale;

        GLfloat vertices[6][4] = {
                {xPos    , yPos + h, 0.0, 1.0},
                {xPos + w, yPos    , 1.0, 0.0},
                {xPos    , yPos    , 0.0, 0.0},

                {xPos    , yPos + h, 0.0, 1.0},
                {xPos + w, yPos + h, 1.0, 1.0},
                {xPos + w, yPos    , 1.0, 0.0},
        };

        glBindTexture(GL_TEXTURE_2D, character.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance x for next glyph
        x += (character.advance >> 6) * scale; // Bit shifting by 6 gives us the number of pixels (1/64th times 2^6 = 64)
    }

    // Unbind
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
