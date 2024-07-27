#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Shader.h>
#include <Camera.h>

struct Character 
{
    GLuint TextureID;  
    glm::ivec2 Size;   
    glm::ivec2 Bearing;
    GLuint Advance;    
};

struct Text
{
    std::string text;
    std::string font;
    glm::vec2 scale;
    glm::vec2 position;
    glm::vec4 color = glm::vec4(1.0f);

    bool operator==(const Text& other) const
    {
        if (text == other.text && font == other.font && scale == other.scale && position == other.position && color == other.color)
        {
            return true;
        }
        else
        { 
            return false;
        }
    }
};

struct Button
{
    std::string text;
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 backgroundColor;
    glm::vec4 textColor;

    bool operator==(const Button& other) const
    {
        return text == other.text && position == other.position && size == other.size &&
            backgroundColor == other.backgroundColor && textColor == other.textColor;
    }
};


class Canvas {
public:
    Canvas(int width, int height);
    ~Canvas();
    void AddText(Text& text);
    void RenderCanvas();
    void EditText(size_t index, const std::string& newText, float x, float y, float scale, glm::vec3 color);
    void Clear();

    std::vector<Text> texts;
    std::vector<Button> buttons;

private:
    void InitFreeType();
    void LoadFont(const std::string& fontPath);
    void InitOpenGL();
    void LoadShader();

    int m_width, m_height;
    std::map<std::string, std::map<char, Character>> Fonts;
    GLuint VAO, VBO;
    FT_Library ft;
    Shader* shader;
    glm::mat4 projection;
};