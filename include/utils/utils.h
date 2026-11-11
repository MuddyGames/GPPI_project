#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>
#include "gameobjects/gameobject.h"

// How many items in a fixed-size array, e.g. ARRAY_COUNT(idleValidTransitions).
// Only works on a real array, not on a pointer to one: sizeof a pointer is
// the size of the pointer, not of what it points at.
// This is whats called a MACRO, It just expands to the expression in parentheses.
// USAGE: int count = ARRAY_COUNT(myArray); // count is the number of elements in myArray
#define ARRAY_COUNT(array) (int)(sizeof(array) / sizeof((array)[0]))

// Convert a Vector2 axis to a Direction enum value
// typedef enum Direction
//{
//	UP,
//	DOWN,
//	LEFT,
//	RIGHT
//} Direction;
Direction DirectionAxis(Vector2 axis);

// Convert a Direction enum value to a Vector2 axis
// e.g. UP -> (0, -1), DOWN -> (0, 1), LEFT -> (-1, 0), RIGHT -> (1, 0)
Vector2 DirectionVector(Direction direction);

// Move a GameObject along a given direction vector, updating its position and collider
// @object: The GameObject to move
// @direction: The direction vector to move the GameObject in (should be normalized)
// @deltaTime: The time elapsed since the last frame (used to scale movement speed)
void GameObjectMove(GameObject *object, Vector2 direction, float deltaTime);

#endif // UTILS_H
