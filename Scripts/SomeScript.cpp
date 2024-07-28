#include <SomeScript.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SnailFunctions.h>
#include <Console.h>
#include <ContactListener.h>

MyScript::MyScript() = default;
MyScript::~MyScript() = default;

nlohmann::json MyScript::toJson() const 
{
    return
    {
        {"MovementSpeed", MovementSpeed},
        {"JumpSpeed", JumpSpeed},
        {"ObjectName", ObjectName}
    };
}

void MyScript::fromJson(const nlohmann::json& j)
{
    MovementSpeed = j["MovementSpeed"];
    JumpSpeed = j["JumpSpeed"];
    ObjectName = j["ObjectName"];
}

std::string MyScript::getTypeName() const
{
    return "MyScript";
}

void MyScript::Start() {
    
    mySoundDevice = SoundDevice::get();
    mySound = SoundBuffer::get()->addSoundEffect("Resources/Sounds/Flicky.wav");

    soundSource = std::make_shared<SoundSource>();
}

void MyScript::Update() {

    if (!ObjectName.empty())
    {
        ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
        auto bodyOne = objectManager->FindObjectByName(ObjectName);

        objectManager->globalCamera->updateShake(0.016f);

        if (bodyOne)
        {
            if (!hasAnimated)
            {
                std::vector<unsigned int> walkFrames = { loadTexture("Resources/Textures/walk/1.png"), loadTexture("Resources/Textures/walk/2.png"), loadTexture("Resources/Textures/walk/3.png"), loadTexture("Resources/Textures/walk/4.png"), loadTexture("Resources/Textures/walk/5.png"), loadTexture("Resources/Textures/walk/6.png"), loadTexture("Resources/Textures/walk/7.png"), loadTexture("Resources/Textures/walk/8.png"), loadTexture("Resources/Textures/walk/9.png"), loadTexture("Resources/Textures/walk/10.png"), loadTexture("Resources/Textures/walk/11.png"), loadTexture("Resources/Textures/walk/12.png"), loadTexture("Resources/Textures/walk/13.png") };
                Animation walkAnimation(walkFrames, 0.047f);

                bodyOne->AddAnimation("walk", walkAnimation);
                bodyOne->SetCurrentAnimation("walk");
                hasAnimated = true;
            }

            if (GetKey(Key::Left))
            {
                bodyOne->isFlipped = true;
                bodyOne->Animate();
                b2Vec2 pos = bodyOne->body->GetPosition();
                pos.x += -0.004 * MovementSpeed;
                bodyOne->body->SetTransform(pos, bodyOne->body->GetAngle());
                b2Vec2 force(-0.00001f, 0.0f);
                bodyOne->body->ApplyForceToCenter(force, true);
            }
            else if (GetKey(Key::Right))
            {
                bodyOne->isFlipped = false;
                bodyOne->Animate();
                b2Vec2 pos = bodyOne->body->GetPosition();
                pos.x += 0.004 * MovementSpeed;
                bodyOne->body->SetTransform(pos, bodyOne->body->GetAngle());
                b2Vec2 force(0.00001f, 0.0f);
                bodyOne->body->ApplyForceToCenter(force, true);
            }
            else
            {
                bodyOne->StopAnimating();
            }

            if (GetKeyDown(Key::Up))
            {
                Console::Log("You just jumped!");

                objectManager->globalCamera->Shake(0.2f, 0.15f);
                b2Vec2 impulse(0.0f, 0.2f * JumpSpeed);
                bodyOne->body->ApplyLinearImpulse(impulse, bodyOne->body->GetWorldCenter(), true);

                soundSource->Play(mySound);
            }

            if (GetMouseButton(MouseKey::LeftClick))
            {
                double mouseX = GetMouseInputHorizontal();
                double mouseY = GetMouseInputVertical();

                glm::vec3 coords = objectManager->globalCamera->screenToWorldSpace(glm::vec2(mouseX, mouseY));
                if (IsPointInsideObject(glm::vec2(coords.x, coords.y), *bodyOne))
                {
                    bodyOne->body->SetTransform(b2Vec2(coords.x, coords.y), bodyOne->body->GetAngle());
                }

                auto jbo = objectManager->FindObjectByName(collidingObjectName);
                objectManager->RemoveObject(jbo);
            }

            if (GetMouseButtonDown(MouseKey::RightClick) && !sceneChanged)
            {
                bodyOne->StopAnimating();
                bodyOne->currentAnimation = nullptr;
                objectManager->LoadScene("yru.json");

                sceneChanged = true;
            }
        }
    }
}

void MyScript::DrawImGui() {    
    ImGui::DragFloat("Movement speed", &MovementSpeed);
    ImGui::DragFloat("Jump speed", &JumpSpeed);

    ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
    std::vector<std::string> objectNames = objectManager->FindAllObjectNames(); 

    if (ImGui::BeginCombo("Object", ObjectName.c_str()))
    {
        for (const auto& name : objectNames)
        {
            bool is_selected = (ObjectName == name);
            if (ImGui::Selectable(name.c_str(), is_selected))
            {
                ObjectName = name;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Collider Object", collidingObjectName.c_str()))
    {
        for (const auto& name : objectNames)
        {
            bool is_selected = (collidingObjectName == name);
            if (ImGui::Selectable(name.c_str(), is_selected))
            {
                collidingObjectName = name;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

REGISTER_SCRIPT(MyScript)
