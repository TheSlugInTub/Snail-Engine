#ifndef TEXT_SCRIPT
#define TEXT_SCRIPT

#include "Script.h"
#include <ScriptFactory.h>
#include <glm/glm.hpp>
#include <string>

class TextScript : public Script
{
public:
    TextScript();
    ~TextScript() override;

    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& j) override;
    std::string getTypeName() const override;

    void Start() override;
    void Update() override;
    void DrawImGui() override;

    float getIndex(std::vector<Text>& v, Text& K);

    std::string objectName;
    std::string text;
    int textIndex = -1;
    bool addedText = false;
};

#endif