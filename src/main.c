#include <string.h>
#include <time.h>
#include <stdio.h>
#include <raylib.h>

#include "utils/constants.h"
#include "gameobjects/gameobject.h"
#include "game/game.h"
#include "utils/audio_manager.h"

// Function Prototypes
// Game Loop Function
void GameLoop(GameData *data);

/**
 * main - Entry point of the whole shebang.
 *
 * Sets up the Raylib window, fires up the game systems, and kicks off
 * the main loop. When the player finally closes the window, everything
 * gets cleaned up properly ..... none of this "sure it's grand" memory
 * leaking is no problem I've loads after shelling out for another 64 GB of DDR5.
 */

int main(void)
{
	// Set the log level to debug for detailed output
	// To turn off logging, you can set it to LOG_NONE
	// Debug levels are: LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL
	SetTraceLogLevel(LOG_INFO);

	// Init raylib window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GPP I Project Starter Kit");

	// Init Audio (once, after the window: a restart reuses the same sounds)
	InitAudioManager();

	// Create GameData Pointer
	GameData *data = (GameData *)(malloc(sizeof(GameData)));

	// Initialise Game
	InitGame(data);

	// Set Target FPS
	SetTargetFPS(TARGET_FPS);

	// Raylib while loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Call GameLoop
		GameLoop(data);
	}

	// Free resources
	CloseGame(data);

	// Close Audio (before the window)
	DeleteAudioManager();

	// Close Raylib Window
	CloseWindow();

	return 0;
}

/**
 * GameLoop - One full cycle of updating and drawing.
 *
 * Updates the game world first (because drawing before thinking rarely ends well),
 * then starts the Raylib drawing phase and renders the whole scene.
 *
 * @data: Pointer to the shared GameData struct.
 */

void GameLoop(GameData *data)
{
	// Update Game Data
	// Should be outside BeginDrawing(); and EndDrawing();
	float deltaTime = GetFrameTime(); // Get delta time frame time expressed in seconds

	UpdateGame(data, deltaTime);

	// Raylib function Draw
	BeginDrawing();

	// Clear the screen with a grren background
	ClearBackground(DARKGREEN);

	// Draw the Game Objects
	DrawGame(data);

	// Raylib End drawing to Frame Buffer
	EndDrawing();
}
