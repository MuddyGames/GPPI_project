#include <stdio.h>
#include <raylib.h>

#include "game/game.h"

#include "utils/input_manager.h"
#include "utils/ai_manager.h"

// Frees everything InitGame() created, but not the GameData itself (see below)
static void UnloadGameData(GameData *data);

// Makes a new random background (see below)
static Texture2D GenerateBackground(void);

/**
 * GenerateBackground : Paints a fresh field of grass, dirt and rocks.
 *
 * Three layers of Perlin noise, blended pixel by pixel. The noise offsets
 * are random, so every game (and every restart) gets a different field ...
 * like the Irish weather, you never get the same one twice.
 *
 * @return: The background texture. Unload it with UnloadTexture() when done.
 */

static Texture2D GenerateBackground(void)
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

/**
 * CheckGameOver : Is anyone out of health?
 *
 * Whoever's still standing wins. Both down on the same frame (they can hit
 * each other at once) is a draw, and the winner is NULL.
 *
 * Reports it to the mediator once: the mediator logs it, plays the sound and
 * tells the HUD. game.c just says what happened.
 */

static void CheckGameOver(GameData *data)
{
	GameObject *player = &data->player->base;
	GameObject *npc = &data->npc->base;

	// Both still standing: on we go
	if (player->health > 0 && npc->health > 0)
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
		CheckGameOver(data);			   // anyone out of health?
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

// Draw GameObject HealthBar
static void DrawGameObjectHealthBar(const GameObject *object)
{
	// Healthbar Dimensions
	const int healthBarWidth = 100;
	const int healthBarHeight = 10;

	// Center above GameObject
	const int healthBarX = object->position.x - (healthBarWidth / 2);
	const int healthBarY = object->position.y - 40;

	// Make sure health is between 0 and 100
	int health = object->health;
	if (health < 0)
		health = 0;
	if (health > 100)
		health = 100;

	// Bar width
	float healthPercentage = health / 100.0f;
	int currentHealth = (int)(healthBarWidth * healthPercentage);
	int lostHealth = healthBarWidth - currentHealth;
	lostHealth = Clamp(lostHealth, 0, healthBarWidth);

	// Colours
	Color background = ColorAlpha(GRAY, 0.5f);
	Color currentHealthColor = ColorAlpha(GREEN, 0.5f);
	Color lostHealthColor = ColorAlpha(RED, 0.5f);

	// Background (gray)
	DrawRectangle(healthBarX, healthBarY, healthBarWidth, healthBarHeight, background);

	// Current health (green)
	DrawRectangle(healthBarX, healthBarY, currentHealth, healthBarHeight, currentHealthColor);

	// Lost health (red)
	DrawRectangle(healthBarX + currentHealth, healthBarY, lostHealth, healthBarHeight, lostHealthColor);
}

// Draw the GameObject in this example a simple Circle which is a collider
static void DrawGameObjectColliderCircle(const GameObject *object)
{
	// Set alpha for collisision circle
	Color background = ColorAlpha(object->color, 0.5f);

	// Circle Thickness
	int thickness = 5;

	// GameObject Circle
	DrawRing(
		object->position,					// Vector2 (x, y)
		object->collider.r - thickness / 2, // inner radius
		object->collider.r + thickness / 2, // outer radius
		0.0f, 360.0f,						// start angle, end angle
		64,									// segments
		background							// Color
	);
}

// Draw position information at the bottom of the GameObject
static void DrawGameObjectPositionInfo(const GameObject *object)
{
	const char *information = TextFormat("(%.f, %.f)",
										 object->position.x,
										 object->position.y);
	DrawText(
		information,
		object->position.x - (MeasureText(information, DEFAULT_FONT_SIZE) / 2), // Measure the text with with MeasureText
		object->position.y + 30,												// Place it below the GameObject
		DEFAULT_FONT_SIZE, DARKGRAY);
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
	// Draw Background
	DrawTexture(data->background, 0, 0, WHITE);

	//---------------------------------------------------------
	// Drawing NPC and Position Data
	// NPC Circle
	//---------------------------------------------------------
	DrawGameObjectColliderCircle(&data->npc->base);

	// Render the npc's animation at their current position
	DrawAnimation(&data->npc->base.animation, data->npc->base.position, RAYWHITE);

	// Player Position Information
	DrawGameObjectPositionInfo(&data->npc->base);

	// Drawing Health Bar for the npc
	DrawGameObjectHealthBar(&data->npc->base);

	//---------------------------------------------------------
	// Drawing Player and Position Data
	// Player Circle
	//---------------------------------------------------------
	DrawGameObjectColliderCircle(&data->player->base);

	// Render the player's animation at their current position
	DrawAnimation(&data->player->base.animation, data->player->base.position, WHITE);

	// Player Position Information
	DrawGameObjectPositionInfo(&data->player->base);

	// Drawing Health Bar for the player
	DrawGameObjectHealthBar(&data->player->base);

	// Drawing HUD (on top of the game objects)
	DrawHud(&data->hud);
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
