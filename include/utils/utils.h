#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>
#include "gameobjects/gameobject.h"

// How many items in a fixed-size array, e.g. ARRAY_COUNT(idleValidTransitions).
// Only works on a real array, not on a pointer to one: sizeof a pointer is
// the size of the pointer, not of what it points at.
#define ARRAY_COUNT(array) (int)(sizeof(array) / sizeof((array)[0]))

Direction DirectionAxis(Vector2 axis);
Vector2 DirectionVector(Direction direction);
void GameObjectMove(GameObject *object, Vector2 direction, float deltaTime);

#endif // UTILS_H
