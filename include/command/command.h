#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

// Define the Command enum
typedef enum
{
	NONE 				= 0,		// No command (used to represent idle state)
	MOVE_UP 			= 1 << 0,	// Binary: 000001 Command to move up
	MOVE_DOWN 			= 1 << 1,	// Binary: 000010 Command to move down
	MOVE_LEFT 			= 1 << 2,	// Binary: 000100 Command to move left
	MOVE_RIGHT 			= 1 << 3,	// Binary: 001000 Command to move right
	ATTACK				= 1 << 4,	// Binary: 010000 Command to perform an attack action (e.g., slash with sword)
	DEFEND				= 1 << 5,	// Binary: 100000 Command to perform an defend action (e.g., defend with sheild)
	COMMAND_COUNT 		= 6			// Total number of commands, useful for looping or limits
} Command;

// Function to show active command bits (for debugging)
void GetCommandBits(Command cmd, char *buffer);

// Check if a specific command is active
bool IsCommandActive(Command command, Command filter);

#endif // COMMAND_H