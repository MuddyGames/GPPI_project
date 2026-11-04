#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "./utils/mediator.h"

#include "./utils/input_manager.h"
#include "./utils/ai_manager.h"

#include "./utils/utils.h"

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
 * MediatorHandleCommand : Sends a command off to the FSM for handling.
 *
 * Acts as the go between for player input and the FSM, keeping the whole
 * show running without descending into pure chaos.
 *
 * Mediator figures out what "command" actually means and fires the
 * right event at the FSM, so you don't have to micromanage every button press
 * like an overworked pub manager on a Friday night.
 *
 * @mediator: The mediator minding the GameObject.
 * @command:  The command flags the player has kindly mashed together (big question can it be executed)
 * @deltaTime: Time since last frame, for anything needing it.
 *
 * This function simply forwards Commands for the craic; the real brains are in the
 * event handlers and state transitions further down the line.
 */

void MediatorHandleCommand(Mediator *mediator, Command command, float deltaTime)
{
	if (!mediator || !mediator->object)
	{
		printf("Error: Mediator or Mediator's object is NULL\n");
		return;
	}

	GameObject *object = mediator->object;

	// Movement (multiple keys)
	Vector2 axis = (Vector2){0.0f, 0.0f};

	if (IsCommandActive(command, MOVE_UP))
		axis = Vector2Add(axis, (Vector2){0.0f, -1.0f});

	if (IsCommandActive(command, MOVE_DOWN))
		axis = Vector2Add(axis, (Vector2){0.0f, 1.0f});

	if (IsCommandActive(command, MOVE_LEFT))
		axis = Vector2Add(axis, (Vector2){-1.0f, 0.0f});

	if (IsCommandActive(command, MOVE_RIGHT))
		axis = Vector2Add(axis, (Vector2){1.0f, 0.0f});

	// Check if we are moving
	bool moving = Vector2Length(axis) > 0.0f;
	bool attacking = IsCommandActive(command, ATTACK);
	bool defending = IsCommandActive(command, DEFEND);

	// EVENT_MOVE only of not EVENT_ATTACK
	if (!attacking && moving)
	{
		axis = Vector2Normalize(axis);
		object->inputAxis = axis;
		object->previousDirection = object->currentDirection;
		object->currentDirection = DirectionAxis(axis);

		HandleEvent(object, EVENT_MOVE, deltaTime);
	}
	// Send none if not MOVING or Performing and ACTION
	else if (!moving && !attacking && !defending)
	{
		HandleEvent(object, EVENT_NONE, deltaTime);
	}

	// ATTACK / DEFEND Send last
	if (attacking)
		HandleEvent(object, EVENT_ATTACK, deltaTime);

	if (defending)
		HandleEvent(object, EVENT_DEFEND, deltaTime);
}

/**
 * MediatorUpdatePlayer : Gives the player a fair hearing.
 *
 * Polls input system to see what buttons the player is mashing,
 * then politely hands those intentions to the mediator, who translates 
 * them into something the FSM won't throw a tantrum over.
 * Think of it as: "Player shouts, Mediator interprets, FSM nods knowingly".
 * 
 * @mediator: Pointer to the Mediator
 * @deltaTime: Time since last frame, for anything needing it.
 */

void MediatorUpdatePlayer(Mediator *mediator, float deltaTime)
{
	Command command = PollInput();
	MediatorHandleCommand(mediator, command, deltaTime);
}

/**
 * MediatorUpdateNPC : Lets the NPC have its small moment of glory.
 * Checks in with the so-called "AI" once in a while
 * (about as intelligent as a rotten potatoe, but sure look), and 
 * whatever nonsense command it comes up with, the mediator dutifully 
 * passes along to the FSM.
 * 
 * Uses a timer so the NPC doesn't panic and fire commands like a 
 * Junior B Hurling Manager at a county final match. Ahh Ref!!!
 * 
 * @mediator: Pointer to the Mediator
 * @deltaTime: Time since last frame, for anything that needs a rolex.
 */

void MediatorUpdateNPC(Mediator *mediator, float deltaTime)
{
	// Simple random behavior for NPC AI (not truly an AI, just random selection)
	// Static variable to track the last AI action time
	static float lastAITime = 0.0f;

	// Check if 1 second has passed since the last AI action
	if (GetTime() - lastAITime >= 1.0f)
	{
		// Poll and execute random commands for the NPC (simulate AI actions)
		printf("\n#######################################\n");
		printf("\t%s Handle AI Events", mediator->object->name);
		printf("\n#######################################\n");

		// Randomly select a command for the NPC
		Command command = PollAI();
		MediatorHandleCommand(mediator, command, deltaTime); // Execute the command via the mediator

		// Update the last AI execution time
		lastAITime = GetTime();
	}
}

/**
 * DeleteMediator - Frees a mediator from memory.
 *
 * Sends the mediator off to the great heap spirit in the sky.
 * Does absolutely nothing if you hand it NULL ... very forgiving.
 *
 * @mediator: Pointer to the Mediator to free.
 */

void DeleteMediator(Mediator *mediator)
{
	// Free Mediator
	if (mediator)
	{
		free(mediator);
		mediator = NULL;
	}
}
