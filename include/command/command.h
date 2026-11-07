#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

#include "gameobjects/gameobject.h"

// Define the Command enum
typedef enum
{
	NONE = 0,			 // No command (used to represent idle state)
	MOVE_UP = 1 << 0,	 // Binary: 000001 Command to move up
	MOVE_DOWN = 1 << 1,	 // Binary: 000010 Command to move down
	MOVE_LEFT = 1 << 2,	 // Binary: 000100 Command to move left
	MOVE_RIGHT = 1 << 3, // Binary: 001000 Command to move right
	ATTACK = 1 << 4,	 // Binary: 010000 Command to perform an attack action (e.g., slash with sword)
	DEFEND = 1 << 5,	 // Binary: 100000 Command to perform a defend action (e.g., defend with shield)
	COMMAND_COUNT = 6	 // Total number of commands, useful for looping or limits
} Command;

// Function to execute a command: turns the command bits into FSM events for the game object
void ExecuteCommand(Command command, GameObject *object, float deltaTime);

// Function to show active command bits (for debugging)
void GetCommandBits(Command command, char *buffer);

// Function to show the name of a command (for debugging)
void GetCommandNames(Command command, char *buffer, int size);

// Check if a specific command is active
bool IsCommandActive(Command command, Command filter);

#endif // COMMAND_H