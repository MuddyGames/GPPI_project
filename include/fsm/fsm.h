#ifndef FSM_H
#define FSM_H

#include <stdbool.h>
#include <stdio.h>

// Include the events header file that defines the 'Event' enum
#include "events/events.h"

// Forward declaration of the GameObject structure
typedef struct GameObject GameObject;

// Define function pointer types for event handling and state management
typedef void (*EventFunction)(GameObject *, Event, float); // Function type for event handlers
typedef void (*StateFunction)(GameObject *, float);		   // Function type for state entry, update, and exit handlers

// Define an enumeration for different states of the game object
typedef enum
{
	STATE_IDLE,		 // Represents the idle state (no action)
	STATE_WALKING,	 // Represents the walking state
	STATE_ATTACKING, // Represents the attacking state
	STATE_SHIELD,	 // Represents the shield state (defensive posture)
	STATE_DEAD,		 // Represents the dead state (game over or defeated)
	STATE_RESPAWN,	 // Represents the respawn state (respawns the player)
	STATE_COLLISION, // Represents the state when a collision is detected (e.g., with an enemy or obstacle)
	STATE_COUNT		 // Represents the total number of states (for counting purposes)
} State;			 // Define 'State' as the type of the enum

/**
STATES TO BE IMPLEMENTED
Define an enumeration for different states of the game object
typedef enum
{
	STATE_FIRING,       // Represents the state when the player is firing a weapon or projectile
	STATE_DEFEND,       // Represents the state when the player is defending
	STATE_RUN,          // Represents the state when the player is running
} State;                // Define 'State' as the type of the enum, making it easier to refer to in the code
*/

// Event triggers STATE Switching this could be an input event or event from Collision system
typedef struct
{
	Event event; // Event like EVENT_MOVE, EVENT_COLLISION_START
	State state; // State to switch to STATE_WALKING, STATE_COLLISION
} EventStateTransition;

// Define a configuration structure for each state of the GameObject
typedef struct StateConfig
{
	const char *name;							 // The name of the state (e.g., "Idle", "Walking")
	StateFunction Entry;						 // Pointer to the function that is called when entering this state
	StateFunction Update;						 // Pointer to the function that is called to update the state
	StateFunction Exit;							 // Pointer to the function that is called when exiting this state
	EventStateTransition *eventStateTransitions; // Array of possible next states (state transitions)
	int nextStatesCount;						 // Number of possible next states
} StateConfig;									 // Define 'StateConfig' as a structure that holds all state-related configurations

// Helper for setup Unimplemented States
#define UNIMPLEMENTED_STATE_CONFIG (StateConfig){NULL, NULL, NULL, NULL, NULL, 0}

// Helper Method to Allocate and Initialise STATE Configuration
StateConfig *AllocateStateConfigs(int stateCount);

// Helper Method to Initialise STATE Configurations
void InitStateConfig(GameObject *object, State state,
					 const char *name,
					 void (*Entry)(GameObject *, float),
					 void (*Update)(GameObject *, float),
					 void (*Exit)(GameObject *, float));

// Handles an event for the given game object, triggering changes in state
void HandleEvent(GameObject *obj, Event event, float deltaTime);

// Checks if the game object can enter a new state based on the current context
bool CanEnterState(GameObject *obj, State newState);

// Changes the state of the game object to the new state if possible
bool ChangeState(GameObject *obj, State newState, float deltaTime);

// Updates the current state of the game object (for example, animations, actions)
void UpdateState(GameObject *obj, float deltaTime);

// Function to initialize valid state transitions
void StateTransitions(StateConfig *stateConfig, EventStateTransition *transitions, int count);

// Function to print each state configuration
void PrintStateConfigs(StateConfig *stateConfigs, int stateCount);

#endif // FSM_H
