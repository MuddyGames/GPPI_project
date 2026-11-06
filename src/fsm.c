#include "fsm/fsm.h"
#include "gameobjects/gameobject.h"

// These are name lookups for the Event and State enums, 
// If you add new Events or States, make sure to update the EVENT_NAMES and STATE_NAMES arrays.

static const char *EVENT_NAMES[EVENT_COUNT] = {
	[EVENT_NONE] = "NONE",
	[EVENT_MOVE] = "MOVE",
	[EVENT_ATTACK] = "ATTACK",
	[EVENT_DEFEND] = "DEFEND",
	[EVENT_DIE] = "DIE",
	[EVENT_RESPAWN] = "RESPAWN",
	[EVENT_COLLISION_START] = "COLLISION_START",
	[EVENT_COLLISION_END] = "COLLISION_END",
	[EVENT_ANIMATION_END] = "ANIMATION_END"};

static const char *STATE_NAMES[STATE_COUNT] = {
	[STATE_IDLE] = "Idle",
	[STATE_WALKING] = "Walking",
	[STATE_ATTACKING] = "Attacking",
	[STATE_SHIELD] = "Shield",
	[STATE_DEAD] = "Dead",
	[STATE_RESPAWN] = "Respawn",
	[STATE_COLLISION] = "Collision"};

const char *EventName(Event event)
{
	if (event < 0 || event >= EVENT_COUNT || EVENT_NAMES[event] == NULL)
		return "?";
	return EVENT_NAMES[event];
}

const char *StateName(State state)
{
	if (state < 0 || state >= STATE_COUNT || STATE_NAMES[state] == NULL)
		return "?";
	return STATE_NAMES[state];
}

/**
 * AllocAndInitStateConfigs : Allocates and initializes state configurations.
 *
 * Allocates memory for the specified number of state configurations
 * and initializes each configuration to a default unimplemented state.
 *
 * @stateCount: The number of state configurations to allocate.
 *
 * @return: Pointer to the allocated and initialized array of StateConfig structures.
 */

StateConfig *AllocateStateConfigs(int stateCount)
{
	// Allocate memory for the state configurations array
	StateConfig *configs = (StateConfig *)malloc(sizeof(StateConfig) * stateCount);

	// Check if memory allocation failed
	if (!configs)
	{
		TraceLog(LOG_ERROR, "Failed to allocate state configs");
		exit(1);
	}

	// Initialize all state configs to UNIMPLEMENTED_STATE_CONFIG
	for (int i = 0; i < stateCount; i++)
	{
		configs[i] = UNIMPLEMENTED_STATE_CONFIG;
	}

	return configs;
}

/**
 * InitStateConfig : Sets up a state's callbacks and name.
 *
 * Gives the state its proper bits and pieces, a name and whatever
 * function pointers it needs. Which event leads where is set separately,
 * in its EventStateTransition table (see StateTransitions).
 *
 * Basically handing the state its toolkit so it can behave properly.
 *
 * @object: Poor divil whose state is getting set up, hope its not STATE_DEAD.
 * @state:  Which state is being configuring (STATE_IDLE, STATE_WALKING, STATE_BOOZED (actually BOOZED not allowed LOL etc.)
 * @name:   Grand little STATE label
 * @Entry:  Runs when the object enters the state ... a sort of "Dia duit".
 * @Update: Runs each frame while in the state ... "is she buring in Oil?", "she would if she got it!"
 * @Exit:   Runs when hopping out out out out out of a state ... a polite "Slan leat".
 */

void InitStateConfig(GameObject *object, State state,
					 const char *name,
					 void (*Entry)(GameObject *, float),
					 void (*Update)(GameObject *, float),
					 void (*Exit)(GameObject *, float))
{
	object->stateConfigs[state].name = name;
	object->stateConfigs[state].Entry = Entry;
	object->stateConfigs[state].Update = Update;
	object->stateConfigs[state].Exit = Exit;
}

/**
 * HandleEvent : Fires an event at the object's current state.
 *
 * Checks whatever state the poor lad is in, and if that state actually
 * has a handler, it gets called. Otherwise nothing happens and everyone
 * pretends it's grand.
 *
 * @object: The GameObject receiving the event.
 * @event:  The event being sent (EVENT_MOVE, EVENT_ATTACK, EVENT_COLLISION_START, etc.)
 */

void HandleEvent(GameObject *object, Event event, float deltaTime)
{
	// Get the state configuration for the current state of the object
	StateConfig *config = &object->stateConfigs[object->currentState];
	// Process event and change state based on event
	for (int i = 0; i < config->nextStatesCount; i++)
	{
		if (config->eventStateTransitions[i].event == event)
		{
			ChangeState(object, config->eventStateTransitions[i].state, deltaTime);
			return;
		}
	}
	// Stay in current state if no match
	// (previousState only changes in ChangeState(), so it always means "the state before this one")
}

/**
 * UpdateState : Runs the update function for the current state.
 *
 * Some states like to do things every frame ... animations, timers,
 * walking, attacking... all that good craic. If the state has an
 * Update function, we give it a chance to do its stuff like.
 *
 * @object: The GameObject whose state is being updated.
 */

void UpdateState(GameObject *object, float deltaTime)
{
	// Get the configuration for the current state
	StateConfig *config = &object->stateConfigs[object->currentState];

	// If an update function is defined for the current state, call it
	if (config->Update)
	{
		config->Update(object, deltaTime); // Perform the update for the current state (e.g., animation, actions)
	}
}

/**
 * CanEnterState : Checks if the object's allowed to move to a new state.
 *
 * Avoids illegal transitions .... like trying to go from Dead to Walking,
 * which would raise a few eyebrows. We simply check the list of valid
 * next states for the current state.
 *
 * @object:   The GameObject trying to move.
 * @newState: The state it wants to reach.
 *
 * @return: true if allowed, false if it's having notions.
 */

bool CanEnterState(GameObject *object, State newState)
{
	// Get the current state configuration
	StateConfig *currentConfig = &object->stateConfigs[object->currentState];

	// Loop through the possible next states and check if newState is valid
	for (int i = 0; i < currentConfig->nextStatesCount; i++)
	{
		if (currentConfig->eventStateTransitions[i].state == newState)
			return true; // Valid transition found
	}
	return false; // No valid transition found
}

/**
 * ChangeState : Tries to switch the object to a new state.
 *
 * Checks if the transition is allowed, calls the Exit function of the
 * old state, updates the state, then calls the Entry function for the
 * new state .... all very polite and orderly.
 *
 * @object:   The GameObject changing state.
 * @newState: The state it wants to hop into.
 *
 * @return: true if the change succeeds, false if the FSM says "absolutely not"
 * like a bouncer at the Dinn Ri.
 */

bool ChangeState(GameObject *object, State newState, float deltaTime)
{
	State currentState = object->currentState;

	// Check if the state transition is valid
	if (!CanEnterState(object, newState))
	{
		TraceLog(LOG_WARNING, "[%s] %s -> %s is not allowed: add it to %s's transition table",
				 object->name, StateName(currentState), StateName(newState), StateName(currentState));
		return false; // Transition failed
	}

	// One line per state change: who, from and to
	TraceLog(LOG_INFO, "[%s] %s -> %s", object->name, StateName(currentState), StateName(newState));

	// Get the configuration of the current state and the new state
	StateConfig *currentConfig = &object->stateConfigs[currentState];
	StateConfig *newConfig = &object->stateConfigs[newState];

	// If the current state has an exit function defined, call it
	if (currentConfig->Exit)
		currentConfig->Exit(object, deltaTime);

	// Update the object's previous and current state
	object->previousState = currentState;
	object->currentState = newState;

	// If the new state has an entry function defined, call it
	if (newConfig->Entry)
		newConfig->Entry(object, deltaTime);

	return true; // State transition successful
}

/**
 * StateTransitions : Sets up which states you're allowed to jump to.
 *
 * Allocates a list of valid next states and copies them in. Keeps the FSM
 * from wandering off into nonsense states like a drunk lad on Tullow Street.
 *
 * @stateConfig: The state being configured.
 * @transitions: Array of valid next states.
 * @stateCount:  Number of entries in that array.
 */

void StateTransitions(StateConfig *stateConfig, EventStateTransition *transitions, int stateCount)
{
	// Allocate memory for the next states array based on the given count
	stateConfig->eventStateTransitions = (EventStateTransition *)malloc(sizeof(EventStateTransition) * stateCount);
	if (!stateConfig->eventStateTransitions)
	{
		// If memory allocation fails, print an error and exit
		TraceLog(LOG_ERROR, "Failed to allocate state transitions");
		exit(1); // Exit the program if allocation fails
	}

	// Copy the transitions array into the stateConfig's nextStates array
	memcpy(stateConfig->eventStateTransitions, transitions, sizeof(EventStateTransition) * stateCount);
	stateConfig->nextStatesCount = stateCount; // Set the count of next states
}

/**
 * PrintStateConfigs : Prints all the juicy details of every state.
 *
 * Handy for debugging, or for when you're staring at the FSM wondering
 * why in the name of ####### it won't go into Walking.
 *
 * Shows state names, handlers, transitions... the whole lot.
 *
 * @stateConfigs: Array of StateConfig structures.
 * @stateCount:   Number of states in the system.
 */

 void PrintStateConfigs(StateConfig *stateConfigs, int stateCount)
{
	// Loop through each state configuration and print its details
	for (int i = 0; i < stateCount; i++)
	{
		StateConfig *config = &stateConfigs[i];

		// Only print if the state is properly configured (i.e., has a name and event handler)
		if (config->name == NULL)
			continue; // Skip printing incomplete or improperly configured states

		// Print the state name and availability of its handler functions
		char line[256];
		int used = snprintf(line, sizeof(line), "FSM %-18s [%s %s %s] ",
							config->name,
							config->Entry ? "Entry" : "Unconfigured",
							config->Update ? "Update" : "Unconfigured",
							config->Exit ? "Exit" : "Unconfigured");

		// Check if there are any valid next states
		// Skip if there are none
		if (config->nextStatesCount <= 0 || config->eventStateTransitions == NULL)
			snprintf(line + used, sizeof(line) - used, " (no way out)");

		// Print the list of valid next states
		for (int j = 0; j < config->nextStatesCount && used < (int)sizeof(line); j++)
		{
			used += snprintf(line + used, sizeof(line) - used, "%s %s -> %s",
							 j > 0 ? "," : "",
							 EventName(config->eventStateTransitions[j].event),
							 StateName(config->eventStateTransitions[j].state));
		}

		TraceLog(LOG_INFO, "%s", line);
	}
}