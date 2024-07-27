#include <RopeScript.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SnailFunctions.h>
#include <Window.h>

RopeScript::RopeScript()
    : numPoints(10), 
    pointDistance(1.0f), 
    elasticity(0.5f), 
    damping(0.98f) 
{
    points.resize(numPoints);
    prevPoints.resize(numPoints);
}

RopeScript::~RopeScript() = default;

nlohmann::json RopeScript::toJson() const
{
    nlohmann::json pointsJson = nlohmann::json::array();
    for (const auto& point : points) {
        pointsJson.push_back({ point.x, point.y });
    }

    nlohmann::json prevPointsJson = nlohmann::json::array();
    for (const auto& point : prevPoints) {
        prevPointsJson.push_back({ point.x, point.y });
    }

    return
    {
        {"ObjectName", ObjectName},
        {"BasePosition", {offset.x, offset.y}},
        {"EndPoint", {endPoint.x, endPoint.y}}, 
        {"UseEndPoint", useEndPoint}, 
        {"Elasticity", elasticity},
        {"PointDistance", pointDistance},
        {"Damping", damping},
        {"NumPoints", numPoints},
        {"Points", pointsJson},
        {"PrevPoints", prevPointsJson},
        {"ClingRange", endPointThreshold }
    };
}

void RopeScript::fromJson(const nlohmann::json& j)
{
    ObjectName = j["ObjectName"];
    offset = glm::vec2(j["BasePosition"][0], j["BasePosition"][1]);
    endPoint = glm::vec2(j["EndPoint"][0], j["EndPoint"][1]); 
    useEndPoint = j["UseEndPoint"]; 
    elasticity = j["Elasticity"];
    pointDistance = j["PointDistance"];
    damping = j["Damping"];
    numPoints = j["NumPoints"];
    endPointThreshold = j["ClingRange"];

    points.resize(numPoints);
    prevPoints.resize(numPoints);

    for (size_t i = 0; i < numPoints; ++i) {
        points[i] = glm::vec2(j["Points"][i][0], j["Points"][i][1]);
        prevPoints[i] = glm::vec2(j["PrevPoints"][i][0], j["PrevPoints"][i][1]);
    }

    hasInitialized = true; 
}

std::string RopeScript::getTypeName() const
{
    return "RopeScript";
}

void RopeScript::Start()
{

}

void RopeScript::Update() {

    if (!hasInitialized)
    {
        initializePoints();
        hasInitialized = true;
    }

    simulate(0.016f);

    ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
    Renderer* renderer = ScriptFactory::Instance().GetRenderer();

    if (ObjectName != "")
    {
        std::shared_ptr<Object> object = objectManager->FindObjectByName(ObjectName);
        object->isRope = true;

        basePosition = glm::vec2(object->position.x + offset.x, object->position.y + offset.y);
    }

    for (auto object : objectManager->objects)
    {
        if (object->bodyStatic == false && object->name != ObjectName)
        {
            //resolveCollisionWithObject(*object);
        }
    }

    float distanceToEndpoint = glm::distance(basePosition, endPoint);
    if (distanceToEndpoint > 2.0f) {
        findEndPoint();
    }

    glm::mat4 view = objectManager->globalCamera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(objectManager->globalCamera->GetZoom()), getAspectRatio(), 0.1f, 100.0f);

    renderer->RenderRope(points, view, projection);
}

void RopeScript::resolveCollisionWithObject(Object& object) {
    if (object.body != NULL) {
        for (int iter = 0; iter < 5; ++iter) { // Multiple iterations for better stability
            for (auto& point : points) {
                b2Vec2 b2Point(point.x, point.y);

                for (b2Fixture* fixture = object.body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
                    if (fixture->TestPoint(b2Point)) {
                        b2Vec2 normal = b2Point - object.body->GetWorldCenter();
                        normal.Normalize();

                        // Correction to resolve the collision
                        b2Vec2 correction = normal;
                        correction *= pointDistance * 0.1f; 

                        glm::vec2 glmCorrection(correction.x, correction.y);
                        point += glmCorrection;

                        // Re-constrain points to maintain the rope's integrity
                        for (size_t i = 1; i < points.size(); ++i) {
                            constrainPoint(i);
                        }
                    }
                }
            }
        }
    }
}

void RopeScript::initializePoints() {
    for (size_t i = 0; i < points.size(); ++i) {
        points[i] = basePosition + glm::vec2(0, -static_cast<float>(i) * pointDistance);
        prevPoints[i] = points[i];
    }
}

void RopeScript::constrainPoint(size_t index) {
    glm::vec2& point = points[index];
    glm::vec2& prevPoint = points[index - 1];

    glm::vec2 direction = point - prevPoint;
    float distance = glm::length(direction);

    if (distance > pointDistance) {
        glm::vec2 correction = direction * ((distance - pointDistance) / distance);
        point -= correction * 0.5f;
        prevPoint += correction * 0.5f;
    }
}

void RopeScript::simulate(float deltaTime) {
    // Update the position of the base point
    points[0] = basePosition;

    for (size_t i = 1; i < points.size(); ++i) {
        glm::vec2& point = points[i];
        glm::vec2& prevPoint = prevPoints[i];

        // Verlet integration
        glm::vec2 temp = point;
        glm::vec2 gravity(0, -9.81f * (1.0f - (elasticity * static_cast<float>(i) / points.size())));
        point += (point - prevPoint) * damping + gravity * deltaTime * deltaTime;
        prevPoint = temp;

        // Constrain point to be within the rope's length from the previous point
        constrainPoint(i);
    }

    // Constrain the last point to the endpoint if useEndPoint is true
    if (useEndPoint) {
        points.back() = endPoint;
        prevPoints.back() = endPoint;
    }
}

void RopeScript::setBasePosition(const glm::vec2& newBasePosition) {
    basePosition = newBasePosition;
    points[0] = basePosition;
    prevPoints[0] = basePosition;
}

void RopeScript::findEndPoint() {
    ObjectManager* objectManager = ScriptFactory::Instance().GetManager();

    float minDistance = std::numeric_limits<float>::max();
    glm::vec2 nearestPoint;

    for (auto& object : objectManager->objects) {
        if (object->body != NULL && object->name != ObjectName) {
            b2Body* body = object->body;
            for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
                if (fixture->GetType() == b2Shape::e_polygon) {
                    b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
                    for (int i = 0; i < polygonShape->m_count; ++i) {
                        b2Vec2 vertex = body->GetWorldPoint(polygonShape->m_vertices[i]);
                        glm::vec2 glmVertex(vertex.x, vertex.y);

                        // Check if vertex is within the endPointThreshold radius
                        if (glm::distance(basePosition, glmVertex) <= endPointThreshold) {
                            // Find the nearest point on the edge of the object
                            b2Vec2 nextVertex = body->GetWorldPoint(polygonShape->m_vertices[(i + 1) % polygonShape->m_count]);
                            glm::vec2 glmNextVertex(nextVertex.x, nextVertex.y);

                            glm::vec2 nearestEdgePoint = findNearestPointOnEdge(glmVertex, glmNextVertex, basePosition);
                            float distance = glm::distance(basePosition, nearestEdgePoint);

                            if (distance < minDistance) {
                                minDistance = distance;
                                nearestPoint = nearestEdgePoint;
                            }
                        }
                    }
                }
            }
        }
    }

    if (minDistance != std::numeric_limits<float>::max()) {
        endPoint = nearestPoint;
    }
}

glm::vec2 RopeScript::findNearestPointOnEdge(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& basePos) {
    glm::vec2 edge = pointB - pointA;
    float edgeLengthSquared = glm::dot(edge, edge);
    float t = glm::dot(basePos - pointA, edge) / edgeLengthSquared;
    t = glm::clamp(t, 0.0f, 1.0f); // Ensure t is within the range [0, 1]
    return pointA + t * edge;
}

void RopeScript::DrawImGui() {

    ImGui::DragFloat2("Rope Position", glm::value_ptr(offset));
    ImGui::DragFloat("Elasticity", &elasticity);
    ImGui::DragFloat("Point Distance", &pointDistance);
    ImGui::DragFloat("Damping", &damping);
    ImGui::DragFloat("Cling Range", &endPointThreshold);
    bool changing = ImGui::DragInt("Number of Points", &numPoints);
    if (changing)
    {
        points.resize(numPoints);
        prevPoints.resize(numPoints);
    }

    ImGui::Checkbox("Use End Point", &useEndPoint);
    if (useEndPoint) {
        ImGui::DragFloat2("End Point", glm::value_ptr(endPoint));
    }

    ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
    std::vector<std::string> objectNames = objectManager->FindAllObjectNames(); 

    if (ImGui::BeginCombo("Object", ObjectName.c_str()))
    {
        for (const auto& name : objectNames)
        {
            bool is_selected = (ObjectName == name);
            if (ImGui::Selectable(name.c_str(), is_selected))
            {
                ObjectName = name;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

REGISTER_SCRIPT(RopeScript)
