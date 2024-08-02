#include <ButtonScript.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SnailFunctions.h>
#include <Console.h>
#include <EventSystem.h>

ButtonScript::ButtonScript()
{
    editorScript = true;
}

ButtonScript::~ButtonScript() = default;

nlohmann::json ButtonScript::toJson() const
{
    return
    {
        {"objectName", objectName},
        {"buttonIndex", buttonIndex},
        {"addedButton", addedButton},
        {"text", text},
        {"textScale", {textScale.x, textScale.y}},
        {"buttonScale", {buttonScale.x, buttonScale.y}},
        {"backgroundColor", {backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w}},
        {"eventName", eventName}
    };
}

void ButtonScript::fromJson(const nlohmann::json& j)
{
    objectName = j["objectName"];
    addedButton = j["addedButton"];
    buttonIndex = j["buttonIndex"];
    text = j["text"];
    textScale = { j["textScale"][0], j["textScale"][1] };
    buttonScale = { j["buttonScale"][0], j["buttonScale"][1] };
    backgroundColor = { j["backgroundColor"][0], j["backgroundColor"][1], j["backgroundColor"][2], j["backgroundColor"][3] };
    eventName = j["eventName"];
}

std::string ButtonScript::getTypeName() const
{
    return "ButtonScript";
}


void ButtonScript::Start() {
    if (addedButton && objectName != "")
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        Canvas* canvas = ScriptFactory::Instance().GetCanvas();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        Button button(text, "Resources/Fonts/comic.ttf", glm::vec2(bodyOne->position.x, bodyOne->position.y), buttonScale, textScale, backgroundColor, bodyOne->color);

        canvas->AddButton(button);

        buttonIndex = getIndex(canvas->buttons, button);
    }
}

void ButtonScript::Update() {

    if (!addedButton && objectName != "")
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        Canvas* canvas = ScriptFactory::Instance().GetCanvas();
        auto bodyOne = objectManager->FindObjectByName(objectName);

        Button button(text, "Resources/Fonts/comic.ttf", glm::vec2(bodyOne->position.x, bodyOne->position.y), buttonScale, textScale, backgroundColor, bodyOne->color);

        canvas->AddButton(button);

        buttonIndex = getIndex(canvas->buttons, button);

        addedButton = true;
    }

    if (buttonIndex != -1)
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        Canvas* canvas = ScriptFactory::Instance().GetCanvas();
        auto bodyOne = objectManager->FindObjectByName(objectName);
        glm::vec2 newPos = bodyOne->position;

        canvas->buttons[buttonIndex].position = glm::vec2(newPos.x, newPos.y);
        canvas->buttons[buttonIndex].textSize = textScale;
        canvas->buttons[buttonIndex].buttonSize = buttonScale;
        canvas->buttons[buttonIndex].text = text;
        canvas->buttons[buttonIndex].backgroundColor = backgroundColor;
        canvas->buttons[buttonIndex].textColor = bodyOne->color;
        canvas->buttons[buttonIndex].eventCallback = eventName;
    }
}

float ButtonScript::getIndex(std::vector<Button>& v, Button& K)
{
    auto it = std::find(v.begin(), v.end(), K);
    if (it != v.end()) {
        return std::distance(v.begin(), it);
    }
    return -1;
}

void ButtonScript::DrawImGui() {

    char textBuffer[128];
    std::strncpy(textBuffer, text.c_str(), sizeof(textBuffer));
    if (ImGui::InputText("Text", textBuffer, sizeof(textBuffer))) {
        text = std::string(textBuffer);
    }

    char eventNameBuffer[128];
    std::strncpy(eventNameBuffer, eventName.c_str(), sizeof(eventNameBuffer));
    if (ImGui::InputText("Event", eventNameBuffer, sizeof(eventNameBuffer))) {
        eventName = std::string(eventNameBuffer);
    }

    ImGui::ColorEdit4("Button background color", glm::value_ptr(backgroundColor));
    ImGui::DragFloat2("Button scale", glm::value_ptr(buttonScale));
    ImGui::DragFloat2("Button text scale", glm::value_ptr(textScale));

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

REGISTER_SCRIPT(ButtonScript)
