#include <stdlib.h>
#include "animation/sprite.h"
#include "utils/utils.h"

/*
 * sprite.c : EVERY frame coordinate in the game lives in this file.
 * 
 * Frames are written out as coordinates, not calculated, so this works
 * with any sprite sheet: the starter art, another pack, your own art,
 * or a sheet made with a texture packer (copy the x, y, w, h it exports).
 *
 * Using a different sprite sheet (your own art, another pack)?
 * This is the only file you need to change. player.c and npc.c just ask for
 * an animation by name (InitWalkAnimation, InitDeadAnimation, ...).
 *
 * Each Rectangle is {Pixel X, Pixel Y, width, height} on the sheet.
 * Frames do NOT have to be on a grid or all the same size: list them
 * wherever they are. Check them against art/production/grid_*_sprite_sheet.png.
 *
 * Frame counts are worked out with ARRAY_COUNT, so to add or remove a
 * frame, just add or remove a line. No other number needs changing.
 *
 * Frames are drawn centred on the object's position (see DrawAnimation),
 * so a 192 x 192 swipe frame lines up with a 64 x 64 walk frame.
 */

// How long each frame shows for (seconds)
// TODO: Might move to constants here for now for tweeking
#define IDLE_FRAME_TIME 0.18f
#define NPC_IDLE_FRAME_TIME 0.1f
#define WALK_FRAME_TIME 0.055f
#define ATTACK_FRAME_TIME 0.035f
#define SHIELD_FRAME_TIME 0.08f
#define DEAD_FRAME_TIME 0.2f

// A list of frames and how many there are.
// FRAMES(table) macro below fills in both.
typedef struct
{
	Rectangle *frames;
	int count;
} FrameList;

// Handy macro for FRAMES (might move to utils.h)
#define FRAMES(table) {(table), ARRAY_COUNT(table)}

// ==========================================================================
// WALK (player and NPC: loops)
// ==========================================================================

// WALK UP (Row 9, Pixel : Y = 8 x 64 = 512)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle walk_UP[] = {
	{0, 512, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 512, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 512, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 512, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 512, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 512, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 512, 64, 64},     // Frame 7: Column 7 (Pixel : X 384)
	{448, 512, 64, 64},     // Frame 8: Column 8 (Pixel : X 448)
	{512, 512, 64, 64}      // Frame 9: Column 9 (Pixel : X 512)
};

// WALK LEFT (Row 10, Pixel : Y = 9 x 64 = 576)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle walk_LEFT[] = {
	{0, 576, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 576, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 576, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 576, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 576, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 576, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 576, 64, 64},     // Frame 7: Column 7 (Pixel : X 384)
	{448, 576, 64, 64},     // Frame 8: Column 8 (Pixel : X 448)
	{512, 576, 64, 64}      // Frame 9: Column 9 (Pixel : X 512)
};

// WALK DOWN (Row 11, Pixel : Y = 10 x 64 = 640)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle walk_DOWN[] = {
	{0, 640, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 640, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 640, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 640, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 640, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 640, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 640, 64, 64},     // Frame 7: Column 7 (Pixel : X 384)
	{448, 640, 64, 64},     // Frame 8: Column 8 (Pixel : X 448)
	{512, 640, 64, 64}      // Frame 9: Column 9 (Pixel : X 512)
};

// WALK RIGHT (Row 12, Pixel : Y = 11 x 64 = 704)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle walk_RIGHT[] = {
	{0, 704, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 704, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 704, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 704, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 704, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 704, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 704, 64, 64},     // Frame 7: Column 7 (Pixel : X 384)
	{448, 704, 64, 64},     // Frame 8: Column 8 (Pixel : X 448)
	{512, 704, 64, 64}      // Frame 9: Column 9 (Pixel : X 512)
};

// Which frames go with which direction
static const FrameList WALK[] = {
	[UP] = FRAMES(walk_UP),
	[LEFT] = FRAMES(walk_LEFT),
	[DOWN] = FRAMES(walk_DOWN),
	[RIGHT] = FRAMES(walk_RIGHT),
};

// ==========================================================================
// SWORD ATTACK (player and NPC: plays once, 192 x 192 frames)
// ==========================================================================

// ATTACK UP (Rows 47-49, Pixel : Y = 46 x 64 = 2944)
// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
static Rectangle sword_attack_UP[] = {
	{0, 2944, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
	{192, 2944, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
	{384, 2944, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
	{576, 2944, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
	{768, 2944, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
	{960, 2944, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
};

// ATTACK LEFT (Rows 50-52, Pixel : Y = 49 x 64 = 3136)
// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
static Rectangle sword_attack_LEFT[] = {
	{0, 3136, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
	{192, 3136, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
	{384, 3136, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
	{576, 3136, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
	{768, 3136, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
	{960, 3136, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
};

// ATTACK DOWN (Rows 53-55, Pixel : Y = 52 x 64 = 3328)
// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
static Rectangle sword_attack_DOWN[] = {
	{0, 3328, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
	{192, 3328, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
	{384, 3328, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
	{576, 3328, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
	{768, 3328, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
	{960, 3328, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
};

// ATTACK RIGHT (Rows 56-58, Pixel : Y = 55 x 64 = 3520)
// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
static Rectangle sword_attack_RIGHT[] = {
	{0, 3520, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
	{192, 3520, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
	{384, 3520, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
	{576, 3520, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
	{768, 3520, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
	{960, 3520, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
};

static const FrameList ATTACK[] = {
	[UP] = FRAMES(sword_attack_UP),
	[LEFT] = FRAMES(sword_attack_LEFT),
	[DOWN] = FRAMES(sword_attack_DOWN),
	[RIGHT] = FRAMES(sword_attack_RIGHT),
};

// ==========================================================================
// PLAYER IDLE (loops: a random one is picked each time one finishes)
// ==========================================================================

// IDLE 1 (Row 6, Pixel : Y = 5 x 64 = 320)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle idle1[] = {
	{0, 320, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 320, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 320, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 320, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 320, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 320, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 320, 64, 64},     // Frame 7: Column 7 (Pixel : X 384)
	{448, 320, 64, 64}      // Frame 8: Column 8 (Pixel : X 448)
};

// IDLE 2 (Row 7, Pixel : Y = 6 x 64 = 384)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle idle2[] = {
	{0, 384, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 384, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 384, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 384, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 384, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 384, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 384, 64, 64},     // Frame 7: Column 7 (Pixel : X 384)
	{448, 384, 64, 64}      // Frame 8: Column 8 (Pixel : X 448)
};

// IDLE 3 (Row 8, Pixel : Y = 7 x 64 = 448)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle idle3[] = {
	{0, 448, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 448, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 448, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 448, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 448, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 448, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 448, 64, 64},     // Frame 7: Column 7 (Pixel : X 384)
	{448, 448, 64, 64}      // Frame 8: Column 8 (Pixel : X 448)
};

// IDLE 4 (Row 17, Pixel : Y = 16 x 64 = 1024)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle idle4[] = {
	{0, 1024, 64, 64},      // Frame 1: Column 1 (Pixel : X 0)
	{64, 1024, 64, 64},     // Frame 2: Column 2 (Pixel : X 64)
	{128, 1024, 64, 64},    // Frame 3: Column 3 (Pixel : X 128)
	{192, 1024, 64, 64},    // Frame 4: Column 4 (Pixel : X 192)
	{256, 1024, 64, 64},    // Frame 5: Column 5 (Pixel : X 256)
	{320, 1024, 64, 64},    // Frame 6: Column 6 (Pixel : X 320)
	{384, 1024, 64, 64},    // Frame 7: Column 7 (Pixel : X 384)
	{448, 1024, 64, 64},    // Frame 8: Column 8 (Pixel : X 448)
	{512, 1024, 64, 64},    // Frame 9: Column 9 (Pixel : X 512)
	{576, 1024, 64, 64},    // Frame 10: Column 10 (Pixel : X 576)
	{640, 1024, 64, 64},    // Frame 11: Column 11 (Pixel : X 640)
	{704, 1024, 64, 64},    // Frame 12: Column 12 (Pixel : X 704)
	{768, 1024, 64, 64}     // Frame 13: Column 13 (Pixel : X 768)
};

// IDLE 5 (Row 18, Pixel : Y = 17 x 64 = 1088)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle idle5[] = {
	{0, 1088, 64, 64},      // Frame 1: Column 1 (Pixel : X 0)
	{64, 1088, 64, 64},     // Frame 2: Column 2 (Pixel : X 64)
	{128, 1088, 64, 64},    // Frame 3: Column 3 (Pixel : X 128)
	{192, 1088, 64, 64},    // Frame 4: Column 4 (Pixel : X 192)
	{256, 1088, 64, 64},    // Frame 5: Column 5 (Pixel : X 256)
	{320, 1088, 64, 64},    // Frame 6: Column 6 (Pixel : X 320)
	{384, 1088, 64, 64},    // Frame 7: Column 7 (Pixel : X 384)
	{448, 1088, 64, 64},    // Frame 8: Column 8 (Pixel : X 448)
	{512, 1088, 64, 64},    // Frame 9: Column 9 (Pixel : X 512)
	{576, 1088, 64, 64},    // Frame 10: Column 10 (Pixel : X 576)
	{640, 1088, 64, 64},    // Frame 11: Column 11 (Pixel : X 640)
	{704, 1088, 64, 64},    // Frame 12: Column 12 (Pixel : X 704)
	{768, 1088, 64, 64}     // Frame 13: Column 13 (Pixel : X 768)
};

// IDLE 6 (Row 19, Pixel : Y = 18 x 64 = 1152)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle idle6[] = {
	{0, 1152, 64, 64},      // Frame 1: Column 1 (Pixel : X 0)
	{64, 1152, 64, 64},     // Frame 2: Column 2 (Pixel : X 64)
	{128, 1152, 64, 64},    // Frame 3: Column 3 (Pixel : X 128)
	{192, 1152, 64, 64},    // Frame 4: Column 4 (Pixel : X 192)
	{256, 1152, 64, 64},    // Frame 5: Column 5 (Pixel : X 256)
	{320, 1152, 64, 64},    // Frame 6: Column 6 (Pixel : X 320)
	{384, 1152, 64, 64},    // Frame 7: Column 7 (Pixel : X 384)
	{448, 1152, 64, 64},    // Frame 8: Column 8 (Pixel : X 448)
	{512, 1152, 64, 64},    // Frame 9: Column 9 (Pixel : X 512)
	{576, 1152, 64, 64},    // Frame 10: Column 10 (Pixel : X 576)
	{640, 1152, 64, 64},    // Frame 11: Column 11 (Pixel : X 640)
	{704, 1152, 64, 64},    // Frame 12: Column 12 (Pixel : X 704)
	{768, 1152, 64, 64}     // Frame 13: Column 13 (Pixel : X 768)
};

// IDLE 7 (Row 20, Pixel : Y = 19 x 64 = 1216)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle idle7[] = {
	{0, 1216, 64, 64},      // Frame 1: Column 1 (Pixel : X 0)
	{64, 1216, 64, 64},     // Frame 2: Column 2 (Pixel : X 64)
	{128, 1216, 64, 64},    // Frame 3: Column 3 (Pixel : X 128)
	{192, 1216, 64, 64},    // Frame 4: Column 4 (Pixel : X 192)
	{256, 1216, 64, 64},    // Frame 5: Column 5 (Pixel : X 256)
	{320, 1216, 64, 64},    // Frame 6: Column 6 (Pixel : X 320)
	{384, 1216, 64, 64},    // Frame 7: Column 7 (Pixel : X 384)
	{448, 1216, 64, 64},    // Frame 8: Column 8 (Pixel : X 448)
	{512, 1216, 64, 64},    // Frame 9: Column 9 (Pixel : X 512)
	{576, 1216, 64, 64},    // Frame 10: Column 10 (Pixel : X 576)
	{640, 1216, 64, 64},    // Frame 11: Column 11 (Pixel : X 640)
	{704, 1216, 64, 64},    // Frame 12: Column 12 (Pixel : X 704)
	{768, 1216, 64, 64}     // Frame 13: Column 13 (Pixel : X 768)
};

// The Player has 7 possible idle animations
static const FrameList PLAYER_IDLES[] = {
	FRAMES(idle1), 
	FRAMES(idle2), 
	FRAMES(idle3), 
	FRAMES(idle4),
	FRAMES(idle5), 
	FRAMES(idle6), 
	FRAMES(idle7)
};

// ==========================================================================
// NPC IDLE (loops)
// ==========================================================================

// NPC IDLE (Row 3, Pixel : Y = 2 x 64 = 128)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle npc_idle[] = {
	{0, 128, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 128, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 128, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 128, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 128, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 128, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 128, 64, 64}      // Frame 7: Column 7 (Pixel : X 384)
};

// ==========================================================================
// SHIELD (loops while shielding)
// ==========================================================================

// SHIELDING (Row 7, Pixel : Y = 6 x 64 = 384)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle shielding[] = {
	{0, 384, 64, 64},       // Frame 1: Column 1 (Pixel : X 0)
	{64, 384, 64, 64},      // Frame 2: Column 2 (Pixel : X 64)
	{128, 384, 64, 64},     // Frame 3: Column 3 (Pixel : X 128)
	{192, 384, 64, 64},     // Frame 4: Column 4 (Pixel : X 192)
	{256, 384, 64, 64},     // Frame 5: Column 5 (Pixel : X 256)
	{320, 384, 64, 64},     // Frame 6: Column 6 (Pixel : X 320)
	{384, 384, 64, 64},     // Frame 7: Column 7 (Pixel : X 384)
	{448, 384, 64, 64}      // Frame 8: Column 8 (Pixel : X 448)
};

// ==========================================================================
// DEAD (player and NPC: plays once, stays lying down)
// ==========================================================================

// DEAD (Row 21, Pixel : Y = 20 x 64 = 1280)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle dead[] = {
	{0, 1280, 64, 64},      // Frame 1: Column 1 (Pixel : X 0)
	{64, 1280, 64, 64},     // Frame 2: Column 2 (Pixel : X 64)
	{128, 1280, 64, 64},    // Frame 3: Column 3 (Pixel : X 128)
	{192, 1280, 64, 64},    // Frame 4: Column 4 (Pixel : X 192)
	{256, 1280, 64, 64},    // Frame 5: Column 5 (Pixel : X 256)
	{320, 1280, 64, 64}     // Frame 6: Column 6 (Pixel : X 320)
};

// ==========================================================================
// ANIMATIONS: what the player.c and the npc.c call
// ==========================================================================

/**
 * InitWalkAnimation : The walk for the way the object is facing (loops).
 * Uses currentDirection on the GameObject.
 */

void InitWalkAnimation(GameObject *object)
{
	FrameList walk = WALK[object->currentDirection];
	InitGameObjectAnimation(object, walk.frames, walk.count, WALK_FRAME_TIME);
}

/**
 * InitAttackAnimation : One sword swipe the way the object is facing.
 * Plays once, so the FSM gets EVENT_ANIMATION_END when done.
 */

void InitAttackAnimation(GameObject *object)
{
	FrameList attack = ATTACK[object->currentDirection];
	InitGameObjectAnimationOnce(object, attack.frames, attack.count, ATTACK_FRAME_TIME);
}

/**
 * InitPlayerIdleAnimation : Player idle, picked at random.
 * Just to keep the lad from looking like a statue....in the lecture
 */

void InitPlayerIdleAnimation(GameObject *object)
{
	FrameList idle = PLAYER_IDLES[rand() % ARRAY_COUNT(PLAYER_IDLES)];
	InitGameObjectAnimation(object, idle.frames, idle.count, IDLE_FRAME_TIME);
}

/**
 * InitNPCIdleAnimation : NPC idle (loops).
 */

void InitNPCIdleAnimation(GameObject *object)
{
	InitGameObjectAnimation(object, npc_idle, ARRAY_COUNT(npc_idle), NPC_IDLE_FRAME_TIME);
}

/**
 * InitShieldAnimation : Shield up (loops while shielding).
 * Note : Not implemented in Player
 */

void InitShieldAnimation(GameObject *object)
{
	InitGameObjectAnimation(object, shielding, ARRAY_COUNT(shielding), SHIELD_FRAME_TIME);
}

/**
 * InitDeadAnimation : The fall. Plays once, then stays on the last frame.
 * (Looping it would have the poor divil keel over again and again.)
 */

void InitDeadAnimation(GameObject *object)
{
	InitGameObjectAnimationOnce(object, dead, ARRAY_COUNT(dead), DEAD_FRAME_TIME);
}