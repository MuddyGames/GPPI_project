#ifndef HUD_H
#define HUD_H

#include <raylib.h>

#include "command/command.h"

// HUD (Heads-Up Display)
// Shows which commands are active and which state the player is in.
// It reacts to commands, but knows nothing about the player:
// the mediator passes it the state's name as plain text.
typedef struct
{
	Command lastCommand;   // Command from the most recent frame
	const char *stateName; // Name of the player's current state, e.g. "Player_Attacking"
} Hud;

// Set the HUD to its starting state
void InitHud(Hud *hud);

// React to this frame's command and state
void UpdateHud(Hud *hud, Command command, const char *stateName);

// Draw the HUD on screen
void DrawHud(const Hud *hud);

#endif // HUD_H
