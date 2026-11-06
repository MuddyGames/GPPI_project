#include <stdbool.h>

#include "command/command.h"
#include "fsm/fsm.h"
#include "utils/utils.h"


// Add command names to the buffer, separated by " | " if multiple commands are active
	// Add command names if new commands are added to the Command enum, update this array accordingly
	static const char *COMMAND_NAMES[COMMAND_COUNT] = {
		"MOVE_UP", "MOVE_DOWN", "MOVE_LEFT", "MOVE_RIGHT", "ATTACK", "DEFEND"};

/**
 * ExecuteCommand : Turns the command bits into events for the FSM.
 *
 * Works out which way the poor lad wants to go, then fires the right
 * events at the FSM. The FSM's transition tables decide what actually
 * happens next .... this fella just delivers the message.
 *
 * inputAxis is set every frame (zero when nothing is pressed), so nobody
 * slides off across the field on last week's input. The facing direction
 * is set when moving, but not in the middle of a sword swipe.
 *
 * @command:   The command flags the player (or the AI) has mashed together.
 * @object:    The GameObject getting the events.
 * @deltaTime: Time since last frame, for anything needing it.
 */

void ExecuteCommand(Command command, GameObject *object, float deltaTime)
{
	// Movement (multiple keys)
	Vector2 axis = (Vector2){0.0f, 0.0f};

	if (IsCommandActive(command, MOVE_UP))
		axis = Vector2Add(axis, (Vector2){0.0f, -1.0f});

	if (IsCommandActive(command, MOVE_DOWN))
		axis = Vector2Add(axis, (Vector2){0.0f, 1.0f});

	if (IsCommandActive(command, MOVE_LEFT))
		axis = Vector2Add(axis, (Vector2){-1.0f, 0.0f});

	if (IsCommandActive(command, MOVE_RIGHT))
		axis = Vector2Add(axis, (Vector2){1.0f, 0.0f});

	// Check if we are moving
	bool moving = Vector2Length(axis) > 0.0f;
	bool attacking = IsCommandActive(command, ATTACK);
	bool defending = IsCommandActive(command, DEFEND);

	// Always set the input axis: zero when not moving
	if (moving)
		axis = Vector2Normalize(axis);
	object->inputAxis = axis;

	// Update the facing direction, except in the middle of a sword swipe:
	// a swipe keeps the direction it started in (direction keys are ignored until it finishes)
	if (moving && object->currentState != STATE_ATTACKING)
	{
		object->previousDirection = object->currentDirection;
		object->currentDirection = DirectionAxis(axis);
	}

	// EVENT_MOVE only if not EVENT_ATTACK
	if (!attacking && moving)
	{
		HandleEvent(object, EVENT_MOVE, deltaTime);
	}
	// Send none if not MOVING or Performing an ACTION
	else if (!moving && !attacking && !defending)
	{
		HandleEvent(object, EVENT_NONE, deltaTime);
	}

	// ATTACK / DEFEND Send last
	if (attacking)
		HandleEvent(object, EVENT_ATTACK, deltaTime);

	if (defending)
		HandleEvent(object, EVENT_DEFEND, deltaTime);
}

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
 * Bits are printed from the highest (COMMAND_COUNT - 1) down to bit 0.
 *
 * @command: The command flags to inspect.
 * @buffer:  Where the binary craic ends up.
 */

void GetCommandBits(Command command, char *buffer)
{
	for (int i = COMMAND_COUNT - 1; i >= 0; i--)
	{
		buffer[COMMAND_COUNT - 1 - i] = (command & (1 << i)) ? '1' : '0';
	}
	buffer[COMMAND_COUNT] = '\0'; // null terminator
}

/**
 * GetCommandName : Returns the name of a command as a string.
 *
 * Useful for debugging when you want to see what command is currently active.
 *
 * @command: The command flag to inspect.
 * @buffer:  Where the command name ends up.
 * @size:    The size of the buffer to avoid overflow.
 */

void GetCommandName(Command command, char *buffer, int size)
{
	int used = snprintf(buffer, size, "%s", command == NONE ? "NONE" : "");

	for (int i = 0; i < COMMAND_COUNT && used < size; i++)
	{
		if (IsCommandActive(command, (Command)(1 << i)))
			used += snprintf(buffer + used, size - used, "%s%s", used > 0 ? " | " : "", COMMAND_NAMES[i]);
	}
}
