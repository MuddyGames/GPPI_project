#ifndef GAME_H
#define GAME_H

#include "utils/mediator.h"
#include "gameobjects/player.h"
#include "gameobjects/npc.h"
#include "utils/collision.h"

typedef enum
{
	STATE_PLAY,
	STATE_GAMEOVER
} GameState;

// Define the GameData struct to store the main game components (player, npc, and mediator)
typedef struct
{
	// Game Objects
	Player *player;			  // Pointer to the Player object
	NPC *npc;				  // Pointer to the NPC object

	// Mediator between command and FSM
	Mediator *playerMediator; // Pointer to the Mediator object for managing interactions
	Mediator *npcMediator;	  // Pointer to the Mediator object for managing interactions

	// Simple Background image (Perlin noise)
	Texture background;

	// Game State Manager
	GameState gsm;

} GameData;

// Initialises the game components (player, npc, mediator)
void InitGame(GameData *data);

// Updates the game state each frame (handles game logic)
void UpdateGame(GameData *data, float deltaTime);

// Draws or renders the current game state (e.g., player, npc, environment)
void DrawGame(const GameData *data);

// Closes the game, performing necessary cleanup and freeing resources
void CloseGame(GameData *data);

// Frees memory associated with GameData and its components (player, npc, mediator)
void DeleteGameData(GameData *data);

#endif // GAME_H