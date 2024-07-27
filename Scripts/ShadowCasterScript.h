#pragma once

#include "Script.h"
#include <ScriptFactory.h>
#include <glm/glm.hpp>
#include <string>

class ShadowCasterScript : public Script
{
public:
    ShadowCasterScript();
    ~ShadowCasterScript() override;

    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& j) override; 
    std::string getTypeName() const override;

    void Start() override;
    void Update() override;
    void DrawImGui() override;

    float getIndex(std::vector<ShadowCaster>& v, ShadowCaster& K);

    std::string objectName;
    int shadowIndex = 999;
    bool addCaster;
    bool addedCaster = false;
};
