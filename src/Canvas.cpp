#include <Canvas.h>
#include <stb_image.h>
#include <SnailFunctions.h>
#include <Console.h>
#include <EventSystem.h>
#include <ScriptFactory.h>

Canvas::Canvas(int width, int height) : m_width(width), m_height(height) 
{
    InitFreeType();
    InitOpenGL();
    LoadShader();
}

Canvas::~Canvas() 
{
    FT_Done_FreeType(ft);
    delete shader;
    delete buttonShader;
}

void Canvas::InitFreeType() 
{
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }
}

void Canvas::LoadFont(const std::string& fontPath) 
{
    if (Fonts.find(fontPath) != Fonts.end()) {
        // Font already loaded
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::map<char, Character> characters;
    for (GLubyte c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
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
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x) 
        };

        characters.insert(std::pair<char, Character>(c, character));
    }
    Fonts.insert(std::pair<std::string, std::map<char, Character>>(fontPath, characters));

    FT_Done_Face(face);
}

void Canvas::InitOpenGL() 
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float vertices[] = {
        // positions         // texture coords
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &buttonVAO);
    glGenBuffers(1, &buttonVBO);
    glGenBuffers(1, &buttonEBO);

    glBindVertexArray(buttonVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, buttonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buttonEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    projection = glm::ortho(0.0f, static_cast<float>(m_width), 0.0f, static_cast<float>(m_height));

    defaultTexture = loadTexture("Resources/Textures/DefaultTexture.png");
}

void Canvas::LoadShader() 
{
    shader = new Shader("Shaders/textvertex.shad", "Shaders/textfragment.shad");
    shader->use();
    shader->setMat4("projection", projection);
    buttonShader = new Shader("Shaders/uivertex.shad", "Shaders/uifragment.shad");
}

void Canvas::AddText(Text& text) 
{
    LoadFont(text.font);

    Text newText = { text.text, text.font, text.scale, text.position, text.color};
    texts.push_back(newText);
}

void Canvas::AddButton(Button& button)
{
    LoadFont(button.font);

    Button newButton = { button.text, button.font, button.position, button.buttonSize, button.textSize, button.backgroundColor, button.textColor };
    buttons.push_back(newButton);
}

void Canvas::RenderCanvas() 
{
    shader->use();
    glBindVertexArray(VAO);

    for (const auto& item : texts) {
        const std::string& text = item.text;
        const std::string& fontPath = item.font;
        float x = item.position.x;
        float y = item.position.y;
        glm::vec2 scale = item.scale; // Scaling factors for x and y
        glm::vec3 color = glm::vec3(item.color.x, item.color.y, item.color.z);

        shader->setVec3("textColor", color);

        // Calculate the total width of the text
        float totalWidth = 0.0f;
        for (auto c = text.begin(); c != text.end(); ++c) {
            Character ch = Fonts[fontPath][*c];
            totalWidth += (ch.Advance >> 6) * scale.x; // Advance is in 1/64 pixels
        }

        // Adjust the starting x position to center the text
        float startX = x - totalWidth / 2.0f;

        for (auto c = text.begin(); c != text.end(); ++c) {
            Character ch = Fonts[fontPath][*c];

            float xpos = startX + ch.Bearing.x * scale.x; // Apply x scaling
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale.y; // Apply y scaling

            float w = ch.Size.x * scale.x; // Apply x scaling
            float h = ch.Size.y * scale.y; // Apply y scaling
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            startX += (ch.Advance >> 6) * scale.x; // Move to the next character position
        }
    }

    for (const auto& item : buttons)
    {
        buttonShader->use();
        buttonShader->setTexture2D("texture1", defaultTexture, 0);

        // Calculate the transform matrix
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(item.position.x, item.position.y, 0.0f));
        transform = glm::scale(transform, glm::vec3(item.buttonSize.x, item.buttonSize.y, 1.0f));

        // Set uniforms
        buttonShader->setMat4("transform", transform);
        buttonShader->setMat4("projection", projection);
        buttonShader->setVec4("color", item.backgroundColor);
        buttonShader->setBool("isHovered", false);

        // Render the button
        glBindVertexArray(buttonVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        shader->use();
        glBindVertexArray(VAO);

        const std::string& text = item.text;
        const std::string& fontPath = item.font;

        float buttonWidth = item.buttonSize.x;
        float buttonHeight = item.buttonSize.y;
        float x = item.position.x - buttonWidth / 2.0f;
        float y = item.position.y - buttonHeight / 2.0f;

        glm::vec2 scale = item.textSize; // Scaling factors for x and y
        glm::vec3 color = glm::vec3(item.textColor.x, item.textColor.y, item.textColor.z);

        shader->setVec3("textColor", color);

        // Calculate the total width and height of the text
        float totalWidth = 0.0f;
        float maxHeight = 0.0f;
        for (auto c = text.begin(); c != text.end(); ++c) {
            Character ch = Fonts[fontPath][*c];
            totalWidth += (ch.Advance >> 6) * scale.x; // Advance is in 1/64 pixels
            maxHeight = std::max(maxHeight, static_cast<float>(ch.Size.y));
        }

        // Adjust the starting position to center the text
        float startX = x + (buttonWidth - totalWidth) / 2.0f;
        float startY = y + (buttonHeight - maxHeight * scale.y) / 2.0f;

        for (auto c = text.begin(); c != text.end(); ++c) {
            Character ch = Fonts[fontPath][*c];

            float xpos = startX + ch.Bearing.x * scale.x; // Apply x scaling
            float ypos = startY + (ch.Size.y - ch.Bearing.y) * scale.y; // Apply y scaling

            float w = ch.Size.x * scale.x; // Apply x scaling
            float h = ch.Size.y * scale.y; // Apply y scaling
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            startX += (ch.Advance >> 6) * scale.x;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(getWindow(), &mouseX, &mouseY);
        glm::vec2 mousePos = glm::vec2(mouseX, mouseY);

        if (isMouseOverButton(mousePos, item)) {
            if (GetMouseButtonDown(MouseKey::LeftClick)) { 
                ScriptFactory::Instance().GetEventSystem()->TriggerEvent(item.eventCallback);
            }
        }
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Canvas::Clear()
{
    texts.clear();
    buttons.clear();
}

bool Canvas::isMouseOverButton(const glm::vec2& mousePos, const Button& button)
{
    glm::vec2 objectPos = button.position;

    // Invert the Y position of the button
    float offset = getWindowSizeY() - 63;
    objectPos.y = offset - objectPos.y;

    glm::vec2 minBounds = objectPos - button.buttonSize * 0.5f;
    glm::vec2 maxBounds = objectPos + button.buttonSize * 0.5f;

    // Check if the point is inside the bounds
    return (mousePos.x >= minBounds.x && mousePos.x <= maxBounds.x &&
        mousePos.y >= minBounds.y && mousePos.y <= maxBounds.y);
}
