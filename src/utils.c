#include "utils/utils.h"

/**
 * DirectionAxis : Figures out which way the poor character is facing.
 *
 * Looks at the movement axis and decides the direction for animations,
 * saving you the hassle of guessing whether they're meant to be going
 * left, right, up, or just wandering about like a lost sheep.
 *
 * @axis: The movement vector we're basing the direction on.
 *
 * Handy inside the Player logic for picking the right animation frames,
 * so you don't end up moonwalking by accident.
 */

Direction DirectionAxis(Vector2 axis)
{
	// Axis > zero, zero
	if (fabsf(axis.x) > fabsf(axis.y))
		return (axis.x > 0.0f) ? RIGHT : LEFT;
	else
		return (axis.y > 0.0f) ? DOWN : UP;
}

/**
 * GameObjectMove - Shuffles the GameObject along the input axis.
 *
 * Updates both position and collider so collisions don't think
 * we're still back where we started three seconds ago.
 */

void GameObjectMove(GameObject *object, Vector2 direction, float deltaTime)
{
	// Update position based on direction and movement speed
	object->position.x += direction.x * DEFAULT_MOVEMENT_SPEED * deltaTime;
	object->position.y += direction.y * DEFAULT_MOVEMENT_SPEED * deltaTime;

	// Hold on screen
	ClampGameObjectOnScreen(object);

	// Update Collider
	object->collider.p.x = object->position.x;
	object->collider.p.y = object->position.y;
}