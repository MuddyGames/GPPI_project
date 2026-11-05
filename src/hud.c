#include "hud/hud.h"
#include "utils/constants.h"

/**
 * InitHud : Gets the HUD ready before the craic starts.
 *
 * Clears the last command and the state name, so a restart
 * doesn't carry over last game's glory.
 *
 * @hud: The HUD to set up.
 */

void InitHud(Hud *hud)
{
	hud->lastCommand = NONE;
	hud->stateName = "";
}

/**
 * UpdateHud : Takes note of this frame's command and the player's state.
 *
 * The HUD knows nothing about the player. The mediator passes it the
 * command and the state's name, like a commentator who can't see the
 * pitch but has a lad on the phone telling him what's going on.
 *
 * @hud:       The HUD to update.
 * @command:   This frame's command bits.
 * @stateName: The name of the state the player is now in.
 */

void UpdateHud(Hud *hud, Command command, const char *stateName)
{
	hud->lastCommand = command;
	hud->stateName = stateName ? stateName : "";
}

// Helper function: draw a command's name if its bit is set, then move down a line
static void DrawActiveCommand(Command activeCommand, Command filter, const char *commandText, int *yPositionOffset, Color color)
{
	if (IsCommandActive(activeCommand, filter))
	{
		DrawText(commandText, 10, *yPositionOffset, DEFAULT_FONT_SIZE, color);
		*yPositionOffset += DEFAULT_TEXT_Y_OFFSET;
	}
}

/**
 * DrawHud : Puts the HUD up in the top corner.
 *
 * Starts below the FPS counter, in light colours so it shows up on the grass.
 *
 * @hud: The HUD to draw.
 */

void DrawHud(const Hud *hud)
{
	int y = 45; // Below DrawFPS

	// Command as bits, e.g. MOVE_LEFT | ATTACK -> 010100
	char bits[COMMAND_COUNT + 1];
	GetCommandBits(hud->lastCommand, bits);
	DrawText(TextFormat("Command: %s", bits), 10, y, DEFAULT_FONT_SIZE, RAYWHITE);
	y += DEFAULT_TEXT_Y_OFFSET;

	// Player's current state, e.g. Player_Attacking
	DrawText(TextFormat("State: %s", hud->stateName), 10, y, DEFAULT_FONT_SIZE, RAYWHITE);
	y += DEFAULT_TEXT_Y_OFFSET;

	// Names of the active commands
	DrawActiveCommand(hud->lastCommand, MOVE_UP, "MOVE_UP", &y, SKYBLUE);
	DrawActiveCommand(hud->lastCommand, MOVE_DOWN, "MOVE_DOWN", &y, SKYBLUE);
	DrawActiveCommand(hud->lastCommand, MOVE_LEFT, "MOVE_LEFT", &y, SKYBLUE);
	DrawActiveCommand(hud->lastCommand, MOVE_RIGHT, "MOVE_RIGHT", &y, SKYBLUE);
	DrawActiveCommand(hud->lastCommand, ATTACK, "ATTACK", &y, ORANGE);
	DrawActiveCommand(hud->lastCommand, DEFEND, "DEFEND", &y, YELLOW);
}
