#ifndef BUTTON_SCRIPT
#define BUTTON_SCRIPT

#include "Script.h"
#include <ScriptFactory.h>
#include <glm/glm.hpp>
#include <string>

class ButtonScript : public Script
{
public:
    ButtonScript();
    ~ButtonScript() override;

    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& j) override;
    std::string getTypeName() const override;

    void Start() override;
    void Update() override;
    void DrawImGui() override;

    float getIndex(std::vector<Button>& v, Button& K);

    std::string objectName = "";
    std::string text = "Text";
    std::string fontPath = "";
    std::string eventName = "";
    glm::vec2 textScale = glm::vec2(1.0f);
    glm::vec2 buttonScale = glm::vec2(1.0f);
    glm::vec4 backgroundColor = glm::vec4(1.0f);
    glm::vec4 hoverColor = glm::vec4(1.0f);
    int buttonIndex = -1;
    bool addedButton = false;
};

#endif