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

	// Let the Player and NPC know who to report to
	player->mediator = mediator;
	npc->mediator = mediator;

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
 * MediatorNotify : Something happened in the game, and the mediator decides who gets told.
 *
 * Code only REPORTS what happened:
 *
 *     MediatorNotify(object->mediator, GAME_EVENT_HIT, attacker, target);
 *
 * and this one function decides who hears about it. collision.c doesn't need
 * to know there's such a thing as sound, or a HUD. Adding screen shake on a
 * hit is one new line in here, not a search through the whole project.
 *
 * Who reports what:
 *     game.c              LEVEL_START, GAME_OVER
 *     player.c / npc.c    SWIPE (when a swipe starts)
 *     collision.c         HIT, BLOCK, MISS (when the blade comes round)
 *
 * @mediator: The game's mediator.
 * @event:    What happened (GAME_EVENT_HIT, GAME_EVENT_BLOCK ...).
 * @source:   Who did it (the attacker, the winner ...), or NULL if nobody.
 * @target:   Who it was done to, or NULL if nobody.
 */

void MediatorNotify(Mediator *mediator, GameEvent event, GameObject *source, GameObject *target)
{
	if (!mediator)
		return;

	switch (event)
	{
	case GAME_EVENT_LEVEL_START:
		PlayActionSound(SOUND_LEVEL_START);
		break;

	case GAME_EVENT_SWIPE:
		PlayActionSound(SOUND_SWIPE);
		// Later: a whoosh trail behind the sword, a stamina cost on the HUD ...
		break;

	case GAME_EVENT_HIT:
		TraceLog(LOG_INFO, "[%s] hit [%s] for %d, health %d", source->name, target->name, DAMAGE_DEFAULT, target->health);
		PlayActionSound(SOUND_HIT);
		// Later (yours to write):
		//     HudFlash(mediator->hud, RED);
		//     ShakeCamera(0.2f);
		break;

	case GAME_EVENT_BLOCK:
		TraceLog(LOG_INFO, "[%s] blocked [%s]'s sword", target->name, source->name);
		PlayActionSound(SOUND_BLOCK);
		// Later: sparks where the sword meets the shield ...
		break;

	case GAME_EVENT_MISS:
		// Nobody needs telling yet: just a quiet note in the debug log.
		// Later: a "whiff" sound, or the AI learning you are out of reach ...
		TraceLog(LOG_DEBUG, "[%s] missed [%s]", source->name, target->name);
		break;

	case GAME_EVENT_GAME_OVER:
		// source is the winner, or NULL for a draw (both down on the same frame)
		if (source)
			TraceLog(LOG_INFO, "[Game] Over: [%s] won", source->name);
		else
			TraceLog(LOG_INFO, "[Game] Over: a draw");

		PlayActionSound(SOUND_GAMEOVER);
		HudGameOver(mediator->hud, source ? source->name : NULL); // NULL shows "Nobody wins!"
		break;

	default:
		break;
	}
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
