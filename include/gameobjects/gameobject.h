#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "../utils/cute_c2.h"

#include "./utils/constants.h"

#include "./events/events.h"
#include "./fsm/fsm.h"
#include "./animation/animation.h"

// GameObjects Direction
// This is used to set the animation directions
typedef enum Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;

// Base structure for a game object
typedef struct GameObject
{
	const char *name; // The name of the game object (e.g., "Player", "Enemy")

	// Positions and Transforms
	Vector2 position;			 // Gameobjects position in the game world
	Vector2 inputAxis;			 // Input Axis (WASD / Arrow Keys / D-Pad and Tumbstick)
	Direction currentDirection;	 // Used to face animations in correct direction
	Direction previousDirection; // Used to face animations in correct direction

	// Collision components
	c2Circle collider;	 // Circle collider used for collision detection
	c2Manifold manifold; // Manifold for finding incident normals
	bool isColliding;	 // Is Colliding

	// FSM
	StateConfig *stateConfigs; // Pointer to the array of state configurations for this game object
	State previousState;	   // The state the game object was previously in
	State currentState;		   // The current state of the game object

	// Animation
	AnimationData animation; // Player Animation
	Color color;			 // Gameobject's color, changes based on currentState
	Texture2D keyframes;	 // Sprite Sheet Texture

	// Gameplay Components
	int health;	 // The health of the game object
	float timer; // Can be used during updates

} GameObject;

// Initialize a new game object with the given name and default values
void InitGameObject(GameObject *object,
					const char *name,
					Vector2 position,
					State currentState,
					Color color,
					c2Circle collider,
					Texture2D keyframes,
					int health);

// Helper function to initialize animation
void InitGameObjectAnimation(GameObject *object, Rectangle *frames, int frameCount, float speed);

// Keep on screen
void ClampGameObjectOnScreen(GameObject *object);

// Check collision
bool CheckCollision(GameObject *lhs, GameObject *rhs);

// Handle Collision
void HandleCollision(GameObject *lhs, GameObject *rhs);

// Delete a game object and free associated memory/resources
void DeleteGameObject(GameObject *object);

#endif // GAMEOBJECT_H