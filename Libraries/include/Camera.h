#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f);

    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void PrintValues();

    glm::mat4 GetViewMatrix();
    float GetZoom();
    void Shake(float duration, float intensity);
    void updateShake(float deltaTime);

    glm::vec3 screenToWorldSpace(const glm::vec2& screenCoords);
    glm::vec3 screenToWorldSpaceOriginal(const glm::vec2& screenCoords);
    glm::vec2 worldToScreenSpace(const glm::vec3& worldCoords);
    float GetAspectRatio();
    std::vector<glm::vec2> GetFrustumCorners();
    void SaveOriginal();
    void LoadOriginal();

    void SaveFrustumCorners();

    glm::vec3 Position;
    glm::vec3 OriginalPosition;
    glm::vec3 OriginalFront;
    glm::vec3 OriginalRight;
    glm::vec3 OriginalUp;
    glm::vec3 OriginalWorldUp;

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    std::vector<glm::vec2> corners;
private:
    void updateCameraVectors();

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    bool isShaking;
    float shakeDuration;
    float shakeIntensity;
    float shakeElapsed;
    glm::vec3 originalPosition;
};

#endif // CAMERA_H