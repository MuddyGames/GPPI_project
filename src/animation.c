#include <stdlib.h>
#include "animation/animation.h"

/**
 * InitAnimation : Sets up an animation and gets it ready for the road.
 *
 * Copies in the frames, sets the timing, and starts things off on the
 * first frame so the animation doesn't come out looking pure confused.
 *
 * @animationData: Where all the animation info lives.
 * @texture:       The sprite sheet the frames come from.
 * @frames:        Array of rectangles for each frame.
 * @frameCount:    Total number of frames.
 * @frameDuration: Time each frame stays on screen.
 * @loop:          True if the animation should start over when it ends.
 */

void InitAnimation(AnimationData *animationData,
                   Texture2D texture,
                   Rectangle *frames,
                   int frameCount,
                   float frameDuration,
                   bool loop)
{
    // Store texture and allocate memory for frame data in animationData
    animationData->texture = texture;
    animationData->frames = (Rectangle *)malloc(frameCount * sizeof(Rectangle));

    // Copy each frame from frames array to animationData's frames
    if (animationData->frames != NULL)
    {
        for (int i = 0; i < frameCount; i++)
        {
            animationData->frames[i] = frames[i];
        }
    }

    // Initialise animation properties
    animationData->frameCount = frameCount;
    animationData->frameDuration = frameDuration;
    animationData->loop = loop;
    animationData->currentFrame = 0;  // Start at the first frame
    animationData->frameTimer = 0.0f; // Reset frame timer to zero
    animationData->active = true;     // Set animation as active by default
    animationData->finished = false;  // Not finished, it has only started
}

/**
 * UpdateAnimation : Advances the animation based on time passed.
 *
 * Adds on the delta time, and when enough has built up, moves to the
 * next frame. If we hit the end, we loop back or hold the last frame,
 * depending on the mood you set with the loop flag.
 *
 * An animation that doesn't loop sets finished once its last frame has
 * had its full turn on screen, so a state can tell when a swipe is done.
 *
 * @animationData: The animation we're updating.
 * @deltaTime:     Time since the last update call.
 */

void UpdateAnimation(AnimationData *animationData, float deltaTime)
{
    // If the animation is inactive, return immediately
    if (!animationData->active)
    {
        return;
    }

    // Update frame timer with delta time (time elapsed since last frame)
    animationData->frameTimer += deltaTime;

    // Check if it's time to advance to the next frame
    if (animationData->frameTimer >= animationData->frameDuration)
    {
        animationData->currentFrame++; // Move to the next frame

        // Check if the end of the animation frames is reached
        if (animationData->currentFrame >= animationData->frameCount)
        {
            if (animationData->loop)
            {
                animationData->currentFrame = 0; // Restart at first frame if looping
            }
            else
            {
                animationData->currentFrame = animationData->frameCount - 1; // Stay on last frame if not looping
                animationData->finished = true;                               // and say so
            }
        }

        // Reset the frame timer after advancing frames
        animationData->frameTimer = 0.0f;
    }
}

/**
 * DrawAnimation : Draws the animation's current frame on screen.
 *
 * Grabs the right frame, centres it nicely so it's not hanging off
 * the side like a poorly parked car, and draws it with whatever tint
 * you fancy (tinted windows are a curse during the NCT inspection).
 *
 * @animationData: Animation info and current frame.
 * @position:      Where to draw it.
 * @tint:          Colour tint. (Black for shadows)
 */

void DrawAnimation(const AnimationData *animationData, Vector2 position, Color tint)
{
    // If the animation is inactive, don't render it
    if (!animationData->active)
    {
        return;
    }

    // Retrieve the rectangle for the current frame
    Rectangle frame = animationData->frames[animationData->currentFrame];

    // Adjust the drawing position so the animation is centered at the specified point
    Vector2 adjustedPosition = {
        position.x - frame.width / 2, // Offset X by half the frame width
        position.y - frame.height / 2 // Offset Y by half the frame height
    };

    // Render the current frame with the given tint color
    DrawTextureRec(
        animationData->texture,
        frame,
        adjustedPosition,
        tint);
}

/**
 * FreeAnimation : Frees resources allocated for the animation.
 * Not very animated, but totally necessary.
 *
 * Cleans up any memory used for storing frames to prevent memory leaks.
 * Faster than a Garda chase, and way less paperwork too.
 *
 * @animationData: Animation resources to be freed, be free my friend.
 */
void FreeAnimation(AnimationData *animationData)
{
    if (!animationData)
        return;

    free(animationData->frames);
    animationData->frames = NULL;

    animationData->frameCount = 0;
    animationData->currentFrame = 0;
    animationData->frameTimer = 0.0f;
    animationData->active = false;
    animationData->finished = false;
}
