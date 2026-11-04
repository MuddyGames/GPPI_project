#ifndef AI_MANAGER_H
#define AI_MANAGER_H

#include "command/command.h"

// Init AI Manager
void InitAIManager();

// Poll AI: the NPC's version of PollInput(). Decides once a second
Command PollAI();

// Cleanup
void DeleteAIManager();

#endif // AI_MANAGER_H