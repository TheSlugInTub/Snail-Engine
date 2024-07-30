#include "Camera.h"
#include <iostream>
#include <glm/gtc/random.hpp>
#include <Window.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(1.2f),
    MouseSensitivity(0.1f),
    Zoom(45.0f),
    isShaking(false),
    shakeDuration(0.0f),
    shakeIntensity(0.0f),
    shakeElapsed(0.0f),
    originalPosition(position) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == CameraMovement::FORWARD)
        Position += Front * velocity;
    if (direction == CameraMovement::BACKWARD)
        Position -= Front * velocity;
    if (direction == CameraMovement::LEFT)
        Position -= Right * velocity;
    if (direction == CameraMovement::RIGHT)
        Position += Right * velocity;
    if (direction == CameraMovement::UP)
        Position += Up * velocity;
    if (direction == CameraMovement::DOWN)
        Position -= Up * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

float Camera::GetZoom() {
    return Zoom;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::Shake(float duration, float intensity) {
    isShaking = true;
    shakeDuration = duration;
    shakeIntensity = intensity;
    shakeElapsed = 0.0f;
    originalPosition = Position;
}

void Camera::updateShake(float deltaTime) {
    if (isShaking) {
        shakeElapsed += deltaTime;
        if (shakeElapsed < shakeDuration) {
            glm::vec3 shakeOffset = glm::ballRand(shakeIntensity);
            Position = originalPosition + shakeOffset;
        }
        else {
            isShaking = false;
            Position = originalPosition;
        }
    }
}

glm::vec3 Camera::screenToWorldSpace(const glm::vec2& screenCoords) {

    glm::vec2 viewportSize(getWindowSizeX(), getWindowSizeY());

    float zDepth = Position.z * 1.9f;

    glm::mat4 projection = glm::perspective(glm::radians(GetZoom()), getAspectRatio(), 0.1f, 100.0f);
    glm::mat projInverse = glm::inverse(projection);

    float mouse_x = screenCoords.x;
    float mouse_y = screenCoords.y;

    float ndc_x = (2.0f * mouse_x) / getWindowSizeX() - 1.0f;
    float ndc_y = 1.0f - (2.0f * mouse_y) / getWindowSizeY();

    float focal_length = 1.0f / glm::tan(glm::radians(45.0f / 2.0f));
    float ar = (float)getWindowSizeY() / (float)getWindowSizeX();
    glm::vec3 ray_view(ndc_x / focal_length, (ndc_y * ar) / focal_length, 1.0f);

    glm::vec4 ray_ndc_4d(ndc_x, ndc_y, 1.0f, 1.0f);
    glm::vec4 ray_view_4d = projInverse * ray_ndc_4d;

    glm::vec4 view_space_intersect = glm::vec4(ray_view * zDepth, 1.0f);

    glm::mat4 view = GetViewMatrix();
    glm::mat4 viewInverse = glm::inverse(view);

    glm::vec4 point_world = viewInverse * view_space_intersect;
    
    return glm::vec3(point_world);
}

glm::vec3 Camera::screenToWorldSpaceOriginal(const glm::vec2& screenCoords) {

    glm::vec2 viewportSize(getWindowSizeX(), getWindowSizeY());

    float zDepth = Save.Position.z * 1.9f;

    glm::mat4 projection = glm::perspective(glm::radians(GetZoom()), getAspectRatio(), 0.1f, 100.0f);
    glm::mat projInverse = glm::inverse(projection);

    float mouse_x = screenCoords.x;
    float mouse_y = screenCoords.y;

    float ndc_x = (2.0f * mouse_x) / getWindowSizeX() - 1.0f;
    float ndc_y = 1.0f - (2.0f * mouse_y) / getWindowSizeY();

    float focal_length = 1.0f / glm::tan(glm::radians(45.0f / 2.0f));
    float ar = (float)getWindowSizeY() / (float)getWindowSizeX();
    glm::vec3 ray_view(ndc_x / focal_length, (ndc_y * ar) / focal_length, 1.0f);

    glm::vec4 ray_ndc_4d(ndc_x, ndc_y, 1.0f, 1.0f);
    glm::vec4 ray_view_4d = projInverse * ray_ndc_4d;

    glm::vec4 view_space_intersect = glm::vec4(ray_view * zDepth, 1.0f);

    glm::mat4 view = glm::lookAt(Save.Position, Save.Position + Save.Front, Save.Up);
    glm::mat4 viewInverse = glm::inverse(view);

    glm::vec4 point_world = viewInverse * view_space_intersect;

    return glm::vec3(point_world);
}

glm::vec2 Camera::worldToScreenSpace(const glm::vec3& worldCoords) {

    glm::vec4 viewport(0.0f, 0.0f, getWindowSizeX(), getWindowSizeY());

    // Compute the model-view-projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(GetZoom()), getAspectRatio(), 0.1f, 100.0f);
    glm::mat4 view = GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f); // Assuming no model transformation; adjust if necessary
    glm::mat4 mvp = projection * view * model;

    // Convert world coordinates to clip space
    glm::vec4 worldCoordsHomogeneous(worldCoords, 1.0f); // Convert to homogeneous coordinates
    glm::vec4 clipCoords = mvp * worldCoordsHomogeneous;

    // Convert from clip space to NDC
    glm::vec3 ndc = clipCoords / clipCoords.w; // Perspective divide

    // Convert from NDC to screen space
    glm::vec2 screenCoords;
    screenCoords.x = (ndc.x * 0.5f + 0.5f) * viewport.z + viewport.x;
    screenCoords.y = (ndc.y * 0.5f + 0.5f) * viewport.w + viewport.y;

    return screenCoords;
}

float Camera::GetAspectRatio()
{
    return static_cast<float>(getWindowSizeX()) / static_cast<float>(getWindowSizeY());
}

std::vector<glm::vec2> Camera::GetFrustumCorners() {
    std::vector<glm::vec2> corners;

    // Define the screen space corners
    glm::vec2 bottomLeft(0.0f, 0.0f);
    glm::vec2 bottomRight(getWindowSizeX(), 0.0f);
    glm::vec2 topLeft(0.0f, getWindowSizeY());
    glm::vec2 topRight(getWindowSizeX(), getWindowSizeY());

    // Convert to world space
    glm::vec3 bottomLeftWorld = screenToWorldSpaceOriginal(bottomLeft);
    glm::vec3 bottomRightWorld = screenToWorldSpaceOriginal(bottomRight);
    glm::vec3 topLeftWorld = screenToWorldSpaceOriginal(topLeft);
    glm::vec3 topRightWorld = screenToWorldSpaceOriginal(topRight);

    // Convert to 2D vectors for easier handling
    corners.push_back(glm::vec2(bottomLeftWorld.x, bottomLeftWorld.y));
    corners.push_back(glm::vec2(topLeftWorld.x, topLeftWorld.y));
    corners.push_back(glm::vec2(topRightWorld.x, topRightWorld.y));
    corners.push_back(glm::vec2(bottomRightWorld.x, bottomRightWorld.y));

    return corners;
}

void Camera::SaveOriginal()
{
    Save.Position = Position;
    Save.Front = Front;
    Save.Up = Up;
    Save.Right = Right;
    Save.WorldUp = WorldUp;
}

void Camera::LoadOriginal()
{
    Position = Save.Position;
    Front = Save.Front;
    Right = Save.Right;
    Up = Save.Up;
    WorldUp = Save.WorldUp;
}

void Camera::SaveFrustumCorners()
{
    corners = GetFrustumCorners();
}