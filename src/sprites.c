#include "animation/sprites.h"

// How long each frame shows for (seconds)
#define WALK_FRAME_TIME 0.055f
#define ATTACK_FRAME_TIME 0.035f

// Frames per animation
#define WALK_FRAMES 9
#define ATTACK_FRAMES 6

// Which row each direction lives on. [UP] = 9 puts 9 in the slot for UP,
// so the order here doesn't matter (same trick as the FSM's name lookups).
static const int WALK_ROWS[] = {
	[UP] = 9,	  // WALK UP    (Row 9)
	[LEFT] = 10,  // WALK LEFT  (Row 10)
	[DOWN] = 11,  // WALK DOWN  (Row 11)
	[RIGHT] = 12, // WALK RIGHT (Row 12)
};

// Sword swipes are 3 grid rows tall, so they start every 3 rows
static const int ATTACK_ROWS[] = {
	[UP] = 47,	  // ATTACK UP    (Rows 47-49)
	[LEFT] = 50,  // ATTACK LEFT  (Rows 50-52)
	[DOWN] = 53,  // ATTACK DOWN  (Rows 53-55)
	[RIGHT] = 56, // ATTACK RIGHT (Rows 56-58)
};

/**
 * SpriteRow : Works out where each frame of an animation is on the sheet.
 *
 * Every frame is a Rectangle {Pixel X, Pixel Y, width, height}. Along a row,
 * only Pixel X changes, so a row of frames is just a bit of maths:
 *
 *     Pixel Y = (row - 1) x 64          (rows are on the 64 x 64 grid)
 *     Pixel X = (column - 1) x size     (columns step by the frame size)
 *
 * Worked example: WALK UP is Row 9, 9 frames of 64 x 64
 *
 *     SpriteRow(frames, 9, 9, SPRITE_CELL);
 *
 *     Pixel Y = (9 - 1) x 64 = 512
 *     frames[0] = {0,   512, 64, 64}   // Frame 1: Column 1 (Pixel : X 0)
 *     frames[1] = {64,  512, 64, 64}   // Frame 2: Column 2 (Pixel : X 64)
 *     frames[2] = {128, 512, 64, 64}   // Frame 3: Column 3 (Pixel : X 128)
 *     ...
 *     frames[8] = {512, 512, 64, 64}   // Frame 9: Column 9 (Pixel : X 512)
 *
 * Sword swipes are 192 x 192, but still start on a 64 grid row:
 *
 *     SpriteRow(frames, 47, 6, SPRITE_SWIPE);   // ATTACK UP, Rows 47-49
 *
 *     Pixel Y = (47 - 1) x 64 = 2944
 *     frames[0] = {0,   2944, 192, 192}  // Frame 1: Column 1 (Pixel : X 0)
 *     frames[1] = {192, 2944, 192, 192}  // Frame 2: Column 2 (Pixel : X 192)
 *     ...
 *
 * Check any of these against resources/production/grid_*_sprite_sheet.png.
 *
 * @frames: Where the rectangles go (room for at least count of them).
 * @row:    Row on the 64 x 64 grid, counted from 1 (as on the grid image).
 * @count:  How many frames along the row.
 * @size:   Frame size: SPRITE_CELL (64) or SPRITE_SWIPE (192).
 */

void SpriteRow(Rectangle *frames, int row, int count, int size)
{
	float y = (float)((row - 1) * SPRITE_CELL);

	for (int column = 1; column <= count; column++)
	{
		frames[column - 1] = (Rectangle){
			(float)((column - 1) * size), // Pixel X
			y,							  // Pixel Y
			(float)size,				  // width
			(float)size};				  // height
	}
}

/**
 * InitWalkAnimation : The walk for the way the object is facing (loops).
 *
 * Shared by the player and the NPC: both sheets have the walk on Rows 9-12.
 * Uses currentDirection on the GameObject.
 */

void InitWalkAnimation(GameObject *object)
{
	Rectangle frames[WALK_FRAMES];
	SpriteRow(frames, WALK_ROWS[object->currentDirection], WALK_FRAMES, SPRITE_CELL);
	InitGameObjectAnimation(object, frames, WALK_FRAMES, WALK_FRAME_TIME);
}

/**
 * InitAttackAnimation : One sword swipe the way the object is facing (plays once).
 *
 * Shared by the player and the NPC: both sheets have the swipes on Rows 47-58.
 * Plays once, so the FSM gets EVENT_ANIMATION_END when it's done.
 */

void InitAttackAnimation(GameObject *object)
{
	Rectangle frames[ATTACK_FRAMES];
	SpriteRow(frames, ATTACK_ROWS[object->currentDirection], ATTACK_FRAMES, SPRITE_SWIPE);
	InitGameObjectAnimationOnce(object, frames, ATTACK_FRAMES, ATTACK_FRAME_TIME);
}
