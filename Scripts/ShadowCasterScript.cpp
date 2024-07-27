#include <ShadowCasterScript.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SnailFunctions.h>

ShadowCasterScript::ShadowCasterScript() = default;
ShadowCasterScript::~ShadowCasterScript() = default;

nlohmann::json ShadowCasterScript::toJson() const
{
    return
    {
        {"objectName", objectName},
        {"addedCaster", addedCaster},
        {"shadowIndex", shadowIndex}
    };
}

void ShadowCasterScript::fromJson(const nlohmann::json& j)
{
    objectName = j["objectName"];
    shadowIndex = j["shadowIndex"];
    addedCaster = j["addedCaster"];
}

std::string ShadowCasterScript::getTypeName() const
{
    return "ShadowCasterScript";
}


void ShadowCasterScript::Start() {
    if (addedCaster)
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        ShadowCaster caster(glm::vec3(bodyOne->position.x, bodyOne->position.y, 0.0f), bodyOne->scale, 0.0f, bodyOne->texture, bodyOne->isFlipped);
        objectManager->casters.emplace_back(caster);

        shadowIndex = getIndex(objectManager->casters, caster);
    }
}

void ShadowCasterScript::Update() {

    if (objectName != "" && addCaster)
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        if (!addedCaster)
        {
            ShadowCaster caster(glm::vec3(bodyOne->position.x, bodyOne->position.y, 0.0f), bodyOne->scale, 0.0f, bodyOne->texture, bodyOne->isFlipped);
            objectManager->casters.emplace_back(caster);

            addedCaster = true;

            shadowIndex = getIndex(objectManager->casters, caster);
        }
    }

    if (shadowIndex != 999)
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        objectManager->casters[shadowIndex].position = bodyOne->position;
        objectManager->casters[shadowIndex].scale = bodyOne->scale;
        objectManager->casters[shadowIndex].texture = bodyOne->texture;
        objectManager->casters[shadowIndex].rotation = bodyOne->rotation;
    }
}

float ShadowCasterScript::getIndex(std::vector<ShadowCaster>& v, ShadowCaster& K)
{
    auto it = std::find(v.begin(), v.end(), K);
    if (it != v.end()) {
        return std::distance(v.begin(), it);
    }
    return -1;
}

void ShadowCasterScript::DrawImGui() {

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

    addCaster = ImGui::Button("Add caster");
}

REGISTER_SCRIPT(ShadowCasterScript)
