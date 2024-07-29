#include <SnailFunctions.h>
#include <Window.h>
#include <Console.h>
#include <cstdlib>
#include <thread>

std::unordered_map<Key, int> keyMap = {
    { Key::A, GLFW_KEY_A },
    { Key::B, GLFW_KEY_B },
    { Key::C, GLFW_KEY_C },
    { Key::D, GLFW_KEY_D },
    { Key::E, GLFW_KEY_E },
    { Key::F, GLFW_KEY_F },
    { Key::G, GLFW_KEY_G },
    { Key::H, GLFW_KEY_H },
    { Key::I, GLFW_KEY_I },
    { Key::J, GLFW_KEY_J },
    { Key::K, GLFW_KEY_K },
    { Key::L, GLFW_KEY_L },
    { Key::M, GLFW_KEY_M },
    { Key::N, GLFW_KEY_N },
    { Key::O, GLFW_KEY_O },
    { Key::P, GLFW_KEY_P },
    { Key::Q, GLFW_KEY_Q },
    { Key::R, GLFW_KEY_R },
    { Key::S, GLFW_KEY_S },
    { Key::T, GLFW_KEY_T },
    { Key::U, GLFW_KEY_U },
    { Key::V, GLFW_KEY_V },
    { Key::W, GLFW_KEY_W },
    { Key::X, GLFW_KEY_X },
    { Key::Y, GLFW_KEY_Y },
    { Key::Z, GLFW_KEY_Z },
    { Key::Num0, GLFW_KEY_0 },
    { Key::Num1, GLFW_KEY_1 },
    { Key::Num2, GLFW_KEY_2 },
    { Key::Num3, GLFW_KEY_3 },
    { Key::Num4, GLFW_KEY_4 },
    { Key::Num5, GLFW_KEY_5 },
    { Key::Num6, GLFW_KEY_6 },
    { Key::Num7, GLFW_KEY_7 },
    { Key::Num8, GLFW_KEY_8 },
    { Key::Num9, GLFW_KEY_9 },
    { Key::Escape, GLFW_KEY_ESCAPE },
    { Key::Tab, GLFW_KEY_TAB },
    { Key::Space, GLFW_KEY_SPACE },
    { Key::RShift, GLFW_KEY_RIGHT_SHIFT },
    { Key::LShift, GLFW_KEY_RIGHT_SHIFT },
    { Key::RCtrl, GLFW_KEY_RIGHT_CONTROL },
    { Key::LCtrl, GLFW_KEY_LEFT_CONTROL },
    { Key::Left, GLFW_KEY_LEFT},
    { Key::Right, GLFW_KEY_RIGHT},
    { Key::Up, GLFW_KEY_UP},
    { Key::Down, GLFW_KEY_DOWN},
    { Key::Delete, GLFW_KEY_DELETE },
};

std::unordered_map<MouseKey, int> mouseKeyMap = {
    { MouseKey::LeftClick, GLFW_MOUSE_BUTTON_LEFT },
    { MouseKey::MiddleClick, GLFW_MOUSE_BUTTON_MIDDLE },
    { MouseKey::RightClick, GLFW_MOUSE_BUTTON_RIGHT }
};

bool GetKeyDown(Key key) {
    static std::map<Key, bool> keyState;
    static std::map<Key, bool> keyStatePrev;

    int glfwKey = keyMap[key];
    keyStatePrev[key] = keyState[key];
    keyState[key] = glfwGetKey(getWindow(), glfwKey) == GLFW_PRESS;

    return keyState[key] && !keyStatePrev[key];
}

bool GetKey(Key key)
{
    auto it = keyMap.find(key);
    if (it != keyMap.end())
    {
        int glfwKey = it->second;
        return glfwGetKey(getWindow(), glfwKey) == GLFW_PRESS;
    }
    return false;
}

bool GetMouseButtonDown(MouseKey mouseKey) {
    static std::map<MouseKey, bool> mouseButtonState;
    static std::map<MouseKey, bool> mouseButtonStatePrev;

    int glfwMouseButton = mouseKeyMap[mouseKey];
    mouseButtonStatePrev[mouseKey] = mouseButtonState[mouseKey];
    mouseButtonState[mouseKey] = glfwGetMouseButton(getWindow(), glfwMouseButton) == GLFW_PRESS;

    return mouseButtonState[mouseKey] && !mouseButtonStatePrev[mouseKey];
}

bool GetMouseButton(MouseKey mouseKey)
{
    auto it = mouseKeyMap.find(mouseKey);
    if (it != mouseKeyMap.end())
    {
        int glfwMouseButton = it->second;
        return glfwGetMouseButton(getWindow(), glfwMouseButton) == GLFW_PRESS;
    }
    return false;
}

double GetMouseInputHorizontal()
{
    double mouseX, mouseY;

    glfwGetCursorPos(getWindow(), &mouseX, &mouseY);

    return mouseX;
}

double GetMouseInputVertical()
{
    double mouseX, mouseY;

    glfwGetCursorPos(getWindow(), &mouseX, &mouseY);

    return mouseY;
}

unsigned int loadTexture(char const* path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << '\n';
        std::string pathString = path;
        Console::Log("Texture failed to load at path: " + pathString + "\n");
        stbi_image_free(data);
    }

    return textureID;
}

bool IsPointInsideObject(const glm::vec2& point, Object& object) {

    glm::vec2 objectPos(object.position.x, object.position.y);

    glm::vec2 minBounds = objectPos - object.scale * 0.5f;
    glm::vec2 maxBounds = objectPos + object.scale * 0.5f;

    // Check if the point is inside the bounds
    return (point.x >= minBounds.x && point.x <= maxBounds.x &&
        point.y >= minBounds.y && point.y <= maxBounds.y);
}

void setWindowIcon(GLFWwindow* window, const char* iconPath) {
    int width, height, channels;
    unsigned char* image = stbi_load(iconPath, &width, &height, &channels, 4);
    if (image) {
        GLFWimage images[1];
        images[0].width = width;
        images[0].height = height;
        images[0].pixels = image;
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(image);
    }
    else {
        std::cout << "Could not load window icon.";
    }
}

void InvokeFunction(std::function<void()> func, int delayMilliseconds) {
    // Create a new thread to execute the function after the delay
    std::thread([func, delayMilliseconds]() {
        // Sleep for the specified duration
        std::this_thread::sleep_for(std::chrono::seconds(delayMilliseconds));
        // Call the provided function
        func();
        }).detach(); // Detach the thread so it runs independently
}