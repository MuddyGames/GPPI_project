#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <stdbool.h>

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

// ---------------------------------------------------------------------------
// Game events: news for the rest of the game
//
// Not the same as the FSM's Event (EVENT_MOVE, EVENT_DIE ...). Those drive ONE
// object's state machine. These are things that happened in the game that
// several other parts might care about: HUD, audio, effects, ...
//
//   Command is "please do this": it goes IN, and the FSM can refuse it.
//   Game event is "this happened": it goes OUT, and it's already happened.
// ---------------------------------------------------------------------------
typedef enum
{
	GAME_EVENT_LEVEL_START, // Level has started, so reset the HUD and audio (source is NULL)
	GAME_EVENT_SWIPE,		// A sword swipe started          (source swung)
	GAME_EVENT_HIT,			// A sword swipe landed           (source hit target)
	GAME_EVENT_BLOCK,		// A shield stopped a sword       (target blocked source)
	GAME_EVENT_MISS,		// A sword swipe missed           (source swung, target dodged)
	GAME_EVENT_GAME_OVER,	// Someone has won                (source is the winner)
	GAME_EVENT_COUNT		// Total number of game events (for counting purposes)
} GameEvent;

// Create the game's mediator
Mediator *CreateMediator(GameObject *player, GameObject *npc, Hud *hud);

// Something happened in the game: the mediator decides who gets told.
// target can be NULL when there isn't one (e.g. a swipe that hits nothing).
// STUB: the cases in mediator.c only show, in comments, what could go there.
// Unused arguments? set them to NULL or 0.0f as appropriate
void MediatorNotifyAll(Mediator *mediator, GameEvent event, Command command, GameObject *source, GameObject *target, float deltaTime);

// Mediator Cleanup: frees what CreateMediator() allocated
void DeleteMediator(Mediator *mediator);

#endif // MEDIATOR_H
