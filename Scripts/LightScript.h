#pragma once

#include <Snail.h>

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
};
