#include "gameobjects/player.h"
#include "utils/utils.h"
#include "utils/mediator.h"
#include "animation/sprites.h"

// Sprite sheet layout: see include/animation/sprites.h

// ==========================================================================
// SETUP AND CLEANUP
// ==========================================================================

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
		TraceLog(LOG_ERROR, "Failed to allocate memory for Player structure");
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

	TraceLog(LOG_INFO, "[%s] starts in %s", player->base.name, StateName(player->base.currentState));

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

// ==========================================================================
// STATE MACHINE: which event takes the player to which state
// ==========================================================================

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
	// One config per state (states not set up below stay "unimplemented")
	object->stateConfigs = AllocateStateConfigs(STATE_COUNT);

	// Each state below is three steps:
	//   1. its transition table: which EVENT takes it to which STATE
	//   2. InitStateConfig(): its name and its Entry, Update and Exit functions
	//   3. StateTransitions(): hand it the table

	// ---- STATE_IDLE ----
	EventStateTransition idleValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_MOVE, STATE_WALKING},
		{EVENT_ATTACK, STATE_ATTACKING},
		{EVENT_DEFEND, STATE_SHIELD},
		{EVENT_DIE, STATE_DEAD}};
	InitStateConfig(object, STATE_IDLE, "Player_Idle", PlayerEnterIdle, PlayerUpdateIdle, PlayerExitIdle);
	StateTransitions(&object->stateConfigs[STATE_IDLE], idleValidTransitions, ARRAY_COUNT(idleValidTransitions));

	// ---- STATE_WALKING ----
	EventStateTransition walkingValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_NONE, STATE_IDLE},
		{EVENT_ATTACK, STATE_ATTACKING},
		{EVENT_DIE, STATE_DEAD}};
	InitStateConfig(object, STATE_WALKING, "Player_Walking", PlayerEnterWalking, PlayerUpdateWalking, PlayerExitWalking);
	StateTransitions(&object->stateConfigs[STATE_WALKING], walkingValidTransitions, ARRAY_COUNT(walkingValidTransitions));

	// ---- STATE_ATTACKING ----
	// A swipe always finishes: letting go of SPACE or pressing a direction doesn't cut it short.
	// When it ends the player goes Idle, and from there straight back to Walking
	// (direction held) or into another swipe (SPACE held) on the next frame.
	EventStateTransition attackValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_ANIMATION_END, STATE_IDLE},
		{EVENT_DIE, STATE_DEAD}};
	InitStateConfig(object, STATE_ATTACKING, "Player_Attacking", PlayerEnterAttacking, PlayerUpdateAttacking, PlayerExitAttacking);
	StateTransitions(&object->stateConfigs[STATE_ATTACKING], attackValidTransitions, ARRAY_COUNT(attackValidTransitions));

	// ---- STATE_SHIELD ----
	EventStateTransition shieldingValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_NONE, STATE_IDLE},
		{EVENT_ATTACK, STATE_ATTACKING},
		{EVENT_DIE, STATE_DEAD}};
	InitStateConfig(object, STATE_SHIELD, "Player_Shielding", PlayerEnterShielding, PlayerUpdateShielding, PlayerExitShielding);
	StateTransitions(&object->stateConfigs[STATE_SHIELD], shieldingValidTransitions, ARRAY_COUNT(shieldingValidTransitions));

	// ---- STATE_DEAD ----
	EventStateTransition deadValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_RESPAWN, STATE_RESPAWN}};
	InitStateConfig(object, STATE_DEAD, "Player_Dead", PlayerEnterDead, PlayerUpdateDead, PlayerExitDead);
	StateTransitions(&object->stateConfigs[STATE_DEAD], deadValidTransitions, ARRAY_COUNT(deadValidTransitions));

	// ---- STATE_RESPAWN ----
	EventStateTransition respawnValidTransitions[] = {
		// EVENT -> STATE
		{EVENT_RESPAWN, STATE_IDLE}};
	InitStateConfig(object, STATE_RESPAWN, "Player_Respawn", PlayerEnterRespawn, PlayerUpdateRespawn, PlayerExitRespawn);
	StateTransitions(&object->stateConfigs[STATE_RESPAWN], respawnValidTransitions, ARRAY_COUNT(respawnValidTransitions));

	// Print out Configs
	PrintStateConfigs(object->stateConfigs, STATE_COUNT);
}

// ==========================================================================
// IDLE
// ==========================================================================

/**
 * SelectRandomIdleAnimation - Picks a random idle from the sheet.
 *
 * Just to keep the lad from looking like a statue. The player sheet has
 * seven idle rows: some 8 frames long, some 13. One is picked at random
 * each time the last one finishes.
 */

void SelectRandomIdleAnimation(GameObject *object, float deltaTime)
{
	(void)deltaTime;

	// The idle rows on the sheet (see grid_player_sprite_sheet.png)
	static const struct
	{
		int row;   // Row on the 64 x 64 grid
		int count; // Frames along it
	} IDLES[] = {
		{6, 8},	  // IDLE 1 (Row 6)
		{7, 8},	  // IDLE 2 (Row 7)
		{8, 8},	  // IDLE 3 (Row 8)
		{17, 13}, // IDLE 4 (Row 17)
		{18, 13}, // IDLE 5 (Row 18)
		{19, 13}, // IDLE 6 (Row 19)
		{20, 13}, // IDLE 7 (Row 20)
	};

	int choice = rand() % ARRAY_COUNT(IDLES);

	Rectangle frames[13]; // Room for the longest idle
	SpriteRow(frames, IDLES[choice].row, IDLES[choice].count, SPRITE_CELL);
	InitGameObjectAnimation(object, frames, IDLES[choice].count, 0.18f);
}

void PlayerEnterIdle(GameObject *object, float deltaTime)
{
	// Player *player = (Player *)object;

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

	// Complete the remainder of the method
}

// ==========================================================================
// WALKING
// ==========================================================================

void PlayerEnterWalking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	(void)player;

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
	(void)player;

	// Complete the remainder of the method
}

// ==========================================================================
// ATTACKING (one sword swipe, then recovery)
// ==========================================================================

void PlayerEnterAttacking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	(void)player;

	// Complete the remainder of the method
	// Example: Deduct some stamina when attacking

	// A fresh swipe hasn't landed yet (see AttackHit in collision.c)
	object->attackHit = false;

	// Recovery timer starts again for this swipe (see PlayerUpdateAttacking)
	object->timer = 0.0f;

	// One swipe, facing the way the player was going
	InitAttackAnimation(object);

	// Entry runs once per swipe, so this is the place for the swoosh
	MediatorNotify(object->mediator, GAME_EVENT_SWIPE, object, NULL);
}

void PlayerUpdateAttacking(GameObject *object, float deltaTime)
{
	// Complete the remainder of the method
	// Check if the attack should end or be interrupted (e.g., stamina depletion)

	// No movement while swinging: the swipe frames have no walking legs,
	// so moving here looks like sliding on ice. Feet planted, sword out.

	// Update the Animation
	UpdateAnimation(&object->animation, deltaTime);

	// Swipe finished? Hold the last frame for ATTACK_RECOVERY (sword down,
	// a missed swing leaves you open), then tell the FSM, and transition table decides what's next
	if (object->animation.finished)
	{
		object->timer += deltaTime;
		if (object->timer >= ATTACK_RECOVERY)
			HandleEvent(object, EVENT_ANIMATION_END, deltaTime);
	}
}

void PlayerExitAttacking(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	Player *player = (Player *)object;
	(void)player;

	// Complete the remainder of the method
	// Reset or adjust any temporary changes during attack, if needed
}

// ==========================================================================
// SHIELDING (yours to finish: see the tasks)
// ==========================================================================

void PlayerEnterShielding(GameObject *object, float deltaTime)
{
	// TODO : Not Currently Implemented
	(void)deltaTime;
	Player *player = (Player *)object;
	(void)player;

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
	(void)player;

	// Complete the remainder of the method
	// Reset any temporary shielding effects if necessary
}

// ==========================================================================
// DEAD (yours to finish: see the tasks)
// ==========================================================================

void PlayerEnterDead(GameObject *object, float deltaTime)
{
	// TODO : Not Currently Implemented
	(void)deltaTime;

	// Complete the remainder of the method
}

void PlayerUpdateDead(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	ChangeState(object, STATE_RESPAWN, deltaTime);
	// Complete the remainder of the method
	UpdateAnimation(&object->animation, deltaTime);
}

void PlayerExitDead(GameObject *object, float deltaTime)
{
	(void)deltaTime;

	// Complete the remainder of the method
}

// ==========================================================================
// RESPAWN (yours to finish: see the tasks)
// ==========================================================================

void PlayerEnterRespawn(GameObject *object, float deltaTime)
{
	// TODO : Not Currently Implemented
	(void)deltaTime;

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

	// Complete the remainder of the method
}