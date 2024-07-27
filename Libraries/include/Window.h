#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* initializeWindow(const char* title, int width, int height, bool isFullscreen);
GLFWwindow* getWindow();
int getWindowSizeX();
int getWindowSizeY();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
float getAspectRatio();