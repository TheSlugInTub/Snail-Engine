#ifndef SNAIL_FUNCTIONS_H
#define SNAIL_FUNCTIONS_H

#include <unordered_map>
#include <stb_image.h>
#include <map>
#include <glm/glm.hpp>
#include <string>
#include <cstdlib>
#include <Camera.h>
#include <Object.h>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Utils class

enum class Key
{
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Escape, Tab, LShift, RShift, LCtrl, RCtrl, Space,
    Left, Right, Up, Down, Delete
};

enum class MouseKey
{
    LeftClick, RightClick, MiddleClick
};

bool GetKeyDown(Key key);

bool GetKey(Key key);

bool GetMouseButtonDown(MouseKey mouseKey);

bool GetMouseButton(MouseKey mouseKey);

double GetMouseInputHorizontal();

double GetMouseInputVertical();

unsigned int loadTexture(char const* path);

bool IsPointInsideObject(const glm::vec2& point, Object& object);

void setWindowIcon(GLFWwindow* window, const char* iconPath);

void InvokeFunction(std::function<void()> func, int delayMilliseconds);

#endif