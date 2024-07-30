#ifndef CUSTOM_SHAPE_H
#define CUSTOM_SHAPE_H

#include <glad/glad.h>
#include <Box2D/box2d/box2d.h>
#include <vector>
#include <Shader.h>
#include <glm/glm.hpp>
#include <json.hpp>

// Used for a custom b2Body collider.
struct CustomShape {
    std::vector<glm::vec2> vertices;

    CustomShape() {}

    void AddVertex(const glm::vec2& vertex) 
    {
        vertices.push_back(vertex);
    }

    void MoveVertex(int index, const glm::vec2& newPos) 
    {
        if (index >= 0 && index < vertices.size()) {
            vertices[index] = newPos;
        }
    }

    nlohmann::json toJson() const 
    {
        nlohmann::json j;
        for (const auto& vertex : vertices) {
            j.push_back({ vertex.x, vertex.y });
        }
        return j;
    }

    static CustomShape fromJson(const nlohmann::json& j) {
        CustomShape shape;
        for (const auto& vertex : j) {
            shape.vertices.emplace_back(vertex[0], vertex[1]);
        }
        return shape;
    }
};

#endif