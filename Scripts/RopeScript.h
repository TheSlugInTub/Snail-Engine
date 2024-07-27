#pragma once

#include "Script.h"
#include <ScriptFactory.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

class RopeScript : public Script
{
public:
    RopeScript();
    ~RopeScript() override;

    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& j) override; 
    std::string getTypeName() const override;

    void Start() override;
    void Update() override;
    void DrawImGui() override;

    void resolveCollisionWithObject(Object& object);
    void initializePoints();
    void constrainPoint(size_t index);
    void simulate(float deltaTime);
    void setBasePosition(const glm::vec2& newBasePosition);
    void findEndPoint();
    glm::vec2 findNearestPointOnEdge(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& basePos);

    std::string ObjectName = "";

    glm::vec2 basePosition = glm::vec2(0.0f, 0.0f);
    glm::vec2 offset;
    glm::vec2 endPoint = glm::vec2(0.0f, 0.0f);
    bool useEndPoint = false;
    int numPoints = 10;
    std::vector<glm::vec2> points;
    std::vector<glm::vec2> prevPoints;
    float elasticity = 1.0f;
    float pointDistance= 1.0f;
    float damping = 0.98f;
    float endPointThreshold = 5.0f;

    bool hasInitialized = false;
};
