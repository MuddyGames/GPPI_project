#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <raylib.h>

// Paint a new random field of grass, dirt and rocks (Perlin noise).
// Unload it with UnloadTexture() when done.
Texture2D GenerateBackground(void);

#endif // BACKGROUND_H
