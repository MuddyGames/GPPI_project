#include <stdio.h>
#include <raylib.h>

#include "game/game.h"
#include "game/background.h"

#include "utils/input_manager.h"
#include "utils/ai_manager.h"

// Frees everything InitGame() created, but not the GameData itself (see below)
static void UnloadGameData(GameData *data);

/**
 * InitGame : Sets everything up before the madness begins.
 *
 * Creates player, NPC, HUD and mediator that stops them
 * shouting directly at the FSM. Also fires up the input and AI systems.
 * Oh and as Ireland is so green (from all the rain), a nice lush grass background,
 * a new field of grass every time. Remember how hot it was in 2026? no green fields then!
 *
 * @data: The GameData structure that holds the whole operation together
 */

void InitGame(GameData *data)
{
	// Initialise Game
	data->gsm = STATE_PLAY;

	TraceLog(LOG_INFO, "[Game] Initialising");

	// Initialize the player and NPC with their respective names
	// Setup position
	Vector2 playerPosition = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	Vector2 npcPosition = (Vector2){GetScreenWidth() / 2.0f, 100.0f};

	data->player = InitPlayer("Player Hero", playerPosition, DEFAULT_GAMEOBJECT_RADIUS);
	data->npc = InitNPC("The Villan", npcPosition, DEFAULT_GAMEOBJECT_RADIUS);

	// Initialise HUD
	InitHud(&data->hud);

	// Create a mediator to facilitate communication between
	// Player, Hud, Command and FSM, ultimately updating the players state
	// One mediator for the game: it knows the player, the NPC and the HUD,
	// so they never need to know about each other
	data->mediator = CreateMediator(&data->player->base, &data->npc->base, &data->hud);

	// A new background every game (see GenerateBackground above)
	data->background = GenerateBackground();

	// Initialise Input Managers
	InitInputManager();
	InitAIManager();

	TraceLog(LOG_INFO, "[Game] Initialised");

	MediatorNotify(data->mediator, GAME_EVENT_LEVEL_START, NULL, NULL);
}

// ---------------------------------------------------------------------------
// UpdateGame's helpers: one job each, so UpdateGame() reads like a list
// ---------------------------------------------------------------------------

/**
 * UpdatePlayer : The player's turn. Input -> mediator -> FSM.
 */

static void UpdatePlayer(GameData *data, float deltaTime)
{
	// What's the player pressing? The mediator passes it on (and tells the HUD)
	Command command = PollInput();
	MediatorReceiveCommand(data->mediator, &data->player->base, command, deltaTime);

	// Run the player's current state (animation, walking, the swipe ...)
	UpdateState(&data->player->base, deltaTime);
}

/**
 * UpdateNPC : The NPC's turn. AI -> mediator -> FSM, same road as the player.
 *
 * The AI makes up its mind once a second (see ai_manager.c), which is
 * generous given its low IQ.
 */

static void UpdateNPC(GameData *data, float deltaTime)
{
	// What does the AI want? Same mediator, same road in as the player's commands
	Command command = PollAI();
	MediatorReceiveCommand(data->mediator, &data->npc->base, command, deltaTime);

	// The NPC's target is always the player: it chases it while Walking
	data->npc->target = data->player->base.position;

	// The longer it chases, the angrier (and faster) it gets (see npc.c)
	UpdateNPCAggression(data->npc, deltaTime);

	// Run the NPC's current state
	UpdateState(&data->npc->base, deltaTime);
}

/**
 * UpdateCollisions : Have they bumped into each other?
 *
 * Three cases, like a door: just walked in (ENTER), still in the doorway
 * (ONGOING), just walked out (EXIT). The FSMs hear about ENTER and EXIT as
 * events; the push-back keeps them from standing inside each other.
 */

static void UpdateCollisions(GameData *data, float deltaTime)
{
	GameObject *player = &data->player->base;
	GameObject *npc = &data->npc->base;

	bool isColliding = CheckCollision(player, npc);

	if (isColliding && !player->isColliding)
	{
		// ENTER: tell the FSMs (the NPC swings when it catches the player)
		HandleEvent(player, EVENT_COLLISION_START, deltaTime);
		HandleEvent(npc, EVENT_COLLISION_START, deltaTime);

		CollisionEntry(player, npc);
		HandleCollision(player, npc);
	}
	else if (isColliding && player->isColliding)
	{
		// ONGOING: keep them apart
		HandleCollision(player, npc);
	}
	else if (!isColliding && player->isColliding)
	{
		// EXIT
		HandleEvent(player, EVENT_COLLISION_END, deltaTime);
		HandleEvent(npc, EVENT_COLLISION_END, deltaTime);

		CollisionExit(player, npc);
	}

	// Remember for next frame
	player->isColliding = isColliding;
	npc->isColliding = isColliding;
}

/**
 * UpdateCombat : Sword swipes land (or don't).
 *
 * Damage lands once per swipe, on the hit frame (see AttackHit in collision.c).
 * AttackHit reports hits, blocks and misses to the mediator itself.
 */

static void UpdateCombat(GameData *data)
{
	AttackHit(&data->player->base, &data->npc->base);
	AttackHit(&data->npc->base, &data->player->base);
}

// Out of health, but not lying down yet? Send EVENT_DIE: the Dead state plays the fall
static void KnockOut(GameObject *object, float deltaTime)
{
	if (object->health <= 0 && object->currentState != STATE_DEAD)
		HandleEvent(object, EVENT_DIE, deltaTime);
}

// Has this fighter finished falling over? (Dead, and its death animation played out)
static bool HasFallen(const GameObject *object)
{
	return object->currentState == STATE_DEAD && object->animation.finished;
}

/**
 * CheckGameOver : Is anyone out of health?
 *
 * Two steps, so the death animation gets to play:
 *   1. Out of health? EVENT_DIE, and the Dead state plays the fall.
 *      The game carries on meanwhile (the winner can still walk about).
 *   2. Once every fighter who's down has finished falling: game over.
 *
 * Whoever's still standing wins. Both down at once (they can hit each other
 * on the same frame) is a draw, and the winner is NULL.
 *
 * Reports it to the mediator once: the mediator logs it, plays the sound and
 * tells the HUD. game.c just says what happened.
 */

static void CheckGameOver(GameData *data, float deltaTime)
{
	GameObject *player = &data->player->base;
	GameObject *npc = &data->npc->base;

	// 1. Down? Fall over (does nothing for anyone still standing, or already Dead)
	KnockOut(player, deltaTime);
	KnockOut(npc, deltaTime);

	// Both still standing: on we go
	if (player->health > 0 && npc->health > 0)
		return;

	// 2. Wait until everyone who's down has finished falling over
	if (player->health <= 0 && !HasFallen(player))
		return;
	if (npc->health <= 0 && !HasFallen(npc))
		return;

	data->gsm = STATE_GAMEOVER;

	GameObject *winner = NULL; // A draw, unless someone's still standing
	if (player->health > 0)
		winner = player;
	else if (npc->health > 0)
		winner = npc;

	MediatorNotify(data->mediator, GAME_EVENT_GAME_OVER, winner, NULL);
}

/**
 * UpdateGame : Runs one tick of game logic.
 *
 * Reads like the game loop itself: player, NPC, collisions, combat, game over.
 * Each step is its own small function above, so when you add something new
 * (pickups, a second NPC ...) it gets its own line here, not 20 lines in the middle.
 *
 * @data:      All the game's current state.
 * @deltaTime: Time since the last update.
 */

void UpdateGame(GameData *data, float deltaTime)
{
	switch (data->gsm)
	{
	case STATE_PLAY:
		UpdatePlayer(data, deltaTime);	   // input -> mediator -> FSM
		UpdateNPC(data, deltaTime);		   // AI -> mediator -> FSM, chase the player
		UpdateCollisions(data, deltaTime); // enter / ongoing / exit
		UpdateCombat(data);				   // sword hits land on the hit frame
		CheckGameOver(data, deltaTime);	   // anyone down? fall over, then game over
		break;

	case STATE_GAMEOVER:
		// R (or Start on a gamepad) for another go
		if (PollRestart())
		{
			// Free the old game first, or every restart leaks the player, the NPC,
			// the mediator and three textures
			UnloadGameData(data);
			InitGame(data);
		}
		break;
	}
}

/**
 * DrawGame : Draws all the craic on screen.
 *
 * Renders the player, the NPC, their positions, health bars,
 * and whatever animations they've decided to show off.
 *
 * @data: The game state to render.
 */

void DrawGame(const GameData *data)
{
	// Back to front: whatever is drawn last ends up on top
	DrawTexture(data->background, 0, 0, WHITE); // The field
	DrawGameObject(&data->npc->base);           // The NPC
	DrawGameObject(&data->player->base);        // The player, over the NPC
	DrawHud(&data->hud);                        // The HUD, over everything
}

/**
 * CloseGame - Shuts everything down gracefully.
 *
 * Runs the cleanup routine so you're not leaving memory lying around
 * like empty pint glasses after closing time (in Dinn Ri again).
 *
 * @data: The game data to be disposed of.
 */

void CloseGame(GameData *data)
{
	TraceLog(LOG_INFO, "[Game] Closed");

	// If the game data is not null, delete all objects associated with the game
	if (data != NULL)
	{
		DeleteGameData(data);
	}
}

/**
 * UnloadGameData - Frees everything InitGame() created.
 *
 * Deletes player, NPC, mediator and the background, but leaves the
 * GameData itself alone, so InitGame() can fill it in again on a restart.
 * Pointers are set to NULL after, so nothing points at freed memory.
 *
 * @data: The structure to empty out.
 */

static void UnloadGameData(GameData *data)
{
	if (data == NULL)
		return;

	// Delete the player and NPC objects if they are not null
	if (data->player != NULL)
	{
		DeletePlayer(&data->player->base);
		data->player = NULL;
	}

	if (data->npc != NULL)
	{
		DeleteNPC(&data->npc->base);
		data->npc = NULL;
	}

	// Free the mediator (free(NULL) is safe)
	DeleteMediator(data->mediator);
	data->mediator = NULL;

	// Free the background
	UnloadTexture(data->background);
	data->background = (Texture2D){0};
}

/**
 * DeleteGameData - Frees every object in the game's data.
 *
 * Unloads player, NPC, mediator and background, and finally frees the GameData
 * itself. No memory leaks here, thank you very much.
 *
 * @data: The structure to purge (I've watched a few)
 * "The unwritten Purge rule: don't save lives" but do free memory
 */

void DeleteGameData(GameData *data)
{
	UnloadGameData(data);

	// Free GameData
	free(data); // Free game data memory
}
