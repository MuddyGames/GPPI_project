#include <string.h>
#include <time.h>
#include <stdio.h>
#include <raylib.h>

#include "./utils/constants.h"
#include "./gameobjects/gameobject.h"
#include "./game/game.h"

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
	// Init raylib window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Command Pattern with Animated FSM Starter Kit GPP I");

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
