#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "command/command.h"

// Init Input Manager
void InitInputManager();

// Poll Input
Command PollInput();

// Restart pressed? (R, or Start on a gamepad) Only used at game over
bool PollRestart();

// Cleanup
void DeleteInputManager();

#endif // INPUT_MANAGER_H