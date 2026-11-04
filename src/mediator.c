#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/mediator.h"

// For a grand explanation on Mediator Pattern see
// https://www.geeksforgeeks.org/mediator-design-pattern/

/**
 * CreateMediator - Builds a brand-new mediator to mind a GameObject.
 *
 * The mediator sits between input/commands and the FSM, keeping the two
 * from screaming at each other directly. Think of it as the polite friend
 * who translates your drunken ramblings into something the bouncer understands.
 *
 * @object: The GameObject this mediator will look after.
 *
 * @return: Pointer to an allocated Mediator. Caller is expected to free it later.
 */

Mediator *CreateMediator(GameObject *object)
{
	Mediator *mediator = (Mediator *)malloc(sizeof(Mediator));
	mediator->object = object;
	return mediator;
}

/**
 * MediatorHandleCommand : Sends a command off to the GameObject for handling.
 *
 * Acts as the go between for player input (or the AI) and the GameObject,
 * keeping the whole show running without descending into pure chaos.
 *
 * The mediator doesn't decide what a command means .... ExecuteCommand() turns
 * it into events, and the FSM's transition tables decide what happens next,
 * so you don't have to micromanage every button press like an overworked
 * pub manager on a Friday night.
 *
 * @mediator: The mediator minding the GameObject.
 * @command:  The command flags the player has kindly mashed together (big question can it be executed)
 * @deltaTime: Time since last frame, for anything needing it.
 */

void MediatorHandleCommand(Mediator *mediator, Command command, float deltaTime)
{
	if (!mediator || !mediator->object)
	{
		printf("Error: Mediator or Mediator's object is NULL\n");
		return;
	}

	// GameObject acts on the command (via the Command System, which sends FSM events)
	ExecuteCommand(command, mediator->object, deltaTime);
}

/**
 * DeleteMediator - Frees a mediator from memory.
 *
 * Sends the mediator off to the great heap spirit in the sky.
 * Does absolutely nothing if you hand it NULL ... very forgiving.
 * It does not free the GameObject: the mediator only minds it, it doesn't own it.
 *
 * @mediator: Pointer to the Mediator to free.
 */

void DeleteMediator(Mediator *mediator)
{
	// Free Mediator (free(NULL) is safe, so no check needed)
	free(mediator);

	// Note: mediator = NULL here would only change this local copy of the pointer.
	// The caller sets its own pointer to NULL.
}
