#ifndef AI_MANAGER_H
#define AI_MANAGER_H

#include "./command/command.h"

void InitAIManager();
Command PollAI();
void ExitInputManager();

#endif // AI_MANAGER_H