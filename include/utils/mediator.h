#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <stdbool.h>

#include "command/command.h"
#include "gameobjects/gameobject.h"

// Define the Mediator structure
typedef struct Mediator
{
    GameObject *object;
} Mediator;

// Function to create a mediator instance
Mediator *CreateMediator(GameObject *object);

// Handle Command: passes the command on to the game object
void MediatorHandleCommand(Mediator *mediator, Command command, float deltaTime);

// Mediator Cleanup: frees what CreateMediator() allocated
void DeleteMediator(Mediator *mediator);

#endif