#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "command/command.h"

// Init Input Manager
void InitInputManager();

// Poll Input
Command PollInput();

// Cleanup
void DeleteInputManager();

#endif // INPUT_MANAGER_H