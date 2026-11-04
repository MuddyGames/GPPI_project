#include <stdbool.h>

#include "./command/command.h"
#include "./fsm/fsm.h"
#include "./utils/mediator.h"

/**
 * IsCommandActive : Checks if a particular command flag is set.
 *
 * Handy for seeing if a button is actually pressed,
 * or if you're just imagining things.
 *
 * Returns true if the given filter bit is active in the command,
 * false if it's as useful as a ashtray one a motorcycle.
 *
 * @command: The full set of combined command flags.
 * @filter:  The specific flag you're nosing for.
 */

bool IsCommandActive(Command command, Command filter)
{
	return (command & filter) != 0;
}

/**
 * GetCommandBits : Converts a Command bitmask into a grand old string of 1s and 0s.
 *
 * Useful for debugging when you've no clue what the player is actually pressing
 * and you'd rather not guess like a gobshite.
 *
 * Each bit becomes:
 *     '1' = the command is active
 *     '0' = the command is doing absolutely nothing
 *
 * Make sure the buffer is at least (COMMAND_COUNT + 1), or you'll have
 * worse problems than a few wrong key presses.
 *
 * Example:
 *     MOVE_UP | ATTACK  ->  "010001"
 *
 * @command: The command flags to inspect.
 * @buffer:  Where the binary craic ends up.
 */

void GetCommandBits(Command command, char *buffer)
{
	for (int i = COMMAND_COUNT; i >= 0; i--)
	{
		buffer[COMMAND_COUNT - 1 - i] = (command & (1 << i)) ? '1' : '0';
	}
	buffer[COMMAND_COUNT] = '\0'; // null terminator
}
