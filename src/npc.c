#include "gameobjects/npc.h"
#include "utils/utils.h"

/*
 * Sprite sheet layout (resources/npc_sprite_sheet.png)
 *
 * Grid is 64 x 64: Row n starts at Pixel Y = (n - 1) x 64, Column c at Pixel X = (c - 1) x 64
 * Sword swipes are 192 x 192 (3 x 3 grid cells) at the bottom of the sheet, Rows 47-58:
 *     Pixel Y = (first row - 1) x 64, Pixel X = (Column - 1) x 192
 *
 * Each Rectangle below is {Pixel X, Pixel Y, width, height}
 */

/**
 * InitNPC - Sets up a brand-new NPC and gets them ready for mischief.
 *
 * @name:     The NPC's name ... purely for debugging and adds a bit of personality.
 * @position: Where the NPC spawns into the world.
 * @radius:   Collider radius (because even NPCs need personal space).
 *
 * Allocates the NPC, loads its sprite sheet, builds its collider,
 * sets aggression, health, and wires up the NPC-specific FSM.
 *
 * Returns: Pointer to the freshly-minted NPC.
 *          Exits the program if memory/texture loading fails.
 */

NPC *InitNPC(const char *name, Vector2 position, int radius)
{
	// Allocate memory for the NPC structure
	NPC *npc = (NPC *)malloc(sizeof(NPC));

	// Check if memory allocation failed
	if (!npc)
	{
		// Print an error message to stderr and terminate the program if allocation fails
		fprintf(stderr, "Failed to allocate NPC\n");
		exit(1);
	}

	// Load player texture
	Texture2D npcTexture = LoadTexture("resources/npc_sprite_sheet.png");

	// Setup Collider
	c2Circle collider = (c2Circle){.p = {position.x, position.y}, .r = radius};

	// Initialize the base GameObject structure within the NPC with the provided name
	InitGameObject(&npc->base,
				   name,
				   position,   // Position
				   STATE_IDLE, // Initial State
				   DARKGREEN,  // Player Color
				   collider,   // cute_c2 Circle Collider
				   npcTexture,
				   100 // Initial Health
	);

	// Set the default aggression level for the NPC
	npc->aggression = NPC_MIN_AGGRESSION; // Mild agression

	// Nothing to chase yet: game.c sets the target to the player while attacking
	npc->target = position;

	// Initialize the NPC's finite state machine (FSM) with state configurations
	InitNPCFSM(&npc->base);

	if (npc->base.currentState == STATE_IDLE)
	{
		// Initialize the idle animation immediately
		NPCEnterIdle(&npc->base, 0.0f); // Trigger idle animation at initialization
	}

	// Return a pointer to the initialized NPC object
	return npc;
}

/**
 * DeleteNPC - Sends the NPC off to the big heap in the sky (Fly NPC Fly!).
 *
 * @object: The NPC's GameObject.
 *
 * Handles any NPC-specific teardown (none yet), then hands off to
 * DeleteGameObject for the real cleanup.
 */

void DeleteNPC(GameObject *object)
{
	// Perform any npc-specific cleanup here
	// Cast to NPC if npc-specific cleanup is required
	// NPC *npc = (NPC *)object;
	// Example of potential cleanup (not implemented here):
	// If the npc is holding a dynamically allocated object, such as a thor hammer:
	// free(npc->holding);
	DeleteGameObject(object);
}

/**
 * InitNPCFSM - Wires up the finite-state machine for an NPC.
 *
 * @object: The NPC's GameObject.
 *
 * Allocates the state configuration table, assigns handler
 * functions for each implemented state, and defines each state's
 * legal transitions. Unused states are filled with empty configs
 * so they don't cause chaos.
 */

void InitNPCFSM(GameObject *object)
{
	// Allocate memory for the state configurations array with a size for all possible states
	object->stateConfigs = AllocateStateConfigs(STATE_COUNT);

	// ---- STATE_IDLE state configuration ----
	// Define valid transitions from STATE_IDLE
	EventStateTransition idleValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_ATTACK, STATE_ATTACKING},
		{EVENT_DEFEND, STATE_SHIELD},
		{EVENT_DIE, STATE_DEAD}};
	// Set up the state configuration for STATE_IDLE
	InitStateConfig(object, STATE_IDLE, "NPC_Idle", NPCEnterIdle, NPCUpdateIdle, NPCExitIdle);
	// Configure valid transitions for STATE_IDLE
	StateTransitions(&object->stateConfigs[STATE_IDLE], idleValidTransitions, sizeof(idleValidTransitions) / sizeof(EventStateTransition));

	// ---- STATE_ATTACKING state configuration ----
	// Define valid transitions from STATE_ATTACKING
	EventStateTransition attackValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_NONE, STATE_IDLE},
		{EVENT_DEFEND, STATE_SHIELD},
		{EVENT_DIE, STATE_DEAD}};
	// Set up the state configuration for STATE_ATTACKING
	InitStateConfig(object, STATE_ATTACKING, "NPC_Attacking", NPCEnterAttacking, NPCUpdateAttacking, NPCExitAttacking);
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
	InitStateConfig(object, STATE_SHIELD, "NPC_Shielding", NPCEnterShielding, NPCUpdateShielding, NPCExitShielding);
	// Configure valid transitions for STATE_SHIELD
	StateTransitions(&object->stateConfigs[STATE_SHIELD], sheildingValidTransitions, sizeof(sheildingValidTransitions) / sizeof(EventStateTransition));

	// ---- STATE_DEAD state configuration ----
	// Define valid transitions from STATE_DEAD
	EventStateTransition deadValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_RESPAWN, STATE_IDLE}}; // Should go to STATE_RESPAWN to keep kit small goes to IDLE
	// Set up the state configuration for STATE_DEAD
	InitStateConfig(object, STATE_DEAD, "NPC_Dead", NPCEnterDead, NPCUpdateDead, NPCExitDead);
	// Configure valid transitions for STATE_DEAD
	StateTransitions(&object->stateConfigs[STATE_DEAD], deadValidTransitions, sizeof(deadValidTransitions) / sizeof(EventStateTransition));

	// Print out Configs
	PrintStateConfigs(object->stateConfigs, STATE_COUNT);
}

// Enter function for Idle state, executed once upon entering Idle
void NPCEnterIdle(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	// printf("%s -> ENTER -> Idle\n", object->name);
	// printf("Aggression: %d\n\n", npc->aggression);
	// Initialization code for entering Idle state, such as resetting timers or animation.

	if (npc->base.previousState != npc->base.currentState && npc->base.currentState == STATE_IDLE)
	{
		// Setup Idle Animations
		// IDLE (Row 3, Pixel : Y = 2 x 64 = 128)
		// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
		Rectangle idle[7] = {
			{0, 128, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
			{64, 128, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
			{128, 128, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
			{192, 128, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
			{256, 128, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
			{320, 128, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
			{384, 128, 64, 64}    // Frame 7: Column 7 (Pixel : X 384)
		};

		// Initialize the idle animation frames and play it
		InitGameObjectAnimation(&npc->base, idle, 7, 0.1f);
	}
}

// Update function for Idle state, called repeatedly during game ticks while in Idle
void NPCUpdateIdle(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	(void)npc;
	// During game loop and game ticks, execute Idle state behavior here, such as patrolling or observing.
	UpdateAnimation(&object->animation, deltaTime);
}

// Exit function for Idle state, executed once upon leaving Idle
void NPCExitIdle(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	(void)npc;
	// printf("%s <- EXIT <- Idle\n", object->name);
	// printf("Aggression: %d\n\n", npc->aggression);
	// Cleanup code for leaving Idle state, if any.
}

/**
 * InitAttackAnimation - Picks the correct attack arc for the direction.
 *
 * Uses the big 192x192 attack frames further down the sprite sheet.
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

// Enter function for Attacking state, executed once upon entering Attacking
void NPCEnterAttacking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	printf("%s -> ENTER -> Attacking\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
	// Initialization code for entering Attacking state, such as setting up attack animations.
	InitAttackAnimation(object);
}

// Update function for Attacking state, called repeatedly during game ticks while in Attacking
void NPCUpdateAttacking(GameObject *object, float deltaTime)
{
	// Cast GameObject to NPC to access NPC-specific data
	NPC *npc = (NPC *)object;


	// Direction from NPC to target (player)
	Vector2 direction;
	direction.x = npc->target.x - npc->base.position.x;
	direction.y = npc->target.y - npc->base.position.y;

	// Normalise direction using Raylib function (0,0) to (1,1)
	direction = Vector2Normalize(direction);

	// Calculate distance to target using Raylib function ( 0 to distance n)
	float distance = Vector2Distance(npc->base.position, npc->target);

	// Store previous direction (see utils/utils.h)
	object->previousDirection = object->currentDirection;

	// Update current direction based on movement direction
	// (see utils/utils.h)
	object->currentDirection = DirectionAxis(direction);

	// If Direction changed change attack animation
	if (object->currentDirection != object->previousDirection)
	{
		InitAttackAnimation(object);
		object->previousDirection = object->currentDirection;
	}

	// Move towards target
	// Use aggression to affect movement speed example below
	// aggression already runs from NPC_MIN_AGGRESSION to NPC_MAX_AGGRESSION (see game.c),
	// so it's used directly as a speed factor: 0.3 = 30% speed, 0.8 = 80% speed

	// Make sure aggression factor is clamped between min and max agression
	float aggression_factor = Clamp(npc->aggression, NPC_MIN_AGGRESSION, NPC_MAX_AGGRESSION);

	float movement = DEFAULT_MOVEMENT_SPEED * aggression_factor * deltaTime;

	// Dont go past target
	if (movement > distance)
		movement = distance;

	// Scale direction by movement size
	Vector2 delta = (Vector2){direction.x * movement, direction.y * movement};

	// Apply movement
	npc->base.position = Vector2Add(npc->base.position, delta);

	// Keep NPC on screen
	ClampGameObjectOnScreen(&npc->base);

	// Update collider position
	npc->base.collider.p.x = npc->base.position.x;
	npc->base.collider.p.y = npc->base.position.y;

	// Update attack animation
	UpdateAnimation(&object->animation, deltaTime);
}

// Exit function for Attacking state, executed once upon leaving Attacking
void NPCExitAttacking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	printf("%s <- EXIT <- Attacking\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
	// Cleanup code for leaving Attacking state, such as resetting attack cooldown.
}

// Enter function for Shielding state, executed once upon entering Shielding
void NPCEnterShielding(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	printf("%s -> ENTER -> Shielding\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
	// Initialization code for entering Shielding state, such as enabling shield effects.

	// SHIELDING (Row 7, Pixel : Y = 6 x 64 = 384)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle sheilding[8] = {
		{0, 384, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
		{64, 384, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
		{128, 384, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
		{192, 384, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
		{256, 384, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
		{320, 384, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
		{384, 384, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
		{448, 384, 64, 64}    // Frame 8: Column 8 (Pixel : X 448)
	};

	// Initialize shielding animation
	InitGameObjectAnimation(&npc->base, sheilding, 8, 0.08f);
}

// Update function for Shielding state, called repeatedly during game ticks while in Shielding
void NPCUpdateShielding(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	// During game loop and game ticks, execute Shielding state behavior here, such as reducing incoming damage.
	UpdateAnimation(&object->animation, deltaTime);
}

// Exit function for Shielding state, executed once upon leaving Shielding
void NPCExitShielding(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	printf("%s -> EXIT -> Shielding\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
	// Cleanup code for leaving Shielding state, if any.
}

// Enter function for Dead state, executed once upon entering Dead
void NPCEnterDead(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	printf("%s -> ENTER -> Dead\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
	// Initialization code for entering Dead state, such as playing death animation or disabling further actions.
	// DEAD (Row 21, Pixel : Y = 20 x 64 = 1280)
	// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
	Rectangle dead[6] = {
		{0, 1280, 64, 64},    // Frame 1: Column 1 (Pixel : X 0)
		{64, 1280, 64, 64},   // Frame 2: Column 2 (Pixel : X 64)
		{128, 1280, 64, 64},  // Frame 3: Column 3 (Pixel : X 128)
		{192, 1280, 64, 64},  // Frame 4: Column 4 (Pixel : X 192)
		{256, 1280, 64, 64},  // Frame 5: Column 5 (Pixel : X 256)
		{320, 1280, 64, 64}   // Frame 6: Column 6 (Pixel : X 320)
	};
	// Initialize dead animation
	InitGameObjectAnimation(&npc->base, dead, 6, 0.2f);
}

// Update function for Dead state, called repeatedly during game ticks while in Dead
void NPCUpdateDead(GameObject *object, float deltaTime)
{
	// During game loop and game ticks, execute Dead state behavior here, such as preventing any actions.
	// This could be a place to check if the NPC should be removed or respawned.
	UpdateAnimation(&object->animation, deltaTime);
}

// Exit function for Dead state, executed once upon leaving Dead
void NPCExitDead(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	printf("%s -> EXIT -> Dead\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
	// Cleanup code for leaving Dead state, such as removing NPC from the active world, playing respawn animations, etc.
}
