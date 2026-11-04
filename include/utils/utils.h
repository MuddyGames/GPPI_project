#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>
#include "gameobjects/gameobject.h"

Direction DirectionAxis(Vector2 axis);
void GameObjectMove(GameObject *object, Vector2 direction, float deltaTime);

#endif // UTILS_H