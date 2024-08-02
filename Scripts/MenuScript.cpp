#include <MenuScript.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SnailFunctions.h>
#include <Console.h>
#include <EventSystem.h>

MenuScript::MenuScript()
{

}

MenuScript::~MenuScript() = default;

nlohmann::json MenuScript::toJson() const
{
    nlohmann::json j;
    return j;
}

void MenuScript::fromJson(const nlohmann::json& j)
{

}

std::string MenuScript::getTypeName() const
{
    return "MenuScript";
}

void PlayEvent()
{
    ScriptFactory::Instance().GetManager()->LoadScene("Game.json");
}

void SettingsEvent()
{
    ScriptFactory::Instance().GetManager()->LoadScene("Settings.json");
}

void QuitEvent()
{
    Console::Log("quit");
}

void BackEvent()
{
    ScriptFactory::Instance().GetManager()->LoadScene("MainScene.json");
}

void MenuScript::Start() {
    ScriptFactory::Instance().GetEventSystem()->RegisterCallback("PlayEvent", PlayEvent);
    ScriptFactory::Instance().GetEventSystem()->RegisterCallback("OptionsEvent", SettingsEvent);
    ScriptFactory::Instance().GetEventSystem()->RegisterCallback("QuitEvent", QuitEvent);
    ScriptFactory::Instance().GetEventSystem()->RegisterCallback("BackEvent", BackEvent);
}

void MenuScript::Update() {

}

void MenuScript::DrawImGui() {


}

REGISTER_SCRIPT(MenuScript)
