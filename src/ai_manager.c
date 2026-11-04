#include <stdlib.h>
#include <time.h>

#include "./command/command.h"
#include "./utils/ai_manager.h"

/**
 * InitAIManager : Sets up the AI manager....or at least pretends to. It does 
 * not a thing at the moment.
 *
 * Here for when you eventually give your NPCs a few more brain cells 
 * to rub together.
 *
 * Add proper setup later if the AI gets notions.
 */

void InitAIManager()
{
	// Initialize AI
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
 * @return: A random Command somewhere between 0 and COMMAND_COUNT-1.
 */

Command PollAI()
{
	// Return a random command
	static Command commands[] = {
		NONE,
		MOVE_UP,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT,
		ATTACK,
		DEFEND};

	int count = sizeof(commands) / sizeof(commands[0]);
	int index = rand() % count;

	return commands[index];
}

/**
 * ExitAIManager : Cleans up after the AI manager... in theory.
 *
 * There's nothing to tidy up yet, but this is the spot for it
 * once the AI starts doing more than guessing like a toddler.
 *
 * Add proper cleanup here when the AI grows up a bit.
 */

void ExitAIManager()
{
	// Cleanup resources if needed : Currently a placeholder.
	// Add any necessary cleanup logic here (e.g., freeing memory, closing files).
}
