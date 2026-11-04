#include "./gameobjects/npc.h"
#include "./utils/utils.h"

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
	Texture2D npcTexture = LoadTexture("./assets/npc_sprite_sheet.png");

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
		Rectangle idle[7] = {
			{0, 128, 64, 64},	// Frame 1: Row 3, Column 1
			{64, 128, 64, 64},	// Frame 2: Row 3, Column 2
			{128, 128, 64, 64}, // Frame 3: Row 3, Column 3
			{192, 128, 64, 64}, // Frame 4: Row 3, Column 4
			{256, 128, 64, 64}, // Frame 5: Row 3, Column 5
			{320, 128, 64, 64}, // Frame 6: Row 3, Column 6
			{384, 128, 64, 64}	// Frame 7: Row 3, Column 7
		};

		// Initialize the idle animation frames and play it
		InitGameObjectAnimation(&npc->base, idle, 6, 0.1f);
	}
}

// Update function for Idle state, called repeatedly during game ticks while in Idle
void NPCUpdateIdle(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	(void)npc;
	// printf("%s -> UPDATE -> Idle\n", object->name);
	// printf("Aggression: %d\n\n", npc->aggression);
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

	printf("%s -> UPDATE -> Attacking\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);

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
	// Aggression factor scales between min and max based on npc->aggression (min to max)
	float aggression_factor = NPC_MIN_AGGRESSION + (NPC_MAX_AGGRESSION - NPC_MIN_AGGRESSION) * npc->aggression;

	// Make sure aggression factor is clamped between min and max agression
	aggression_factor = Clamp(aggression_factor, NPC_MIN_AGGRESSION, NPC_MAX_AGGRESSION);

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
	UpdateAnimation(&object->animation, deltaTime);
}

// Enter function for Shielding state, executed once upon entering Shielding
void NPCEnterShielding(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	printf("%s -> ENTER -> Shielding\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
	// Initialization code for entering Shielding state, such as enabling shield effects.

	Rectangle sheilding[8] = {
		{0, 384, 64, 64},	// Frame 1: Row 7, Column 1
		{64, 384, 64, 64},	// Frame 2: Row 7, Column 2
		{128, 384, 64, 64}, // Frame 3: Row 7, Column 3
		{192, 384, 64, 64}, // Frame 4: Row 7, Column 4
		{256, 384, 64, 64}, // Frame 5: Row 7, Column 5
		{320, 384, 64, 64}, // Frame 6: Row 7, Column 6
		{384, 384, 64, 64}, // Frame 7: Row 7, Column 7
		{448, 384, 64, 64}	// Frame 8: Row 7, Column 8
	};

	// Initialize attack animation
	InitGameObjectAnimation(&npc->base, sheilding, 6, 0.08f);
}

// Update function for Shielding state, called repeatedly during game ticks while in Shielding
void NPCUpdateShielding(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	NPC *npc = (NPC *)object;
	printf("%s -> UPDATE -> Shielding\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
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
	Rectangle dead[6] = {
		{0, 1280, 64, 64},	 // Frame 1: Row 21, Column 1
		{64, 1280, 64, 64},	 // Frame 1: Row 21, Column 2
		{128, 1280, 64, 64}, // Frame 1: Row 21, Column 3
		{192, 1280, 64, 64}, // Frame 1: Row 21, Column 4
		{256, 1280, 64, 64}, // Frame 1: Row 21, Column 5
		{320, 1280, 64, 64}	 // Frame 1: Row 21, Column 6
	};
	// Initialize dead animation
	InitGameObjectAnimation(&npc->base, dead, 6, 0.2f);
}

// Update function for Dead state, called repeatedly during game ticks while in Dead
void NPCUpdateDead(GameObject *object, float deltaTime)
{
	NPC *npc = (NPC *)object;
	printf("%s -> UPDATE -> Dead\n", object->name);
	printf("Aggression: %f\n\n", npc->aggression);
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
