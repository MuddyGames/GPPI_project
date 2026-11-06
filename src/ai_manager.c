#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <raylib.h>

#include "command/command.h"
#include "utils/ai_manager.h"

/**
 * InitAIManager : Sets up the AI manager....or at least pretends to. All it
 * does at the moment is shake the dice, so the NPC isn't the same eejit every run.
 *
 * Here for when you eventually give your NPCs a few more brain cells
 * to rub together.
 *
 * Add proper setup later if the AI gets notions.
 */

void InitAIManager()
{
	// Seed the random number generator so the AI is different every run
	srand((unsigned int)time(NULL));
}

/**
 * PollAI : Gets a random command from the AI, sure why not.
 *
 * This fella doesn't think too hard...he just picks a command at random
 * like someone closing their eyes and throwing darts at a pub wall.
 *
 * Handy for testing or for NPCs that aren't exactly the sharpest tools
 * in the shed (yet).
 *
 * It only makes up its mind once a second, then holds that command,
 * like a player holding a key down. Otherwise it would change its mind
 * 60 times a second, like a Junior B Hurling Manager at a county final. Ahh Ref!!!
 *
 * Note: the timer and the decision are static, so every NPC shares them.
 * Grand for one NPC. For more, keep them in the NPC struct instead.
 *
 * @return: A random Command from the list below.
 */

Command PollAI()
{
	// The current decision, and when it was made
	static Command command = NONE;
	static double lastDecisionTime = -1.0;

	// Keep the same command until a second has passed
	if (lastDecisionTime >= 0.0 && GetTime() - lastDecisionTime < 1.0)
		return command;

	// Pick a random command
	// No ATTACK here: the NPC swings when it catches the player (EVENT_COLLISION_START)
	// Any MOVE means "chase" (EVENT_MOVE); the NPC works out the direction itself
	static Command commands[] = {
		NONE,
		MOVE_UP,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT,
		DEFEND};

	int count = sizeof(commands) / sizeof(commands[0]);
	int index = rand() % count;

	command = commands[index];
	lastDecisionTime = GetTime();

	// Log each new decision (once a second, not every frame)
	char bits[COMMAND_COUNT + 1];
	GetCommandBits(command, bits);
	TraceLog(LOG_INFO, "AI Command Bits: %s (%d)", bits, command);

	return command;
}

/**
 * DeleteAIManager : Cleans up after the AI manager... in theory.
 *
 * There's nothing to tidy up yet, but this is the spot for it
 * once the AI starts doing more than guessing like a toddler.
 *
 * Add proper cleanup here when the AI grows up a bit.
 */

void DeleteAIManager()
{
	// Cleanup resources if needed : Currently a placeholder.
	// Add any necessary cleanup logic here (e.g., freeing memory, closing files).
}
