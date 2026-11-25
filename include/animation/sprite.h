#ifndef SPRITE_H
#define SPRITE_H

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
// Rows are counted from 1, as on the grid image:
//   Pixel Y = (row - 1) x 64, Pixel X = (column - 1) x frame width
// ---------------------------------------------------------------------------

// Every frame coordinate lives in src/sprite.c. Using a different
// sprite sheet? That's the only file to change.

// Shared by the player and the NPC (same rows on both sheets)
void InitWalkAnimation(GameObject *object);   // loops, faces currentDirection
void InitAttackAnimation(GameObject *object); // plays once, faces currentDirection
void InitShieldAnimation(GameObject *object); // loops
void InitDeadAnimation(GameObject *object);   // plays once, stays lying down

// Each has its own idle
void InitPlayerIdleAnimation(GameObject *object); // random pick of the player's idles
void InitNPCIdleAnimation(GameObject *object);    // loops

#endif // SPRITE_H