#include <Object.h>
#include <SnailFunctions.h>
#include <iostream>
#include <ScriptFactory.h>

Object::Object(b2Body* objectBody, unsigned int objectTexture, glm::vec2 objectScale, b2BodyType objectBodyType, float objectRotation, glm::vec3 objectPosition, glm::vec4 objectColor)
{
    texture = objectTexture;
    position = objectPosition;
    scale = objectScale;
    rotation = objectRotation;
    color = objectColor;
    bodyScale = glm::vec2(0.5f);
    bodyType = objectBodyType;
    body = objectBody;
    body->SetTransform(b2Vec2(position.x, position.y), rotation);
    name = "Object";
    isRope = false;
    isFlipped = false;
}

void Object::UpdateTransform()
{
    if (body != nullptr && bodyStatic == false)
    {
        position = glm::vec3(body->GetPosition().x, body->GetPosition().y, position.z);
        rotation = body->GetAngle();
    }

    for (auto& script : scripts) {
        if (!script->startFrameEnded && isPlaying) {
            script->Start();
            script->startFrameEnded = true;
        }
        else if (!script->startFrameEnded && script->editorScript)
        {
            script->Start();
            script->startFrameEnded = true;
        }
    }

    for (auto& script : scripts) {
        if (!isRope && isPlaying)
        {
            script->Update();
        }
        else if (!isRope && script->editorScript)
        {
            script->Update();
        }
    }
}

nlohmann::json Object::toJson() const {
    nlohmann::json j = {
        {"position", {position.x, position.y, position.z}},
        {"scale", {scale.x, scale.y}},
        {"bodyScale", {bodyScale.x, bodyScale.y}},
        {"rotation", rotation},
        {"color", {color.x, color.y, color.z, color.w}},
        {"texture", texturePath},
        {"bodyType", bodyType},
        {"scripts", nlohmann::json::array()},
        {"name", name},
        {"shape", shape.toJson()},
        {"bodyStatic", bodyStatic}
    };

    // Serialize animations
    for (const auto& anim : animations) {
        auto animationJson = anim.toJson();
        j["animations"].push_back(animationJson);
    }

    if (j.contains("animations") && currentAnimation != nullptr)
    {
        j["currentAnimationName"] = currentAnimation->name;
    }

    // Serialize scripts
    for (const auto& script : scripts) {
        nlohmann::json scriptJson = script->toJson();
        scriptJson["type"] = script->getTypeName();
        j["scripts"].push_back(scriptJson);
    }

    return j;
}

std::shared_ptr<Object> Object::fromJson(const nlohmann::json& j, b2World& world) {
    glm::vec3 position = { j["position"][0], j["position"][1], j["position"][2] };
    glm::vec2 scale = { j["scale"][0], j["scale"][1] };
    glm::vec2 bodyScale = { j["bodyScale"][0], j["bodyScale"][1] };
    float rotation = j["rotation"];
    glm::vec4 color = { j["color"][0], j["color"][1], j["color"][2], j["color"][3] };
    std::string texturePath = j["texture"];
    b2BodyType bodyType = static_cast<b2BodyType>(j["bodyType"]);
    std::string objectName = j["name"];
    bool bodyStatic = j["bodyStatic"];

    // Create the Box2D body
    b2BodyDef bodyDef;
    bodyDef.type = bodyType;
    bodyDef.position.Set(position.x, position.y);
    b2Body* body = world.CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(bodyScale.x, bodyScale.y);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    if (bodyType == b2_staticBody) {
        fixtureDef.density = 0.0f;
    }
    else {
        fixtureDef.density = 1.0f;
    }
    fixtureDef.friction = 0.3f;

    if (bodyStatic)
    {
        fixtureDef.filter.categoryBits = 0x0000;
        fixtureDef.filter.maskBits = 0x0000; // Collide with nothing
    }

    body->CreateFixture(&fixtureDef);

    unsigned int texture;

    if (texturePath == "")
    {
        texture = loadTexture("Resources/Textures/DefaultTexture.png");
    }
    else
    {
        texture = loadTexture(texturePath.c_str());
    }

    auto obj = std::make_shared<Object>(body, texture, scale, bodyType, rotation, position, color);
    obj->bodyScale = bodyScale;
    obj->name = objectName;
    obj->texturePath = texturePath;
    obj->shape = CustomShape::fromJson(j["shape"]);
    obj->body->SetTransform(b2Vec2(position.x, position.y), body->GetAngle());
    obj->bodyStatic = bodyStatic;
    
    if (j.contains("animations"))
    {
        for (const auto& animJson : j["animations"]) {
            Animation anim = Animation::fromJson(animJson);
            obj->animations.push_back(anim);
        }

        obj->currentAnimation = Animation::FindByName(obj->animations, j["currentAnimationName"]);
    }

    for (const auto& scriptJson : j["scripts"]) {
        std::string typeName = scriptJson["type"];
        auto script = ScriptFactory::Instance().CreateScript(typeName);
        script->fromJson(scriptJson);
        obj->AddScript(std::move(script));
    }

    return obj;
}

void Object::AddScript(std::unique_ptr<Script> script)
{
    scripts.push_back(std::move(script));
}

void Object::RemoveScript(const std::string& scriptName) {
    auto it = std::remove_if(scripts.begin(), scripts.end(),
        [&scriptName](const std::unique_ptr<Script>& script) {
            return script->getTypeName() == scriptName;
        });
    scripts.erase(it, scripts.end());
}

void Object::CreateCustomCollider(b2Body* body) {

    b2Fixture* fixture = body->GetFixtureList();
    while (fixture != nullptr) {
        b2Fixture* next = fixture->GetNext();
        body->DestroyFixture(fixture);
        fixture = next;
    }

    b2PolygonShape polygonShape;
    std::vector<b2Vec2> vertices(shape.vertices.size());

    for (int i = 0; i < shape.vertices.size(); ++i) {
        vertices[i].Set(shape.vertices[i].x, shape.vertices[i].y);
    }

    polygonShape.Set(vertices.data(), vertices.size());

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &polygonShape;

    if (bodyType == b2_staticBody) {
        fixtureDef.density = 0.0f;
        fixtureDef.friction = 0.3f;
    }
    else if (bodyType == b2_dynamicBody) {
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;
    }

    body->CreateFixture(&fixtureDef);
}

void Object::AddChild(std::shared_ptr<Object> child)
{
    children.push_back(child);
    child->parent = shared_from_this();
}

void Object::RemoveChild(std::shared_ptr<Object> child)
{
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
    child->parent = nullptr;
}

void Object::AddAnimation(Animation animation) {
    animations.push_back(animation);
}

void Object::Animate() {
    if (currentAnimation) {
        currentAnimation->Start();
    }
}

void Object::StopAnimating() {
    if (currentAnimation) {
        currentAnimation->Stop();
    }
}

void Object::SetCurrentAnimation(std::string& animationName) {
    currentAnimation = Animation::FindByName(animations, animationName);
}