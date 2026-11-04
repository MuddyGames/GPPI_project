#ifndef ANIMATION_H
#define ANIMATION_H

#include <raylib.h>

typedef struct
{
    Texture2D texture;   // Animated Sprite Sheet Texture
    Rectangle *frames;   // Array of frames (rectangles)
    int currentFrame;    // Current frame index
    int frameCount;      // Total number of frames
    float frameDuration; // Duration of each frame
    float frameTimer;    // Timer to track frame duration
    bool active;         // Is the animation active?
    bool loop;           // Should the animation loop?
} AnimationData;

// Init Animation
void InitAnimation(AnimationData *animationData, Texture2D texture, Rectangle *frames, int frameCount, float frameDuration, bool loop);

// Update Animation
void UpdateAnimation(AnimationData *animationData, float deltaTime);

// Draw Animation
void DrawAnimation(const AnimationData *animationData, Vector2 position, Color tint);

// Free Animation resources
void FreeAnimation(AnimationData *animationData);

#endif // ANIMATION_H
