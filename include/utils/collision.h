#ifndef COLLISION_H
#define COLLISION_H

#include "cute_c2.h"

#include "./gameobjects/gameobject.h"

// Collision Methods
bool CheckCollision(GameObject *lhs, GameObject *rhs);
void HandleCollision(GameObject *lhs, GameObject *rhs);

// Collision on Entry and Exit to Handle Collision Response
void CollisionEntry(GameObject *lhs, GameObject *rhs);
void CollisionExit(GameObject *lhs, GameObject *rhs);

#endif // COLLISION_H
