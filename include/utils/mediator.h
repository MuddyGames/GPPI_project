#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <stdbool.h>

#include "./command/command.h"
#include "./gameobjects/gameobject.h"

// Define the Mediator structure
typedef struct Mediator
{
    GameObject *object;
} Mediator;

// Function to create a mediator instance
Mediator *CreateMediator(GameObject *object);

// void MediatorExecuteCommand(Command command, Mediator *mediator);
void MediatorHandleCommand(Mediator *mediator, Command command, float deltaTime);

// Called from Game PollsInput
void MediatorUpdatePlayer(Mediator *mediator, float deltaTime);

// PollsAI
void MediatorUpdateNPC(Mediator *mediator, float deltaTime);

// Mediator Cleanup
void DeleteMediator(Mediator *mediator);

#endif