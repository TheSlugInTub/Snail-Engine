#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <json.hpp>
#include <LoadTexture.h>

struct AnimationFrame
{
    glm::vec3 position;
    glm::vec2 scale;
    float rotation;
    unsigned int texture;
    float timeUntilNextFrame;
    std::string texturePath = "";

    bool operator==(const AnimationFrame& other) const
    {
        return position == other.position &&
            scale == other.scale &&
            rotation == other.rotation &&
            texture == other.texture &&
            timeUntilNextFrame == other.timeUntilNextFrame;
    }

    nlohmann::json toJson() const
    {
        nlohmann::json j = {
            {"FramePosition", {position.x, position.y, position.z}},
            {"FrameScale", {scale.x, scale.y}},
            {"FrameRotation", rotation},
            {"FrameTime", timeUntilNextFrame},
            {"FrameTexture", texturePath}
        };

        return j;
    }

    static AnimationFrame fromJson(const nlohmann::json& j)
    {
        glm::vec3 position = { j["FramePosition"][0], j["FramePosition"][1], j["FramePosition"][2] };
        glm::vec2 scale = { j["FrameScale"][0], j["FrameScale"][1] };
        float rotation = j["FrameRotation"];
        float timeUntilNextFrame = j["FrameTime"];
        std::string texturePath = j["FrameTexture"];
        unsigned int texture = TextureLoader::textureLoad(texturePath.c_str());

        AnimationFrame frame(position, scale, rotation, texture, timeUntilNextFrame, texturePath);
        return frame;
    }
};

class Animation {
public:
    Animation();
    Animation(std::vector<AnimationFrame> frames, float frameDuration);
    void Update(float deltaTime);
    AnimationFrame GetCurrentFrame() const;
    void Start();
    void Stop();

    bool operator==(const Animation& other) const
    {
        return frames == other.frames &&
            name == other.name &&
            frameDuration == other.frameDuration &&
            currentTime == other.currentTime &&
            currentFrame == other.currentFrame &&
            isAnimating == other.isAnimating &&
            currentFrameData == other.currentFrameData;
    }

    // Returns a pointer to the animation that is found by its name in a vector of animations.
    static Animation* FindByName(const std::vector<Animation>& animations, const std::string& name);

    // Serialization functions.
    nlohmann::json toJson() const;
    static Animation fromJson(const nlohmann::json& j);

    std::vector<AnimationFrame> frames;
    std::string name;
    float frameDuration;
    float currentTime;
    int currentFrame;
    bool isAnimating;

    // Current frame.
    AnimationFrame currentFrameData;
private:

};

#endif