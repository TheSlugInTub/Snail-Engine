#include <LightScript.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SnailFunctions.h>
#include <Console.h>

LightScript::LightScript() = default;
LightScript::~LightScript() = default;

nlohmann::json LightScript::toJson() const
{
    return
    {
        {"objectName", objectName},
        {"innerRadius", innerRadius},
        {"outerRadius", outerRadius},
        {"intensity", intensity},
        {"lightIndex", lightIndex},
        {"addedLight", addedLight}
    };
}

void LightScript::fromJson(const nlohmann::json& j)
{
    objectName = j["objectName"];
    innerRadius = j["innerRadius"];
    outerRadius = j["outerRadius"];
    intensity = j["intensity"];
    addedLight = j["addedLight"];
    lightIndex = j["lightIndex"];
}

std::string LightScript::getTypeName() const
{
    return "LightScript";
}


void LightScript::Start() {
    if (addedLight)
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        Light light(bodyOne->position, innerRadius, outerRadius, bodyOne->color, intensity);
        objectManager->lights.emplace_back(light);

        lightIndex = getIndex(objectManager->lights, light);
    }
}

void LightScript::Update() {

    if (objectName != "" && addLight)
    {
        Console::Log("Light added.");

        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        if (!addedLight)
        {
            Light light(bodyOne->position, innerRadius, outerRadius, bodyOne->color, intensity);
            objectManager->lights.emplace_back(light);

            addedLight = true;

            lightIndex = getIndex(objectManager->lights, light);
        }
    }

    if (lightIndex != 999)
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        objectManager->lights[lightIndex].color = bodyOne->color;
        objectManager->lights[lightIndex].position = bodyOne->position;
        objectManager->lights[lightIndex].innerRadius = innerRadius;
        objectManager->lights[lightIndex].outerRadius = outerRadius;
        objectManager->lights[lightIndex].intensity = intensity;
    }
}

float LightScript::getIndex(std::vector<Light>& v, Light& K)
{
    auto it = std::find(v.begin(), v.end(), K);
    if (it != v.end()) {
        return std::distance(v.begin(), it);
    }
    return -1; 
}

void LightScript::DrawImGui() {
    ImGui::DragFloat("Inner radius", &innerRadius);
    ImGui::DragFloat("Outer radius", &outerRadius);
    ImGui::DragFloat("Intensity", &intensity);

    ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
    std::vector<std::string> objectNames = objectManager->FindAllObjectNames(); 

    if (ImGui::BeginCombo("Object", objectName.c_str()))
    {
        for (const auto& name : objectNames)
        {
            bool is_selected = (objectName == name);
            if (ImGui::Selectable(name.c_str(), is_selected))
            {
                objectName = name;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    addLight = ImGui::Button("Add light");
}

REGISTER_SCRIPT(LightScript)
