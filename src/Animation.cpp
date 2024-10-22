#include "Animation.h"
#include <iostream>

Animation::Animation()
    : frameDuration(1.0f), currentTime(0.0f), currentFrame(0), isAnimating(false) {}

Animation::Animation(std::vector<AnimationFrame> frames, float frameDuration)
    : frames(std::move(frames)), frameDuration(frameDuration), currentTime(0.0f), currentFrame(0), isAnimating(false) {}


void Animation::Update(float deltaTime)
{
    if (!isAnimating || frames.empty()) return;

    currentTime += deltaTime;

    // Calculate the total duration of the animation
    float totalDuration = 0.0f;
    for (const auto& frame : frames) {
        totalDuration += frame.timeUntilNextFrame;
    }

    // Loop the animation
    if (currentTime >= totalDuration) {
        currentTime = 0.0f; // Reset to start
    }

    // Find the current frame based on elapsed time
    float accumulatedDuration = 0.0f;
    for (size_t i = 0; i < frames.size(); ++i) {
        accumulatedDuration += frames[i].timeUntilNextFrame;
        if (currentTime <= accumulatedDuration) {
            currentFrame = i;
            break;
        }
    }

    // Calculate the next frame index and wrap around if needed
    int nextFrame = (currentFrame + 1) % frames.size();
    AnimationFrame frameA = frames[currentFrame];
    AnimationFrame frameB = frames[nextFrame];

    // Calculate the interpolation factor
    float frameTime = currentTime;
    for (int i = 0; i < currentFrame; ++i) {
        frameTime -= frames[i].timeUntilNextFrame;
    }

    float t = frameTime / frames[currentFrame].timeUntilNextFrame;

    // Interpolate between the current frame and the next frame
    currentFrameData.position = glm::mix(frameA.position, frameB.position, t);
    currentFrameData.scale = glm::mix(frameA.scale, frameB.scale, t);
    currentFrameData.rotation = glm::mix(frameA.rotation, frameB.rotation, t);
    currentFrameData.texture = frames[currentFrame].texture;
}

AnimationFrame Animation::GetCurrentFrame() const 
{
    return currentFrameData;
}

void Animation::Start() {
    isAnimating = true;
}

void Animation::Stop() {
    isAnimating = false;
}

Animation* Animation::FindByName(const std::vector<Animation>& animations, const std::string& name) {
    for (auto& animation : animations) {
        if (animation.name == name) {
            return const_cast<Animation*>(&animation);
        }
    }
    return nullptr; 
}

nlohmann::json Animation::toJson() const
{
    nlohmann::json j = {
        {"Name", name},
        {"FrameDuration", frameDuration},
        {"CurrentTime", currentTime},
        {"CurrentFrame", currentFrame},
        {"IsAnimating", isAnimating}
    };

    // Serialize frames
    std::vector<nlohmann::json> framesJson;
    for (const auto& frame : frames) {
        framesJson.push_back(frame.toJson());
    }
    j["Frames"] = framesJson;

    return j;
}

Animation Animation::fromJson(const nlohmann::json& j)
{
    Animation animation;
    
    animation.name = j["Name"];
    animation.frameDuration = j["FrameDuration"];
    animation.currentTime = j["CurrentTime"];
    animation.currentFrame = j["CurrentFrame"];
    animation.isAnimating = j["IsAnimating"];

    // Deserialize frames
    if (j.contains("Frames")) {
        for (const auto& frameJson : j["Frames"]) {
            animation.frames.push_back(AnimationFrame::fromJson(frameJson));
        }
    }

    return animation;
}