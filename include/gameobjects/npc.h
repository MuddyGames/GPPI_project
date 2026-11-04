#ifndef NPC_H
#define NPC_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Include the header for the base game object
#include "gameobjects/gameobject.h"

// Define the NPC structure that extends GameObject with an additional aggression property
typedef struct
{
	GameObject base;  // Base game object (inherits from GameObject)
	float aggression; // Aggression level of the NPC (could affect behavior)
	Vector2 target;	  // Target position the NPC to chase (player position)
	Direction facing; // Which way the walk animation faces (the AI's commands can change currentDirection)
} NPC;

// Initialise a new NPC with a given name (returns a pointer to the NPC)
NPC *InitNPC(const char *name, Vector2 position, int radius);

// Cleanup NPC
void DeleteNPC(GameObject *object);

// Initialise NPC-specific states for the given GameObject
void InitNPCFSM(GameObject *object);

// NPC-specific behaviors for different states

// State functions for idle state
void NPCEnterIdle(GameObject *object, float deltaTime);
void NPCUpdateIdle(GameObject *object, float deltaTime);
void NPCExitIdle(GameObject *object, float deltaTime);

// State functions for walking state (chasing the target)
void NPCEnterWalking(GameObject *object, float deltaTime);
void NPCUpdateWalking(GameObject *object, float deltaTime);
void NPCExitWalking(GameObject *object, float deltaTime);

// State functions for attacking state
void NPCEnterAttacking(GameObject *object, float deltaTime);
void NPCUpdateAttacking(GameObject *object, float deltaTime);
void NPCExitAttacking(GameObject *object, float deltaTime);

// State functions for shielding state
void NPCEnterShielding(GameObject *object, float deltaTime);
void NPCUpdateShielding(GameObject *object, float deltaTime);
void NPCExitShielding(GameObject *object, float deltaTime);

// State functions for dead state
void NPCEnterDead(GameObject *object, float deltaTime);
void NPCUpdateDead(GameObject *object, float deltaTime);
void NPCExitDead(GameObject *object, float deltaTime);

#endif // NPC_H