#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/mediator.h"

// For a grand explanation on Mediator Pattern see
// https://www.geeksforgeeks.org/mediator-design-pattern/

/**
 * CreateMediator: Builds the game's one and only mediator to mind Game.
 *
 * Mediator is the only part of the game that knows about everyone:
 * player, NPC and HUD. They don't know about each other, so
 * anything that needs passing between them comes through here. Think of it
 * as the barman who knows every regular .... they don't talk to each other,
 * they talk through him.
 *
 * @player: The player.
 * @npc:    The NPC.
 * @hud:    The HUD.
 *
 * @return: Pointer to an allocated Mediator. Caller is expected to free it later.
 */

Mediator *CreateMediator(GameObject *player, GameObject *npc, Hud *hud)
{
	Mediator *mediator = (Mediator *)malloc(sizeof(Mediator));
	if (!mediator)
	{
		TraceLog(LOG_ERROR, "Failed to allocate mediator");
		exit(1);
	}

	mediator->player = player;
	mediator->npc = npc;
	mediator->hud = hud;
	return mediator;
}

/**
 * MediatorReceiveCommand : Command for an object arrives, and gets passed on.
 *
 * Player's commands come from the input manager, NPC's from the AI.
 * Both arrive here the same way; the only difference is which object they're for.
 *
 * The mediator doesn't decide what a command means .... ExecuteCommand() turns
 * it into events, and the FSM's transition tables decide what happens next,
 * so you don't have to micromanage every button press like an overworked
 * pub manager on a Friday night.
 *
 * Renamed from MediatorHandleCommand() in the command practical: the mediator
 * receives the command and sends it on, the FSM does the handling.
 *
 * @mediator:  The game's mediator.
 * @object:    Who the command is for (the player or the NPC).
 * @command:   The command flags (big question: can it be executed?)
 * @deltaTime: Time since last frame, for anything needing it.
 */

void MediatorReceiveCommand(Mediator *mediator, GameObject *object, Command command, float deltaTime)
{
	if (!mediator || !object)
	{
		TraceLog(LOG_ERROR, "Mediator or object is NULL");
		return;
	}

	// 1. The object acts on the command (via the Command System, which sends FSM events)
	ExecuteCommand(command, object, deltaTime);

	// 2. The HUD shows the player's command and state, like the HUD in any game
	//    shows YOUR stuff, not the enemy's.
	//
	//    STRETCH TASK: an F1 debug overlay that shows the NPC's command and state
	//    too, so you can see what the AI is thinking. This is where you'd send it.
	if (object == mediator->player && mediator->hud)
	{
		UpdateHud(mediator->hud, command, object->stateConfigs[object->currentState].name);
	}
}

/**
 * MediatorNotify : Something happened in game, and mediator decides who gets told.
 *
 * STUB: nothing calls this yet, and each case below is mostly comments.
 * It's here to show where the mediator earns its keep.
 *
 * Right now "who gets told" is spread around the code:
 *     collision.c         plays the hit and block sounds
 *     player.c and npc.c  play the swipe sound
 *     game.c              plays the game-over sound and tells the HUD who won
 *
 * Every time you add an effect (screen shake on a hit, a spark on a block),
 * you are hunting through files for every place that needs to know.
 *
 * With the mediator, code only REPORTS what happened:
 *
 *     MediatorNotify(data->mediator, GAME_EVENT_HIT, attacker, target);
 *
 * and this one function decides who hears about it. collision.c doesn't need
 * to know there's such a thing as sound, or a HUD. Adding screen shake on a
 * hit is one new line in here, not a search through the whole project.
 *
 * @mediator: Game mediator.
 * @event:    What happened (GAME_EVENT_HIT, GAME_EVENT_BLOCK ...).
 * @source:   Who did it (the attacker, the winner ...).
 * @target:   Who it was done to, or NULL if nobody.
 */

void MediatorNotify(Mediator *mediator, GameEvent event, GameObject *source, GameObject *target)
{
	if (!mediator)
		return;

	switch (event)
	{
	case GAME_EVENT_SWIPE:
		// Today: player.c and npc.c each call PlayActionSound(SOUND_SWIPE) when the
		// swipe starts. Report it instead, and play the sound here:
		//
		//     PlayActionSound(SOUND_SWIPE);
		//
		// Later: a whoosh trail behind the sword, a stamina cost on the HUD ...
		break;

	case GAME_EVENT_HIT:
		// Today: collision.c calls PlayActionSound(SOUND_HIT) inside AttackHit().
		// One way to move it: have AttackHit() return what happened (hit, block
		// or miss), then game.c reports it:
		//
		//     MediatorNotify(data->mediator, GAME_EVENT_HIT, attacker, target);
		//
		// and the reactions all live here:
		//
		//     PlayActionSound(SOUND_HIT);
		//     HudFlash(mediator->hud, RED);   // something for you to write
		//     ShakeCamera(0.2f);              // and this
		break;

	case GAME_EVENT_BLOCK:
		// Today: collision.c calls PlayActionSound(SOUND_BLOCK).
		//
		//     PlayActionSound(SOUND_BLOCK);
		//
		// Later: sparks where the sword meets the shield ...
		break;

	case GAME_EVENT_GAME_OVER:
		// Today: game.c plays the sound and tells the HUD who won, itself.
		// With the mediator, game.c just reports the winner:
		//
		//     MediatorNotify(data->mediator, GAME_EVENT_GAME_OVER, winner, NULL);
		//
		// and in here:
		//
		//     PlayActionSound(SOUND_GAMEOVER);
		//     HudGameOver(mediator->hud, source->name);
		break;

	default:
		break;
	}

	// Stop the compiler moaning about unused parameters while this is a stub.
	// Delete these once the cases above use them.
	(void)source;
	(void)target;
}

/**
 * DeleteMediator - Frees the mediator from memory.
 *
 * Sends the mediator off to the great heap spirit in the sky.
 * Does absolutely nothing if you hand it NULL ... very forgiving.
 * It does not free player, NPC or HUD: the mediator only
 * knows them, it doesn't own them.
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
