#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <stdbool.h>

#include "command/command.h"
#include "gameobjects/gameobject.h"
#include "hud/hud.h"

// Define the Mediator structure
// Mediator knows about (game object and HUD).
// Game object and HUD don't know about each other.
// The NPC's mediator has no HUD (hud is NULL).
typedef struct Mediator
{
    GameObject *object;
    Hud *hud;
} Mediator;

// Function to create a mediator instance (hud can be NULL)
Mediator *CreateMediator(GameObject *object, Hud *hud);

// Handle Command: passes the command on to the game object and the HUD
void MediatorHandleCommand(Mediator *mediator, Command command, float deltaTime);

// Mediator Cleanup: frees what CreateMediator() allocated
void DeleteMediator(Mediator *mediator);

#endif