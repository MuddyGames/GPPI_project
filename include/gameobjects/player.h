#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Include the header for the base game object
#include "gameobject.h"

// Define the Player structure that extends GameObject with additional properties like stamina and mana
typedef struct
{
	GameObject base; // The base game object (inherits from GameObject)
	float stamina;	 // The player's stamina (could affect actions like running, attacking)
	float mana;		 // The player's mana (could affect magic abilities or special moves)
} Player;

// Initialise a new Player with a given name (returns a pointer to the Player)
Player *InitPlayer(const char *name, Vector2 position, int radius);

// Cleanup Player
void DeletePlayer(GameObject *object);

// Initialize the finite state machine (FSM) for the Player (sets up the player's states)
void InitPlayerFSM(GameObject *object);

// Player-specific behaviors for different states

// State functions for idle state
void PlayerEnterIdle(GameObject *object, float deltaTime);	// Called when entering the idle state
void PlayerUpdateIdle(GameObject *object, float deltaTime); // Called to update the player's behavior while idle
void PlayerExitIdle(GameObject *object, float deltaTime);	// Called when exiting the idle state

// State functions for walking state
void PlayerEnterWalking(GameObject *object, float deltaTime);  // Called when entering the walking state
void PlayerUpdateWalking(GameObject *object, float deltaTime); // Called to update the player's behavior while walking
void PlayerExitWalking(GameObject *object, float deltaTime);   // Called when exiting the walking state


// State functions for attacking state
void PlayerEnterAttacking(GameObject *object, float deltaTime);	 // Called when entering the attacking state
void PlayerUpdateAttacking(GameObject *object, float deltaTime); // Called to update the player's behavior while attacking
void PlayerExitAttacking(GameObject *object, float deltaTime);	 // Called when exiting the attacking state

// State functions for shielding state
void PlayerEnterShielding(GameObject *object, float deltaTime);	 // Called when entering the shielding state
void PlayerUpdateShielding(GameObject *object, float deltaTime); // Called to update the player's behavior while shielding
void PlayerExitShielding(GameObject *object, float deltaTime);	 // Called when exiting the shielding state

// State functions for die state
void PlayerEnterDie(GameObject *object, float deltaTime);  // Called when entering the die state
void PlayerUpdateDie(GameObject *object, float deltaTime); // Called to update the player's behavior while dead
void PlayerExitDie(GameObject *object, float deltaTime);   // Called when exiting the dead state

// State functions for respawn state
void PlayerEnterRespawn(GameObject *object, float deltaTime);  // Called when entering the respawn state
void PlayerUpdateRespawn(GameObject *object, float deltaTime); // Called to update the player's behavior while respawning
void PlayerExitRespawn(GameObject *object, float deltaTime);   // Called when exiting the respawn

#endif // PLAYER_H