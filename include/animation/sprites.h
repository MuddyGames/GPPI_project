#ifndef SPRITES_H
#define SPRITES_H

#include <raylib.h>

#include "gameobjects/gameobject.h"

// ---------------------------------------------------------------------------
// Sprite sheet reference card
//
// resources/player_sprite_sheet.png and resources/npc_sprite_sheet.png share
// one layout (see resources/production/grid_*_sprite_sheet.png for the grid):
//
//   Most animations are rows of 64 x 64 frames
//   Sword swipes are bigger: 192 x 192 frames (3 x 3 grid cells), Rows 47-58
//
// Rows are counted from 1, as on the grid image.
// ---------------------------------------------------------------------------

#define SPRITE_CELL 64   // Size of one grid cell, and of most frames
#define SPRITE_SWIPE 192 // Size of a sword swipe frame (3 x 3 cells)

// Fill frames[] with count frames from one row of the sheet
// (row counted from 1 on the 64 x 64 grid, size = SPRITE_CELL or SPRITE_SWIPE)
void SpriteRow(Rectangle *frames, int row, int count, int size);

// Shared by the player and the NPC (same rows on both sheets):
// pick the walk or swipe animation for the way the object is facing
void InitWalkAnimation(GameObject *object);
void InitAttackAnimation(GameObject *object);

#endif // SPRITES_H
