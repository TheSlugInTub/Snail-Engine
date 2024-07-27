#pragma once

#include "Script.h"
#include <ScriptFactory.h>
#include <glm/glm.hpp>
#include <string>

class LightScript : public Script
{
public:
    LightScript();
    ~LightScript() override;

    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& j) override; 
    std::string getTypeName() const override;

    void Start() override;
    void Update() override;
    void DrawImGui() override;

    float getIndex(std::vector<Light>& v, Light& K);

    float innerRadius = 1.0f;
    float outerRadius = 1.0f;
    std::string objectName;
    int lightIndex = 999;
    float intensity = 1.0f;
    bool addLight;
    bool addedLight = false;
};
