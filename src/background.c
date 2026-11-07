#include <raylib.h>

#include "game/background.h"
#include "utils/constants.h"

/**
 * GenerateBackground : Paints a fresh field of grass, dirt and rocks.
 *
 * Three layers of Perlin noise, blended pixel by pixel. The noise offsets
 * are random, so every game (and every restart) gets a different field ...
 * like the Irish weather, you never get the same one twice.
 *
 * @return: The background texture. Unload it with UnloadTexture() when done.
 */

Texture2D GenerateBackground(void)
{
	// Random offsets into the noise, so the field is different every time
	// (raylib seeds GetRandomValue() when the window opens)
	// Background Grass... looks better than a plain background
	// https://github.com/raysan5/raylib/blob/master/examples/textures/textures_image_generation.c
	Image grass = GenImagePerlinNoise(SCREEN_WIDTH, SCREEN_HEIGHT, GetRandomValue(0, 10000), GetRandomValue(0, 10000), 6.0f);
	ImageFormat(&grass, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

	// https://www.color-hex.com/color-palette/46612
	Color darkGrass = (Color){20, 66, 63, 255};
	Color lightGrass = (Color){48, 107, 64, 255};

	Image dirt = GenImagePerlinNoise(SCREEN_WIDTH, SCREEN_HEIGHT, GetRandomValue(0, 10000), GetRandomValue(0, 10000), 4.0f);
	ImageFormat(&dirt, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

	// https://www.color-hex.com/color-palette/46612
	Color darkDirt = (Color){58, 39, 43, 255};
	Color lightDirt = (Color){92, 70, 62, 255};

	// https://www.color-hex.com/color-palette/46612
	Image rocks = GenImagePerlinNoise(SCREEN_WIDTH, SCREEN_HEIGHT, GetRandomValue(0, 10000), GetRandomValue(0, 10000), 3.0f);
	ImageFormat(&rocks, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

	// https://www.color-hex.com/color-palette/46612
	Color darkRocks = (Color){90, 76, 66, 255};
	Color lightRocks = (Color){163, 162, 165, 255};

	// PixelData
	Color *grassPixels = (Color *)grass.data;
	Color *dirtPixels = (Color *)dirt.data;
	Color *rockPixels = (Color *)rocks.data;

	// Acts like a fragment shader coloring each pixel
	int pixelCount = SCREEN_WIDTH * SCREEN_HEIGHT;

	// Precalculate for performance
	const int deltaGrassRed = lightGrass.r - darkGrass.r;
	const int deltaGrassGreen = lightGrass.g - darkGrass.g;
	const int deltaGrassBlue = lightGrass.b - darkGrass.b;

	const int deltaDirtRed = lightDirt.r - darkDirt.r;
	const int deltaDirtGreen = lightDirt.g - darkDirt.g;
	const int deltaDirtBlue = lightDirt.b - darkDirt.b;

	const int deltaRocksRed = lightRocks.r - darkRocks.r;
	const int deltaRocksGreen = lightRocks.g - darkRocks.g;
	const int deltaRocksBlue = lightRocks.b - darkRocks.b;

	for (int i = 0; i < pixelCount; i++)
	{
		// Use green component like a scale (normalised)
		float t = grassPixels[i].g * (1 / 255.0f);
		float noise = grassPixels[i].b * (1 / 255.0f);

		if (dirtPixels[i].r < 55)
		{
			grassPixels[i].r = darkDirt.r + (unsigned char)(t * deltaDirtRed);
			grassPixels[i].g = darkDirt.g + (unsigned char)(t * deltaDirtGreen);
			grassPixels[i].b = darkDirt.b + (unsigned char)(t * deltaDirtBlue);
		}
		else if (rockPixels[i].r < 70)
		{
			grassPixels[i].r = darkRocks.r + (unsigned char)(t * deltaRocksRed);
			grassPixels[i].g = darkRocks.g + (unsigned char)(t * deltaRocksGreen);
			grassPixels[i].b = darkRocks.b + (unsigned char)(t * deltaRocksBlue);
		}
		else
		{
			grassPixels[i].r = darkGrass.r + (unsigned char)(t * deltaGrassRed);
			grassPixels[i].g = darkGrass.g + (unsigned char)(t * deltaGrassGreen);
			grassPixels[i].b = darkGrass.b + (unsigned char)(t * deltaGrassBlue);
		}
		grassPixels[i].a = 148 + (unsigned char)(noise * 30.0f);
	}

	// Blended grass set as background
	Texture2D background = LoadTextureFromImage(grass);
	UnloadImage(grass); // Free up as Texture was produced
	UnloadImage(dirt);	// Free up as Texture was produced
	UnloadImage(rocks); // Free up as Texture was produced

	return background;
}
