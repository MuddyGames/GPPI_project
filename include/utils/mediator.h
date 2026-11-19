#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <stdbool.h>

#include "events/game_event.h"
#include "command/command.h"
#include "gameobjects/gameobject.h"
#include "hud/hud.h"
#include "utils/audio_manager.h"

// One Mediator for the whole game.
// It knows about everyone: Player, NPC,  HUD (and audio).
// Player, NPC,  HUD (and audio) don't know about each other: anything that needs passing between
// them goes through here.
struct Mediator
{
	GameObject *player; // The player (the HUD shows what this one is doing)
	GameObject *npc;	// The NPC
	Hud *hud;			// The HUD
};


// Create the game's mediator
Mediator *CreateMediator(GameObject *player, GameObject *npc, Hud *hud);

// Something happened in the game: the mediator decides who gets told.
// target can be NULL when there isn't one (e.g. a swipe that hits nothing).
// STUB: the cases in mediator.c only show, in comments, what could go there.
// Unused arguments? set them to NULL or 0.0f as appropriate
void MediatorNotify(Mediator *mediator, GameEvent event, Command command, GameObject *source, GameObject *target, float deltaTime);

// Mediator Cleanup: frees what CreateMediator() allocated
void DeleteMediator(Mediator *mediator);

#endif // MEDIATOR_H
