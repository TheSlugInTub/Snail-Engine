#include "Window.h"
#include <iostream>

// Global window variable
GLFWwindow* g_window = nullptr;
int windowSizeX;
int windowSizeY;

GLFWwindow* initializeWindow(const char* title, int width, int height, bool isFullscreen) {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    g_window = glfwCreateWindow(width, height, title, isFullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (g_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(g_window);
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    windowSizeX = width;
    windowSizeY = height;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return g_window;
}

GLFWwindow* getWindow() {
    return g_window;
}

int getWindowSizeX() {
    return windowSizeX;
}

int getWindowSizeY() {
    return windowSizeY;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    windowSizeX = width;
    windowSizeY = height;

    if (width > 0 && height > 0) {
        glViewport(0, 0, width, height);
    }
}

float getAspectRatio() {
    int width, height;
    glfwGetWindowSize(g_window, &width, &height);

    if (width == 0 || height == 0) {
        // Handle the minimized window case
        return 1.0f; 
    }

    return (float)width / (float)height;
}
