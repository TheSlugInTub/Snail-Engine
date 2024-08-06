#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <glm/glm.hpp>
#include <string>

struct AnimationFrame
{
    glm::vec3 position;
    glm::vec2 scale;
    float rotation;
    unsigned int texture;
    float timeUntilNextFrame;

    bool operator==(const AnimationFrame& other) const
    {
        return position == other.position &&
            scale == other.scale &&
            rotation == other.rotation &&
            texture == other.texture &&
            timeUntilNextFrame == other.timeUntilNextFrame;
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

    static Animation* FindByName(const std::vector<Animation>& animations, const std::string& name);

    std::vector<AnimationFrame> frames;
    std::string name;
    float frameDuration;
    float currentTime;
    int currentFrame;
    bool isAnimating;
    AnimationFrame currentFrameData;
private:

};

#endif