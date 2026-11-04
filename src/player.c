#include "gameobjects/player.h"
#include "utils/utils.h"

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
	EventStateTransition attackValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_NONE, STATE_IDLE},
		{EVENT_MOVE, STATE_WALKING},
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

	Rectangle idle1[8] = {
		{0, 320, 64, 64},	// Frame 1: Row 6, Column 1
		{64, 320, 64, 64},	// Frame 2: Row 6, Column 2
		{128, 320, 64, 64}, // Frame 3: Row 6, Column 3
		{192, 320, 64, 64}, // Frame 4: Row 6, Column 4
		{256, 320, 64, 64}, // Frame 5: Row 6, Column 5
		{320, 320, 64, 64}, // Frame 6: Row 6, Column 6
		{384, 320, 64, 64}, // Frame 7: Row 6, Column 7
		{448, 320, 64, 64}	// Frame 8: Row 6, Column 8
	};

	Rectangle idle2[8] = {
		{0, 384, 64, 64},	// Frame 1: Row 7, Column 1
		{64, 384, 64, 64},	// Frame 2: Row 7, Column 2
		{128, 384, 64, 64}, // Frame 3: Row 7, Column 3
		{192, 384, 64, 64}, // Frame 4: Row 7, Column 4
		{256, 384, 64, 64}, // Frame 5: Row 7, Column 5
		{320, 384, 64, 64}, // Frame 6: Row 7, Column 6
		{384, 384, 64, 64}, // Frame 7: Row 7, Column 7
		{448, 384, 64, 64}	// Frame 8: Row 7, Column 8
	};

	Rectangle idle3[8] = {
		{0, 448, 64, 64},	// Frame 1: Row 8, Column 1
		{64, 448, 64, 64},	// Frame 2: Row 8, Column 2
		{128, 448, 64, 64}, // Frame 3: Row 8, Column 3
		{192, 448, 64, 64}, // Frame 4: Row 8, Column 4
		{256, 448, 64, 64}, // Frame 5: Row 8, Column 5
		{320, 448, 64, 64}, // Frame 6: Row 8, Column 6
		{384, 448, 64, 64}, // Frame 7: Row 8, Column 7
		{448, 448, 64, 64}	// Frame 8: Row 8, Column 8
	};

	Rectangle idle4[13] = {
		{0, 1024, 64, 64},	 // Frame 1: Row 17, Column 1
		{64, 1024, 64, 64},	 // Frame 2: Row 17, Column 2
		{128, 1024, 64, 64}, // Frame 3: Row 17, Column 3
		{192, 1024, 64, 64}, // Frame 4: Row 17, Column 4
		{256, 1024, 64, 64}, // Frame 5: Row 17, Column 5
		{320, 1024, 64, 64}, // Frame 6: Row 17, Column 6
		{384, 1024, 64, 64}, // Frame 7: Row 17, Column 7
		{448, 1024, 64, 64}, // Frame 8: Row 17, Column 8
		{512, 1024, 64, 64}, // Frame 9: Row 17, Column 9
		{576, 1024, 64, 64}, // Frame 10: Row 17, Column 10
		{640, 1024, 64, 64}, // Frame 11: Row 17, Column 11
		{704, 1024, 64, 64}, // Frame 12: Row 17, Column 12
		{768, 1024, 64, 64}	 // Frame 13: Row 17, Column 13
	};

	Rectangle idle5[13] = {
		{0, 1088, 64, 64},	 // Frame 1: Row 18, Column 1
		{64, 1088, 64, 64},	 // Frame 2: Row 18, Column 2
		{128, 1088, 64, 64}, // Frame 3: Row 18, Column 3
		{192, 1088, 64, 64}, // Frame 4: Row 18, Column 4
		{256, 1088, 64, 64}, // Frame 5: Row 18, Column 5
		{320, 1088, 64, 64}, // Frame 6: Row 18, Column 6
		{384, 1088, 64, 64}, // Frame 7: Row 18, Column 7
		{448, 1088, 64, 64}, // Frame 8: Row 18, Column 8
		{512, 1088, 64, 64}, // Frame 9: Row 18, Column 9
		{576, 1088, 64, 64}, // Frame 10: Row 18, Column 10
		{640, 1088, 64, 64}, // Frame 11: Row 18, Column 11
		{704, 1088, 64, 64}, // Frame 12: Row 18, Column 12
		{768, 1088, 64, 64}	 // Frame 13: Row 18, Column 13
	};

	Rectangle idle6[13] = {
		{0, 1152, 64, 64},	 // Frame 1: Row 19, Column 1
		{64, 1152, 64, 64},	 // Frame 2: Row 19, Column 2
		{128, 1152, 64, 64}, // Frame 3: Row 19, Column 3
		{192, 1152, 64, 64}, // Frame 4: Row 19, Column 4
		{256, 1152, 64, 64}, // Frame 5: Row 19, Column 5
		{320, 1152, 64, 64}, // Frame 6: Row 19, Column 6
		{384, 1152, 64, 64}, // Frame 7: Row 19, Column 7
		{448, 1152, 64, 64}, // Frame 8: Row 19, Column 8
		{512, 1152, 64, 64}, // Frame 9: Row 19, Column 9
		{576, 1152, 64, 64}, // Frame 10: Row 19, Column 10
		{640, 1152, 64, 64}, // Frame 11: Row 19, Column 11
		{704, 1152, 64, 64}, // Frame 12: Row 19, Column 12
		{768, 1152, 64, 64}	 // Frame 13: Row 19, Column 13
	};

	Rectangle idle7[13] = {
		{0, 1216, 64, 64},	 // Frame 1: Row 20, Column 1
		{64, 1216, 64, 64},	 // Frame 2: Row 20, Column 2
		{128, 1216, 64, 64}, // Frame 3: Row 20, Column 3
		{192, 1216, 64, 64}, // Frame 4: Row 20, Column 4
		{256, 1216, 64, 64}, // Frame 5: Row 20, Column 5
		{320, 1216, 64, 64}, // Frame 6: Row 20, Column 6
		{384, 1216, 64, 64}, // Frame 7: Row 20, Column 7
		{448, 1216, 64, 64}, // Frame 8: Row 20, Column 8
		{512, 1216, 64, 64}, // Frame 9: Row 20, Column 9
		{576, 1216, 64, 64}, // Frame 10: Row 20, Column 10
		{640, 1216, 64, 64}, // Frame 11: Row 20, Column 11
		{704, 1216, 64, 64}, // Frame 12: Row 20, Column 12
		{768, 1216, 64, 64}	 // Frame 13: Row 20, Column 13
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
	// WALK UP (Row 9)
	static Rectangle walk_UP[9] = {
		{0, 512, 64, 64},	// Frame 1: Row 9, Column 1
		{64, 512, 64, 64},	// Frame 2: Row 9, Column 2
		{128, 512, 64, 64}, // Frame 3: Row 9, Column 3
		{192, 512, 64, 64}, // Frame 4: Row 9, Column 4
		{256, 512, 64, 64}, // Frame 5: Row 9, Column 5
		{320, 512, 64, 64}, // Frame 6: Row 9, Column 6
		{384, 512, 64, 64}, // Frame 7: Row 9, Column 7
		{448, 512, 64, 64}, // Frame 8: Row 9, Column 8
		{512, 512, 64, 64}	// Frame 9: Row 9, Column 9
	};

	// WALK DOWN (Row 10)
	static Rectangle walk_DOWN[9] = {
		{0, 640, 64, 64},	// Frame 1: Row 10, Column 1
		{64, 640, 64, 64},	// Frame 2: Row 10, Column 2
		{128, 640, 64, 64}, // Frame 3: Row 10, Column 3
		{192, 640, 64, 64}, // Frame 4: Row 10, Column 4
		{256, 640, 64, 64}, // Frame 5: Row 10, Column 5
		{320, 640, 64, 64}, // Frame 6: Row 10, Column 6
		{384, 640, 64, 64}, // Frame 7: Row 10, Column 7
		{448, 640, 64, 64}, // Frame 8: Row 10, Column 8
		{512, 640, 64, 64}	// Frame 9: Row 10, Column 9
	};

	// WALK LEFT (Row 11)
	static Rectangle walk_LEFT[9] = {
		{0, 576, 64, 64},	// Frame 1: Row 11, Column 1
		{64, 576, 64, 64},	// Frame 2: Row 11, Column 2
		{128, 576, 64, 64}, // Frame 3: Row 11, Column 3
		{192, 576, 64, 64}, // Frame 4: Row 11, Column 4
		{256, 576, 64, 64}, // Frame 5: Row 11, Column 5
		{320, 576, 64, 64}, // Frame 6: Row 11, Column 6
		{384, 576, 64, 64}, // Frame 7: Row 11, Column 7
		{448, 576, 64, 64}, // Frame 8: Row 11, Column 8
		{512, 576, 64, 64}	// Frame 9: Row 11, Column 9
	};

	// WALK RIGHT (Row 12)
	static Rectangle walk_RIGHT[9] = {
		{0, 704, 64, 64},	// Frame 1: Row 12, Column 1
		{64, 704, 64, 64},	// Frame 2: Row 12, Column 2
		{128, 704, 64, 64}, // Frame 3: Row 12, Column 3
		{192, 704, 64, 64}, // Frame 4: Row 12, Column 4
		{256, 704, 64, 64}, // Frame 5: Row 12, Column 5
		{320, 704, 64, 64}, // Frame 6: Row 12, Column 6
		{384, 704, 64, 64}, // Frame 7: Row 12, Column 7
		{448, 704, 64, 64}, // Frame 8: Row 12, Column 8
		{512, 704, 64, 64}	// Frame 9: Row 12, Column 9
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
 */

static void InitAttackAnimation(GameObject *object)
{
	// ATTACK UP (Row 48)
	static Rectangle sword_attack_UP[6] = {
		{0, 2952, 192, 192},   // Frame 1: Row 44, Column 1
		{192, 2952, 192, 192}, // Frame 2: Row 44, Column 2
		{384, 2952, 192, 192}, // Frame 3: Row 44, Column 3
		{576, 2952, 192, 192}, // Frame 4: Row 44, Column 4
		{768, 2952, 192, 192}, // Frame 5: Row 44, Column 5
		{960, 2952, 192, 192}  // Frame 6: Row 44, Column 6
	};

	// ATTACK LEFT (Row 51)
	static Rectangle sword_attack_LEFT[6] = {
		{0, 3144, 192, 192},   // Frame 1: Row 51, Column 1
		{192, 3144, 192, 192}, // Frame 2: Row 51, Column 2
		{384, 3144, 192, 192}, // Frame 3: Row 51, Column 3
		{576, 3144, 192, 192}, // Frame 4: Row 51, Column 4
		{768, 3144, 192, 192}, // Frame 5: Row 51, Column 5
		{960, 3144, 192, 192}  // Frame 6: Row 51, Column 6
	};

	// ATTACK DOWN (Row 54)
	static Rectangle sword_attack_DOWN[6] = {
		{0, 3336, 192, 192},   // Frame 1: Row 54, Column 1
		{192, 3336, 192, 192}, // Frame 2: Row 54, Column 2
		{384, 3336, 192, 192}, // Frame 3: Row 54, Column 3
		{576, 3336, 192, 192}, // Frame 4: Row 54, Column 4
		{768, 3336, 192, 192}, // Frame 5: Row 54, Column 5
		{960, 3336, 192, 192}  // Frame 6: Row 54, Column 6
	};

	// ATTACK RIGHT (Row 57)
	static Rectangle sword_attack_RIGHT[6] = {
		{0, 3528, 192, 192},   // Frame 1: Row 57, Column 1
		{192, 3528, 192, 192}, // Frame 2: Row 57, Column 2
		{384, 3528, 192, 192}, // Frame 3: Row 57, Column 3
		{576, 3528, 192, 192}, // Frame 4: Row 57, Column 4
		{768, 3528, 192, 192}, // Frame 5: Row 57, Column 5
		{960, 3528, 192, 192}  // Frame 6: Row 57, Column 6
	};

	switch (object->currentDirection)
	{
	case UP:
		InitGameObjectAnimation(object, sword_attack_UP, 6, 0.035f);
		break;
	case DOWN:
		InitGameObjectAnimation(object, sword_attack_DOWN, 6, 0.035f);
		break;
	case LEFT:
		InitGameObjectAnimation(object, sword_attack_LEFT, 6, 0.035f);
		break;
	case RIGHT:
		InitGameObjectAnimation(object, sword_attack_RIGHT, 6, 0.035f);
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
	InitAttackAnimation(object);
}

void PlayerUpdateAttacking(GameObject *object, float deltaTime)
{
	// Complete the remainder of the method
	// Check if the attack should end or be interrupted (e.g., stamina depletion)

	// Move according to inputAxis
	GameObjectMove(object, object->inputAxis, deltaTime);

	// Check if moving and direction changed since last frame
	if (Vector2Length(object->inputAxis) > 0.0f &&
		object->currentDirection != object->previousDirection)
	{
		InitAttackAnimation(object);
		object->previousDirection = object->currentDirection;
	}

	// Update the Animation
	UpdateAnimation(&object->animation, deltaTime);
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