#include "gameobjects/player.h"
#include "utils/utils.h"
#include "utils/audio_manager.h"

/*
 * Sprite sheet layout (resources/player_sprite_sheet.png)
 *
 * Grid is 64 x 64: Row n starts at Pixel Y = (n - 1) x 64, Column c at Pixel X = (c - 1) x 64
 * Sword swipes are 192 x 192 (3 x 3 grid cells) at the bottom of the sheet, Rows 47-58:
 *     Pixel Y = (first row - 1) x 64, Pixel X = (Column - 1) x 192
 *
 * Each Rectangle below is {Pixel X, Pixel Y, width, height}
 */

/**
 * InitPlayer - Spins up a fresh Player.
 *
 * Grabs memory, loads the sprite sheet, sets up the collider and FSM,
 * and drops the poor eejit in the middle of the screen with full health,
 * full stamina, and not a clue what's coming.
 */

Player *InitPlayer(const char *name, Vector2 position, int radius)
{
	// Allocate memory for the Player structure
	Player *player = (Player *)malloc(sizeof(Player));

	// Check if memory allocation failed
	if (!player)
	{
		// Print an error message to stderr and terminate the program if allocation fails
		fprintf(stderr, "Failed to allocate player\n");
		exit(1);
	}

	// Load player texture
	Texture2D texture = LoadTexture("resources/player_sprite_sheet.png");

	// Setup Collider
	c2Circle collider = (c2Circle){.p = {position.x, position.y}, .r = radius};

	InitGameObject(&player->base,
				   name,	   // Name
				   position,   // Position
				   STATE_IDLE, // Initial State
				   DARKGREEN,  // Player Color
				   collider,   // cute_c2 Circle Collider
				   texture,	   // Player spritesheet
				   100		   // Initial Health
	);

	// Player Specific Data
	player->stamina = 100.0f;
	player->mana = 100.0f;

	// Init the Player FSM
	InitPlayerFSM(&player->base);

	if (player->base.currentState == STATE_IDLE)
	{
		// Initialize the idle animation immediately
		PlayerEnterIdle(&player->base, 0.0f); // Trigger idle animation at initialization
	}

	return player;
}

/**
 * DeletePlayer - Clears out a Player and its base GameObject.
 *
 * If the Player ever owned anything fancy (swords, shields, emotional baggage),
 * this would be the place to free it. For now we just hand it off to DeleteGameObject.
 */

void DeletePlayer(GameObject *object)
{
	// Perform any player-specific cleanup here
	// Cast to Player if player-specific cleanup is required
	// Player *player = (Player *)object;
	// Example of potential cleanup (not implemented here):
	// If the player is holding a dynamically allocated object, such as a Shield:
	// free(player->holding);
	// Perform any player-specific cleanup here
	DeleteGameObject(object);
}

/**
 * InitPlayerFSM - Sets up all the Player states and their rules.
 *
 * Wires up which states can follow which, and which functions get called
 * when we enter, update or exit them. It's the bouncer at the door saying
 * "you're not going from Dead to Walking, pal". Go clean yourself off, dust
 * yourself down and respawn there like a good lad.
 */

void InitPlayerFSM(GameObject *object)
{
	// Allocate memory for the state configurations array with a size for all possible states
	object->stateConfigs = AllocateStateConfigs(STATE_COUNT);

	// ---- STATE_IDLE state configuration ----
	// Define valid transitions from STATE_IDLE
	EventStateTransition idleValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_MOVE, STATE_WALKING},
		{EVENT_ATTACK, STATE_ATTACKING},
		{EVENT_DEFEND, STATE_SHIELD},
		{EVENT_DIE, STATE_DEAD}};
	// Set up the state configuration for STATE_IDLE
	InitStateConfig(object, STATE_IDLE, "Player_Idle", PlayerEnterIdle, PlayerUpdateIdle, PlayerExitIdle);
	// Configure valid transitions for STATE_IDLE
	StateTransitions(&object->stateConfigs[STATE_IDLE], idleValidTransitions, sizeof(idleValidTransitions) / sizeof(EventStateTransition));

	// ---- STATE_WALKING state configuration ----
	// Define valid transitions from STATE_WALKING
	EventStateTransition walkingValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_NONE, STATE_IDLE},
		{EVENT_ATTACK, STATE_ATTACKING},
		{EVENT_DIE, STATE_DEAD}};
	// Set up the state configuration for STATE_WALKING
	InitStateConfig(object, STATE_WALKING, "Player_Walking", PlayerEnterWalking, PlayerUpdateWalking, PlayerExitWalking);
	// Configure valid transitions for STATE_WALKING
	StateTransitions(&object->stateConfigs[STATE_WALKING], walkingValidTransitions, sizeof(walkingValidTransitions) / sizeof(EventStateTransition));

	// ---- STATE_ATTACKING state configuration ----
	// Define valid transitions from STATE_ATTACKING
	// A swipe always finishes: letting go of SPACE or pressing a direction doesn't cut it short.
	// When it ends the player goes Idle, and from there straight back to Walking
	// (direction held) or into another swipe (SPACE held) on the next frame.
	EventStateTransition attackValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_ANIMATION_END, STATE_IDLE},
		{EVENT_DIE, STATE_DEAD}};
	// Set up the state configuration for STATE_ATTACKING
	InitStateConfig(object, STATE_ATTACKING, "Player_Attacking", PlayerEnterAttacking, PlayerUpdateAttacking, PlayerExitAttacking);
	// Configure valid transitions for STATE_ATTACKING
	StateTransitions(&object->stateConfigs[STATE_ATTACKING], attackValidTransitions, sizeof(attackValidTransitions) / sizeof(EventStateTransition));

	// ---- STATE_SHIELD state configuration ----
	// Define valid transitions from STATE_SHIELD
	EventStateTransition sheildingValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_NONE, STATE_IDLE},
		{EVENT_ATTACK, STATE_ATTACKING},
		{EVENT_DIE, STATE_DEAD}};
	// Set up the state configuration for STATE_SHIELD
	InitStateConfig(object, STATE_SHIELD, "Player_Shielding", PlayerEnterShielding, PlayerUpdateShielding, PlayerExitShielding);
	// Configure valid transitions for STATE_SHIELD
	StateTransitions(&object->stateConfigs[STATE_SHIELD], sheildingValidTransitions, sizeof(sheildingValidTransitions) / sizeof(EventStateTransition));

	// ---- STATE_DEAD state configuration ----
	// Define valid transitions from STATE_DEAD
	EventStateTransition deadValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_RESPAWN, STATE_RESPAWN}};
	// Set up the state configuration for STATE_DEAD
	InitStateConfig(object, STATE_DEAD, "Player_Dead", PlayerEnterDie, PlayerUpdateDie, PlayerExitDie);
	// Configure valid transitions for STATE_DEAD
	StateTransitions(&object->stateConfigs[STATE_DEAD], deadValidTransitions, sizeof(deadValidTransitions) / sizeof(EventStateTransition));

	// ---- STATE_RESPAWN state configuration ----
	// Define valid transitions from STATE_RESPAWN
	EventStateTransition respawnValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_RESPAWN, STATE_IDLE}};
	// Set up the state configuration for STATE_RESPAWN
	InitStateConfig(object, STATE_RESPAWN, "Player_Respawn", PlayerEnterRespawn, PlayerUpdateRespawn, PlayerExitRespawn);
	// Configure valid transitions for STATE_RESPAWN
	StateTransitions(&object->stateConfigs[STATE_RESPAWN], respawnValidTransitions, sizeof(respawnValidTransitions) / sizeof(EventStateTransition));

	// Print out Configs
	PrintStateConfigs(object->stateConfigs, STATE_COUNT);
}

/**
 * SelectRandomIdleAnimation - Picks a random idle from the sheet.
 *
 * Just to keep the lad from looking like a statue. Chooses one of several
 * rows and wires it into the animation system.
 */

void SelectRandomIdleAnimation(GameObject *object, float deltaTime)
{
	(void)deltaTime;

	// See grid_player_sprite_sheet.png for rows and columns
	int randomChoice = rand() % 7 + 1;

	// IDLE 1 (Row 6, Pixel : Y = 5 x 64 = 320)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle idle1[8] = {
		{0, 320, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
		{64, 320, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
		{128, 320, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
		{192, 320, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
		{256, 320, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
		{320, 320, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
		{384, 320, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
		{448, 320, 64, 64}    // Frame 8: Column 8 (Pixel : X 448)
	};

	// IDLE 2 (Row 7, Pixel : Y = 6 x 64 = 384)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle idle2[8] = {
		{0, 384, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
		{64, 384, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
		{128, 384, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
		{192, 384, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
		{256, 384, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
		{320, 384, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
		{384, 384, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
		{448, 384, 64, 64}    // Frame 8: Column 8 (Pixel : X 448)
	};

	// IDLE 3 (Row 8, Pixel : Y = 7 x 64 = 448)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle idle3[8] = {
		{0, 448, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
		{64, 448, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
		{128, 448, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
		{192, 448, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
		{256, 448, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
		{320, 448, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
		{384, 448, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
		{448, 448, 64, 64}    // Frame 8: Column 8 (Pixel : X 448)
	};

	// IDLE 4 (Row 17, Pixel : Y = 16 x 64 = 1024)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle idle4[13] = {
		{0, 1024, 64, 64},    // Frame 1: Column 1 (Pixel : X 0)
		{64, 1024, 64, 64},   // Frame 2: Column 2 (Pixel : X 64)
		{128, 1024, 64, 64},  // Frame 3: Column 3 (Pixel : X 128)
		{192, 1024, 64, 64},  // Frame 4: Column 4 (Pixel : X 192)
		{256, 1024, 64, 64},  // Frame 5: Column 5 (Pixel : X 256)
		{320, 1024, 64, 64},  // Frame 6: Column 6 (Pixel : X 320)
		{384, 1024, 64, 64},  // Frame 7: Column 7 (Pixel : X 384)
		{448, 1024, 64, 64},  // Frame 8: Column 8 (Pixel : X 448)
		{512, 1024, 64, 64},  // Frame 9: Column 9 (Pixel : X 512)
		{576, 1024, 64, 64},  // Frame 10: Column 10 (Pixel : X 576)
		{640, 1024, 64, 64},  // Frame 11: Column 11 (Pixel : X 640)
		{704, 1024, 64, 64},  // Frame 12: Column 12 (Pixel : X 704)
		{768, 1024, 64, 64}   // Frame 13: Column 13 (Pixel : X 768)
	};

	// IDLE 5 (Row 18, Pixel : Y = 17 x 64 = 1088)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle idle5[13] = {
		{0, 1088, 64, 64},    // Frame 1: Column 1 (Pixel : X 0)
		{64, 1088, 64, 64},   // Frame 2: Column 2 (Pixel : X 64)
		{128, 1088, 64, 64},  // Frame 3: Column 3 (Pixel : X 128)
		{192, 1088, 64, 64},  // Frame 4: Column 4 (Pixel : X 192)
		{256, 1088, 64, 64},  // Frame 5: Column 5 (Pixel : X 256)
		{320, 1088, 64, 64},  // Frame 6: Column 6 (Pixel : X 320)
		{384, 1088, 64, 64},  // Frame 7: Column 7 (Pixel : X 384)
		{448, 1088, 64, 64},  // Frame 8: Column 8 (Pixel : X 448)
		{512, 1088, 64, 64},  // Frame 9: Column 9 (Pixel : X 512)
		{576, 1088, 64, 64},  // Frame 10: Column 10 (Pixel : X 576)
		{640, 1088, 64, 64},  // Frame 11: Column 11 (Pixel : X 640)
		{704, 1088, 64, 64},  // Frame 12: Column 12 (Pixel : X 704)
		{768, 1088, 64, 64}   // Frame 13: Column 13 (Pixel : X 768)
	};

	// IDLE 6 (Row 19, Pixel : Y = 18 x 64 = 1152)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle idle6[13] = {
		{0, 1152, 64, 64},    // Frame 1: Column 1 (Pixel : X 0)
		{64, 1152, 64, 64},   // Frame 2: Column 2 (Pixel : X 64)
		{128, 1152, 64, 64},  // Frame 3: Column 3 (Pixel : X 128)
		{192, 1152, 64, 64},  // Frame 4: Column 4 (Pixel : X 192)
		{256, 1152, 64, 64},  // Frame 5: Column 5 (Pixel : X 256)
		{320, 1152, 64, 64},  // Frame 6: Column 6 (Pixel : X 320)
		{384, 1152, 64, 64},  // Frame 7: Column 7 (Pixel : X 384)
		{448, 1152, 64, 64},  // Frame 8: Column 8 (Pixel : X 448)
		{512, 1152, 64, 64},  // Frame 9: Column 9 (Pixel : X 512)
		{576, 1152, 64, 64},  // Frame 10: Column 10 (Pixel : X 576)
		{640, 1152, 64, 64},  // Frame 11: Column 11 (Pixel : X 640)
		{704, 1152, 64, 64},  // Frame 12: Column 12 (Pixel : X 704)
		{768, 1152, 64, 64}   // Frame 13: Column 13 (Pixel : X 768)
	};

	// IDLE 7 (Row 20, Pixel : Y = 19 x 64 = 1216)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle idle7[13] = {
		{0, 1216, 64, 64},    // Frame 1: Column 1 (Pixel : X 0)
		{64, 1216, 64, 64},   // Frame 2: Column 2 (Pixel : X 64)
		{128, 1216, 64, 64},  // Frame 3: Column 3 (Pixel : X 128)
		{192, 1216, 64, 64},  // Frame 4: Column 4 (Pixel : X 192)
		{256, 1216, 64, 64},  // Frame 5: Column 5 (Pixel : X 256)
		{320, 1216, 64, 64},  // Frame 6: Column 6 (Pixel : X 320)
		{384, 1216, 64, 64},  // Frame 7: Column 7 (Pixel : X 384)
		{448, 1216, 64, 64},  // Frame 8: Column 8 (Pixel : X 448)
		{512, 1216, 64, 64},  // Frame 9: Column 9 (Pixel : X 512)
		{576, 1216, 64, 64},  // Frame 10: Column 10 (Pixel : X 576)
		{640, 1216, 64, 64},  // Frame 11: Column 11 (Pixel : X 640)
		{704, 1216, 64, 64},  // Frame 12: Column 12 (Pixel : X 704)
		{768, 1216, 64, 64}   // Frame 13: Column 13 (Pixel : X 768)
	};

	switch (randomChoice)
	{
	case 1:
		InitGameObjectAnimation(object, idle1, 8, 0.18f); // Animation 1
		break;
	case 2:
		InitGameObjectAnimation(object, idle2, 8, 0.18f); // Animation 2
		break;
	case 3:
		InitGameObjectAnimation(object, idle3, 8, 0.18f); // Animation 3
		break;
	case 4:
		InitGameObjectAnimation(object, idle4, 13, 0.18f); // Animation 4
		break;
	case 5:
		InitGameObjectAnimation(object, idle5, 13, 0.18f); // Animation 5
		break;
	case 6:
		InitGameObjectAnimation(object, idle6, 13, 0.18f); // Animation 6
		break;
	case 7:
		InitGameObjectAnimation(object, idle7, 13, 0.18f); // Animation 7
		break;
	default:
		InitGameObjectAnimation(object, idle1, 8, 0.18f); // Default to Animation 1
		break;
	}
}

void PlayerEnterIdle(GameObject *object, float deltaTime)
{
	// Player *player = (Player *)object;
	// printf("\n%s -> ENTER -> Idle\n", object->name);
	// printf("Stamina: %.1f, Mana: %.1f\n\n", player->stamina, player->mana);

	if (object->previousState != object->currentState && object->currentState == STATE_IDLE)
	{
		SelectRandomIdleAnimation(object, deltaTime);
	}
}

void PlayerUpdateIdle(GameObject *object, float deltaTime)
{
	// Player *player = (Player *)object;
	//  Complete the remainder of the method
	int frameBefore = object->animation.currentFrame;
	UpdateAnimation(&object->animation, deltaTime);

	// Check if the animation has finished: the last frame has been shown and it looped back to the start
	if (frameBefore == object->animation.frameCount - 1 && object->animation.currentFrame == 0)
	{
		// Transition to a another idle animation
		SelectRandomIdleAnimation(object, deltaTime); // Trigger idle animation
	}
}

void PlayerExitIdle(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	(void)player;
	// printf("\n%s <- EXIT <- Idle\n", object->name);
	// printf("Stamina: %.1f, Mana: %.1f\n\n", player->stamina, player->mana);
	// Complete the remainder of the method
}

/**
 * InitWalkAnimation - Picks the right walk row based on direction.
 *
 * Uses currentDirection on the GameObject and points the animation
 * at the matching sprites in the sheet.
 */

static void InitWalkAnimation(GameObject *object)
{
	// WALK UP (Row 9, Pixel : Y = 8 x 64 = 512)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	static Rectangle walk_UP[9] = {
		{0, 512, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
		{64, 512, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
		{128, 512, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
		{192, 512, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
		{256, 512, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
		{320, 512, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
		{384, 512, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
		{448, 512, 64, 64},   // Frame 8: Column 8 (Pixel : X 448)
		{512, 512, 64, 64}    // Frame 9: Column 9 (Pixel : X 512)
	};

	// WALK DOWN (Row 11, Pixel : Y = 10 x 64 = 640)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	static Rectangle walk_DOWN[9] = {
		{0, 640, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
		{64, 640, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
		{128, 640, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
		{192, 640, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
		{256, 640, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
		{320, 640, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
		{384, 640, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
		{448, 640, 64, 64},   // Frame 8: Column 8 (Pixel : X 448)
		{512, 640, 64, 64}    // Frame 9: Column 9 (Pixel : X 512)
	};

	// WALK LEFT (Row 10, Pixel : Y = 9 x 64 = 576)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	static Rectangle walk_LEFT[9] = {
		{0, 576, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
		{64, 576, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
		{128, 576, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
		{192, 576, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
		{256, 576, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
		{320, 576, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
		{384, 576, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
		{448, 576, 64, 64},   // Frame 8: Column 8 (Pixel : X 448)
		{512, 576, 64, 64}    // Frame 9: Column 9 (Pixel : X 512)
	};

	// WALK RIGHT (Row 12, Pixel : Y = 11 x 64 = 704)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	static Rectangle walk_RIGHT[9] = {
		{0, 704, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
		{64, 704, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
		{128, 704, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
		{192, 704, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
		{256, 704, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
		{320, 704, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
		{384, 704, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
		{448, 704, 64, 64},   // Frame 8: Column 8 (Pixel : X 448)
		{512, 704, 64, 64}    // Frame 9: Column 9 (Pixel : X 512)
	};

	switch (object->currentDirection)
	{
	case UP:
		InitGameObjectAnimation(object, walk_UP, 9, 0.055f);
		break;
	case DOWN:
		InitGameObjectAnimation(object, walk_DOWN, 9, 0.055f);
		break;
	case LEFT:
		InitGameObjectAnimation(object, walk_LEFT, 9, 0.055f);
		break;
	case RIGHT:
		InitGameObjectAnimation(object, walk_RIGHT, 9, 0.055f);
		break;
	}
}

void PlayerEnterWalking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	printf("\n%s -> ENTER -> Walking\n", object->name);
	printf("Stamina: %.1f, Mana: %.1f\n\n", player->stamina, player->mana);
	// Complete the remainder of the method
	InitWalkAnimation(object);
}

void PlayerUpdateWalking(GameObject *object, float deltaTime)
{
	// Complete the remainder of the method

	// Move according to inputAxis
	GameObjectMove(object, object->inputAxis, deltaTime);

	// Check if moving and direction changed since last frame
	if (Vector2Length(object->inputAxis) > 0.0f &&
		object->currentDirection != object->previousDirection)
	{
		InitWalkAnimation(object);
		object->previousDirection = object->currentDirection;
	}

	// Update the Animation
	UpdateAnimation(&object->animation, deltaTime);
}

void PlayerExitWalking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	printf("\n%s <- EXIT <- Walking\n", object->name);
	printf("Stamina: %.1f, Mana: %.1f\n\n", player->stamina, player->mana);
	// Complete the remainder of the method
}

/**
 * InitAttackAnimation - Picks the correct attack arc for the direction.
 *
 * Uses the big 192x192 attack frames further down the sprite sheet.
 * The swipe plays once (it doesn't loop), so the state knows when it's done.
 */

static void InitAttackAnimation(GameObject *object)
{
	// ATTACK UP (Rows 47-49, Pixel : Y = 46 x 64 = 2944)
	// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
	static Rectangle sword_attack_UP[6] = {
		{0, 2944, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
		{192, 2944, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
		{384, 2944, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
		{576, 2944, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
		{768, 2944, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
		{960, 2944, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
	};

	// ATTACK LEFT (Rows 50-52, Pixel : Y = 49 x 64 = 3136)
	// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
	static Rectangle sword_attack_LEFT[6] = {
		{0, 3136, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
		{192, 3136, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
		{384, 3136, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
		{576, 3136, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
		{768, 3136, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
		{960, 3136, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
	};

	// ATTACK DOWN (Rows 53-55, Pixel : Y = 52 x 64 = 3328)
	// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
	static Rectangle sword_attack_DOWN[6] = {
		{0, 3328, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
		{192, 3328, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
		{384, 3328, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
		{576, 3328, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
		{768, 3328, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
		{960, 3328, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
	};

	// ATTACK RIGHT (Rows 56-58, Pixel : Y = 55 x 64 = 3520)
	// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
	static Rectangle sword_attack_RIGHT[6] = {
		{0, 3520, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
		{192, 3520, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
		{384, 3520, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
		{576, 3520, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
		{768, 3520, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
		{960, 3520, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
	};

	switch (object->currentDirection)
	{
	case UP:
		InitGameObjectAnimationOnce(object, sword_attack_UP, 6, 0.035f);
		break;
	case DOWN:
		InitGameObjectAnimationOnce(object, sword_attack_DOWN, 6, 0.035f);
		break;
	case LEFT:
		InitGameObjectAnimationOnce(object, sword_attack_LEFT, 6, 0.035f);
		break;
	case RIGHT:
		InitGameObjectAnimationOnce(object, sword_attack_RIGHT, 6, 0.035f);
		break;
	}
}

void PlayerEnterAttacking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	printf("\n%s -> ENTER -> Attacking\n", object->name);
	printf("Stamina: %.1f, Mana: %.1f\n\n", player->stamina, player->mana);
	// Complete the remainder of the method
	// Example: Deduct some stamina when attacking

	// A fresh swipe hasn't landed yet (see AttackHit in collision.c)
	object->attackHit = false;

	// One swipe, facing the way the player was going
	InitAttackAnimation(object);

	// Entry runs once per swipe, so this is the place for the swoosh
	PlayActionSound(SOUND_SWIPE);
}

void PlayerUpdateAttacking(GameObject *object, float deltaTime)
{
	// Complete the remainder of the method
	// Check if the attack should end or be interrupted (e.g., stamina depletion)

	// No movement while swinging: the swipe frames have no walking legs,
	// so moving here looks like sliding on ice. Feet planted, sword out.

	// Update the Animation
	UpdateAnimation(&object->animation, deltaTime);

	// Swipe finished? Tell the FSM, and the transition table decides what's next
	if (object->animation.finished)
	{
		HandleEvent(object, EVENT_ANIMATION_END, deltaTime);
	}
}

void PlayerExitAttacking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	printf("\n%s <- EXIT <- Attacking\n", object->name);
	printf("Stamina: %.1f, Mana: %.1f\n\n", player->stamina, player->mana);
	// Complete the remainder of the method
	// Reset or adjust any temporary changes during attack, if needed
}

void PlayerEnterShielding(GameObject *object, float deltaTime)
{
	// TODO : Not Currently Implemented
	(void)deltaTime;
	Player *player = (Player *)object;
	printf("\n%s -> ENTER -> Sheilding\n", object->name);
	printf("Stamina: %.1f, Mana: %.1f\n\n", player->stamina, player->mana);
	// Complete the remainder of the method
	// Example: Deduct some stamina for shielding
}
void PlayerUpdateShielding(GameObject *object, float deltaTime)
{
	// Complete the remainder of the method
	// Example: Check if the shielding duration is over or if stamina is depleted
	UpdateAnimation(&object->animation, deltaTime);
}

void PlayerExitShielding(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	printf("\n%s <- EXIT <- Sheilding\n", object->name);
	printf("Stamina: %.1f, Mana: %.1f\n\n", player->stamina, player->mana);
	// Complete the remainder of the method
	// Reset any temporary shielding effects if necessary
}

void PlayerEnterDie(GameObject *object, float deltaTime)
{
	// TODO : Not Currently Implemented
	(void)deltaTime;
	printf("\n%s -> ENTER -> Die\n", object->name);
	// Complete the remainder of the method
}

void PlayerUpdateDie(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	ChangeState(object, STATE_RESPAWN, deltaTime);
	// Complete the remainder of the method
	UpdateAnimation(&object->animation, deltaTime);
}

void PlayerExitDie(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	printf("\n%s <- EXIT <- Die\n", object->name);
	// Complete the remainder of the method
}

void PlayerEnterRespawn(GameObject *object, float deltaTime)
{
	// TODO : Not Currently Implemented
	(void)deltaTime;
	printf("\n%s -> ENTER -> Respawn\n", object->name);
	// Complete the remainder of the method
}

void PlayerUpdateRespawn(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	ChangeState(object, STATE_IDLE, deltaTime);
	// Complete the remainder of the method
	UpdateAnimation(&object->animation, deltaTime);
}

void PlayerExitRespawn(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	printf("\n%s <- EXIT <- Respawn\n", object->name);
	// Complete the remainder of the method
}