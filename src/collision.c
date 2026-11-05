#include <raylib.h>
#include <stdio.h>

#include "utils/collision.h"
#include "utils/utils.h"
#include "utils/audio_manager.h"

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
 * Bumping into each other doesn't hurt anymore .... the damage is dealt by
 * the sword, on the frame the blade lands (see AttackHit below).
 * The FSMs hear about the bump through EVENT_COLLISION_START, which is what
 * starts the NPC swinging. Add a wee tap here if you want bumps to hurt too.
 */

void CollisionEntry(GameObject *lhs, GameObject *rhs)
{
	(void)lhs;
	(void)rhs;
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

/**
 * AttackHit : Has the sword landed?
 *
 * @attacker: The one swinging the sword.
 * @target:   The one hoping it misses.
 *
 * Called every frame, but a swipe can only land once, on the frame the blade
 * comes round (ATTACK_HIT_FRAME), and only if:
 *     - the target is in reach (the two colliders, plus ATTACK_REACH)
 *     - the attacker is facing the target (no hitting lads behind your back)
 *     - the target isn't hiding behind its shield
 *
 * Damage on the hit frame feels right: no damage while the sword is still
 * on its way, and none for standing too close for the whole swipe.
 */

void AttackHit(GameObject *attacker, GameObject *target)
{
	// Only while swinging, and only once per swipe
	if (attacker->currentState != STATE_ATTACKING || attacker->attackHit)
		return;

	// Wait for the blade to come round
	if (attacker->animation.currentFrame < ATTACK_HIT_FRAME)
		return;

	// This swipe has had its chance, hit or miss
	attacker->attackHit = true;

	// In reach?
	float reach = attacker->collider.r + target->collider.r + ATTACK_REACH;
	float distance = Vector2Distance(attacker->position, target->position);
	if (distance > reach)
		return;

	// Facing the target? (dot product > 0 means the target is in front)
	Vector2 toTarget = Vector2Normalize(Vector2Subtract(target->position, attacker->position));
	if (Vector2DotProduct(toTarget, DirectionVector(attacker->currentDirection)) <= 0.0f)
		return;

	// Blocked by a shield?
	if (target->currentState == STATE_SHIELD)
	{
		printf("%s blocked %s's sword\n", target->name, attacker->name);
		PlayActionSound(SOUND_BLOCK);
		return;
	}

	// Landed
	target->health -= DAMAGE_DEFAULT;
	if (target->health < 0)
		target->health = 0;

	printf("%s hit %s: Damage [ %d ] Health [ %d ]\n", attacker->name, target->name, DAMAGE_DEFAULT, target->health);
	PlayActionSound(SOUND_HIT);
}
