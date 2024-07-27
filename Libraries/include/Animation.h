#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

class Animation {
public:
    Animation();
    Animation(std::vector<unsigned int> frames, float frameDuration);
    void Update(float deltaTime);
    unsigned int GetCurrentFrame() const;
    void Start();
    void Stop();

private:
    std::vector<unsigned int> frames;
    float frameDuration; 
    float currentTime; 
    int currentFrame;  
    bool isAnimating;  
    bool flipHorizontal; 
};

#endif