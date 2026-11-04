#include <raylib.h>
#include <stdio.h>

#include "./utils/collision.h"

/**
 * CheckCollision : Checks if two game objects have bumped into each other.
 *
 * @lhs: First object (usually the player).
 * @rhs: Second object (usually an NPC).
 *
 * Uses a simple circle-to-circle test (cute_c2 yoke very handy). If the two circles overlap,
 * we call that a collision ... fair play to you for hitting something (stop when you hear the bang).
 * Call the insurance assessor lads.
 *
 * @return: true if they collide, false if they keep their (social) distance.
 */

bool CheckCollision(GameObject *lhs, GameObject *rhs)
{
	return c2CircletoCircle(lhs->collider, rhs->collider);
}

/**
 * HandleCollision : Deals with the aftermath of two objects colliding (see insurance assessor)
 *
 * @lhs: The object that's getting pushed back (usually the player).
 * @rhs: The other object involved in the bump (usually the NPC).
 *
 * Calculates the collision manifold and shoves the lhs object out of
 * the rhs object so they're not sitting on top each other like two
 * lads sharing the same seat at the overcrowed Carlow train station.
 *
 * Colours are changed to show "hey, something lad hit me".
 */

void HandleCollision(GameObject *lhs, GameObject *rhs)
{
	c2CircletoCircleManifold(lhs->collider, rhs->collider, &rhs->manifold);

	if (rhs->manifold.count > 0)
	{
		// Simple collision response: push the lhs circle out of the NPC circle along the normal
		// rhs->manifold.n is the collision normal
		// rhs->manifold.depths[0] is the penetration depth
		float depth = rhs->manifold.depths[0];
		// Does not allow the lhs to penetrate the circle
		lhs->collider.p = c2Sub(lhs->collider.p, c2Mulvs(rhs->manifold.n, depth + PUSHBACK_DISTANCE));
	}

	// Change NPC color to indicate it has collided
	lhs->color = RED;
	rhs->color = RED;

	// Change position of Player as in respond to collision
	lhs->position.x = lhs->collider.p.x;
	lhs->position.y = lhs->collider.p.y;
}

/**
 * CollisionEntry : Called when two objects begin colliding.
 *
 * @lhs: First poor victim
 * @rhs: Second poor victim / volunteer
 *
 * Both take a bit of damage ... nothing dramatic, just a wee tap.
 * Head to the local panel beaters to fix her up like.
 */

void CollisionEntry(GameObject *lhs, GameObject *rhs)
{
	// Simple Damage System
	if (lhs->currentState == STATE_ATTACKING)
	{
		rhs->health -= DAMAGE_DEFAULT; // Example: Reduce rhs's (NPC's) health on collision
		printf("NPC Damage [ %d ] Health[ %d ]\n", DAMAGE_DEFAULT, rhs->health);		
	}
	else
	{
		if (rhs->currentState == STATE_ATTACKING)
		{
			lhs->health -= DAMAGE_DEFAULT; // Example: Reduce Players health on collision
			printf("Player Damage [ %d ] Health[ %d ]\n", DAMAGE_DEFAULT, lhs->health);
		}
	}
}

/**
 * CollisionExit : Called when two objects stop colliding.
 *
 * Resets the colours so they're not stuck looking embarrassed.
 */

void CollisionExit(GameObject *lhs, GameObject *rhs)
{
	lhs->color = DARKGREEN; // Reset Player default color
	rhs->color = DARKGREEN; // Reset NPC default color
}
