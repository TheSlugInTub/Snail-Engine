#pragma once

#include "Script.h"
#include <ScriptFactory.h>
#include <glm/glm.hpp>
#include <SoundSource.h>
#include <SoundBuffer.h>
#include <SoundDevice.h>

class MyScript : public Script
{
public:
    MyScript();
    ~MyScript() override;

    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& j) override; 
    std::string getTypeName() const override;

    void Start() override;
    void Update() override;
    void DrawImGui() override;

    void FunctionToInvoke();

    float MovementSpeed = 1.0f;
    float JumpSpeed = 1.0f;
    std::string ObjectName = "";
    std::string collidingObjectName = "";
    bool hasAnimated = false;
    Script* script;

    ObjectManager* objmanager;

    SoundDevice* mySoundDevice;
    Sound mySound;
    std::shared_ptr<SoundSource> soundSource;

    bool sceneChanged;
};

