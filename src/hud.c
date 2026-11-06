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

// Helper function: draw text with a dark outline so it reads on any background
static void DrawTextOutlined(const char *text, int x, int y, int fontSize, Color color)
{
	const int t = HUD_TEXT_OUTLINE;
	for (int dx = -t; dx <= t; dx += t)
		for (int dy = -t; dy <= t; dy += t)
			if (dx != 0 || dy != 0)
				DrawText(text, x + dx, y + dy, fontSize, BLACK);

	DrawText(text, x, y, fontSize, color);
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
	int y = 20; // Top of the HUD

	// FPS: green is grand, orange is struggling, red it is in bits
	int fps = GetFPS();
	Color fpsColor = (fps < 15) ? RED : (fps < 30) ? ORANGE
												   : LIME;
	const char *fpsLine = TextFormat("%2i FPS", fps);

	// Command as bits, e.g. MOVE_LEFT | ATTACK -> 010100
	char bits[COMMAND_COUNT + 1];
	GetCommandBits(hud->lastCommand, bits);
	const char *commandLine = TextFormat("Command: %s", bits);
	const char *stateLine = TextFormat("State: %s", hud->stateName); // TextFormat keeps 4 buffers, so 3 at once is grand

	// Size the panel: three lines, plus one for each active command
	int lines = 3;
	for (int bit = 0; bit < COMMAND_COUNT; bit++)
		if (IsCommandActive(hud->lastCommand, (Command)(1 << bit)))
			lines++;

	// Widest line sets the width
	int width = MeasureText(commandLine, DEFAULT_FONT_SIZE);
	int stateWidth = MeasureText(stateLine, DEFAULT_FONT_SIZE);
	if (stateWidth > width)
		width = stateWidth;

	Rectangle panel = {
		10 - HUD_PADDING,
		(float)(y - HUD_PADDING),
		(float)(width + 2 * HUD_PADDING),
		(float)((lines - 1) * DEFAULT_TEXT_Y_OFFSET + DEFAULT_FONT_SIZE + 2 * HUD_PADDING)};

	// Panel first, so the text is drawn on top of it
	DrawRectangleRounded(panel, 0.15f, 8, Fade(BLACK, HUD_PANEL_ALPHA));
	DrawRectangleRoundedLinesEx(panel, 0.15f, 8, 2.0f, Fade(RAYWHITE, 0.6f));

	// Frames per second
	DrawTextOutlined(fpsLine, 10, y, DEFAULT_FONT_SIZE, fpsColor);
	y += DEFAULT_TEXT_Y_OFFSET;

	// Draw the command and state lines, then each active command
	DrawTextOutlined(commandLine, 10, y, DEFAULT_FONT_SIZE, RAYWHITE);
	y += DEFAULT_TEXT_Y_OFFSET;

	// Player's current state, e.g. Player_Attacking
	DrawTextOutlined(stateLine, 10, y, DEFAULT_FONT_SIZE, RAYWHITE);
	y += DEFAULT_TEXT_Y_OFFSET;

	// Names of the active commands
	DrawActiveCommand(hud->lastCommand, MOVE_UP, "MOVE_UP", &y, SKYBLUE);
	DrawActiveCommand(hud->lastCommand, MOVE_DOWN, "MOVE_DOWN", &y, SKYBLUE);
	DrawActiveCommand(hud->lastCommand, MOVE_LEFT, "MOVE_LEFT", &y, SKYBLUE);
	DrawActiveCommand(hud->lastCommand, MOVE_RIGHT, "MOVE_RIGHT", &y, SKYBLUE);
	DrawActiveCommand(hud->lastCommand, ATTACK, "ATTACK", &y, ORANGE);
	DrawActiveCommand(hud->lastCommand, DEFEND, "DEFEND", &y, YELLOW);
}