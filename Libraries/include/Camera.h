#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <json.hpp>
#include <fstream>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct CameraSave {
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;
    glm::vec3 WorldUp;

    nlohmann::json CamToJson() const
    {
        nlohmann::json j = {
            {"CamPosition", {Position.x, Position.y, Position.z}},
            {"CamFront", {Front.x, Front.y, Front.z}},
            {"CamRight", {Right.x, Right.y, Right.z}},
            {"CamUp", {Up.x, Up.y, Up.z}},
            {"CamWorldUp", {WorldUp.x, WorldUp.y, WorldUp.z}}
        };

        return j;
    }

    void JsonToCam(nlohmann::json& j)
    {
        Position = { j["CamPosition"][0], j["CamPosition"][1], j["CamPosition"][2] };
        Front = { j["CamFront"][0], j["CamFront"][1], j["CamFront"][2] };
        Right = { j["CamRight"][0], j["CamRight"][1], j["CamRight"][2] };
        Up = { j["CamUp"][0], j["CamUp"][1], j["CamUp"][2] };
        WorldUp = { j["CamWorldUp"][0], j["CamWorldUp"][1], j["CamWorldUp"][2] };
    }
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

    void updateCameraVectors();

    CameraSave Save;

    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Zoom;
    float Yaw;
    float Pitch;

    std::vector<glm::vec2> corners;
private:

    float MovementSpeed;
    float MouseSensitivity;

    bool isShaking;
    float shakeDuration;
    float shakeIntensity;
    float shakeElapsed;
    glm::vec3 originalPosition;
};

#endif // CAMERA_H