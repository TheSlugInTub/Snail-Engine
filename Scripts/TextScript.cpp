#include <TextScript.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SnailFunctions.h>
#include <Console.h>

TextScript::TextScript()
{
    editorScript = true;
}

TextScript::~TextScript() = default;

nlohmann::json TextScript::toJson() const
{
    return
    {
        {"objectName", objectName},
        {"textIndex", textIndex},
        {"addedText", addedText},
        {"text", text}
    };
}

void TextScript::fromJson(const nlohmann::json& j)
{
    objectName = j["objectName"];
    addedText = j["addedText"];
    textIndex = j["textIndex"];
    text = j["text"];
}

std::string TextScript::getTypeName() const
{
    return "TextScript";
}


void TextScript::Start() {
    if (addedText && objectName != "")
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        Canvas* canvas = ScriptFactory::Instance().GetCanvas();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        Text text_l{ text, "Resources/Fonts/comic.ttf", bodyOne->scale, glm::vec2(bodyOne->position.x, bodyOne->position.y), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) };

        canvas->AddText(text_l);

        textIndex = getIndex(canvas->texts, text_l);
    }
}

void TextScript::Update() {

    if (!addedText && objectName != "")
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        Canvas* canvas = ScriptFactory::Instance().GetCanvas();
        auto bodyOne = objectManager->FindObjectByName(objectName);
        
        Text textVar2{ text, "Resources/Fonts/comic.ttf", bodyOne->scale, glm::vec2(bodyOne->position.x, bodyOne->position.y), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) };

        canvas->AddText(textVar2);

        textIndex = getIndex(canvas->texts, textVar2);

        addedText = true;
    }

    if (textIndex != -1)
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        Canvas* canvas = ScriptFactory::Instance().GetCanvas();
        auto bodyOne = objectManager->FindObjectByName(objectName);
        glm::vec2 newPos = bodyOne->position;

        canvas->texts[textIndex].position = glm::vec2(newPos.x, newPos.y);
        canvas->texts[textIndex].scale = bodyOne->scale;
        canvas->texts[textIndex].text = text;
        canvas->texts[textIndex].color = bodyOne->color;
    }
}

float TextScript::getIndex(std::vector<Text>& v, Text& K)
{
    auto it = std::find(v.begin(), v.end(), K);
    if (it != v.end()) {
        return std::distance(v.begin(), it);
    }
    return -1;
}

void TextScript::DrawImGui() {

    char textBuffer[128];
    std::strncpy(textBuffer, text.c_str(), sizeof(textBuffer));
    if (ImGui::InputText("Text", textBuffer, sizeof(textBuffer))) {
        text = std::string(textBuffer);
    }

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
}

REGISTER_SCRIPT(TextScript)
