#ifndef GAME_EVENT_H
#define GAME_EVENT_H

// ---------------------------------------------------------------------------
// Game events: news for the rest of the game
//
// This is not the same as the FSM's Event (EVENT_MOVE, EVENT_DIE ...). 
// Those events drive ONE object's state machine. 
// These are things that happened in the game that
// several other parts might care about: HUD, audio, effects, ...
//
//   Command is "please do this": it goes IN, and the FSM can refuse it.
//   Game event is "this happened": it goes OUT, and it's already happened.
// ---------------------------------------------------------------------------
typedef enum
{
    GAME_EVENT_LEVEL_START, // Level has started, so reset the HUD and audio (source is NULL)
    GAME_EVENT_COMMAND,     // A Command arrived              (source is the player or NPC)
    GAME_EVENT_SWIPE,       // A sword swipe started          (source swung)
    GAME_EVENT_HIT,         // A sword swipe landed           (source hit target)
    GAME_EVENT_BLOCK,       // A shield stopped a sword       (target blocked source)
    GAME_EVENT_MISS,        // A sword swipe missed           (source swung, target dodged)
    GAME_EVENT_GAME_OVER,   // Someone has won                (source is the winner)
    GAME_EVENT_COUNT        // Total number of game events (for counting purposes)
} GameEvent;

#endif // GAME_EVENT_H