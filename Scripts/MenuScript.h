#ifndef BUTTON_SCRIPT
#define BUTTON_SCRIPT

#include "Script.h"
#include <ScriptFactory.h>
#include <glm/glm.hpp>
#include <string>

class MenuScript : public Script
{
public:
    MenuScript();
    ~MenuScript() override;

    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& j) override;
    std::string getTypeName() const override;

    void Start() override;
    void Update() override;
    void DrawImGui() override;
};

#endif