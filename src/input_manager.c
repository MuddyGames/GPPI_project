#include <stdio.h>
#include <math.h>

#include <raylib.h>

#include "utils/input_manager.h"
#include "utils/constants.h"

/**
 * InitInputManager - Gives the input system a once-over before play begins.
 *
 * A tidy little placeholder for now ... meant for any future setup you might add,
 * whether that's controller configs, rebinding, or the whole works.
 *
 * At the moment, it does absolutely nothing... like a student in a bedsit being asked to tidy their room.
 */

void InitInputManager()
{
	// Initialize input sources (keyboard, controller, etc.)
}

/**
 * PollInput - Gathers whatever buttons the player is mashing.
 *
 * First has a nose at the gamepad .... if it's plugged in and doing anything
 * more than gathering dust, we read the D-pad, thumbstick and trigger (could be the r36s even).
 *
 * If the gamepad isn't active, we fall back to the trusty keyboard.
 *
 * Returns a lovely bitmask of commands (MOVE_UP, ATTACK, etc.),
 * or NONE if the player is sitting there doing nothing at all at all at all.
 */

Command PollInput()
{
	// Default command
	Command command = NONE;

	// Check for gamepad input first
	if (IsGamepadAvailable(0))
	{
		// Get values for left thumbstick and right trigger
		float leftStickX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
		float leftStickY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
		float rightTrigger = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER);

		// Determine if any gamepad input exceeds threshold levels
		bool gamepadActive = (fabs(leftStickX) > THUMBSTICK_DEADZONE_THRESHOLD ||
							  fabs(leftStickY) > THUMBSTICK_DEADZONE_THRESHOLD ||
							  rightTrigger > FIRING_TRIGGER_THRESHOLD ||
							  IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) ||
							  IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
							  IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ||
							  IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT));

		// If the gamepad is active, determine specific command based on input
		if (gamepadActive)
		{
			// Check D-pad directional buttons for movement commands
			if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
				command |= MOVE_UP;
			if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
				command |= MOVE_DOWN;
			if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
				command |= MOVE_LEFT;
			if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
				command |= MOVE_RIGHT;

			// Check thumbstick for directional input, prioritising vertical movement
			if (fabsf(leftStickY) > THUMBSTICK_DEADZONE_THRESHOLD ||
				fabsf(leftStickX) > THUMBSTICK_DEADZONE_THRESHOLD)
			{
				if (leftStickY < -MOVE_VERTICAL_THRESHOLD)
					command |= MOVE_UP;
				if (leftStickY > MOVE_VERTICAL_THRESHOLD)
					command |= MOVE_DOWN;
				if (leftStickX < -MOVE_HORIZONTAL_THRESHOLD)
					command |= MOVE_LEFT;
				if (leftStickX > MOVE_HORIZONTAL_THRESHOLD)
					command |= MOVE_RIGHT;
			}

			// Check right trigger for firing command
			if (rightTrigger > FIRING_TRIGGER_THRESHOLD)
				command |= ATTACK;
		}
	}

	// If no gamepad input check keyboard
	if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
		command |= MOVE_UP;
	if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
		command |= MOVE_DOWN;
	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
		command |= MOVE_LEFT;
	if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
		command |= MOVE_RIGHT;

	// Sword: SPACE (swipe with the sword)
	if (IsKeyDown(KEY_SPACE))
		command |= ATTACK;

	// DEFEND is not bound to a key or button yet: one for you to add

	// Log the command only when it changes (a key or button pressed or released),
	// not every frame, so the console stays readable.
	static Command previousCommand = NONE;
	if (command != previousCommand)
	{
		char bits[COMMAND_COUNT + 1]; // COMMAND_COUNT bits + null
		char commandNames[COMMAND_NAMES_SIZE];
		GetCommandBits(command, bits);
		GetCommandNames(command, commandNames, sizeof(commandNames));
		TraceLog(LOG_INFO, "[Input] %s  %s", bits, commandNames);
		previousCommand = command;
	}

	return command; // NONE if nothing was set
}

/**
 * PollRestart : Has the player asked for another go?
 *
 * R on the keyboard, or Start on a gamepad.
 * Only asked at game over, so it doesn't need to be a Command.
 *
 * @return: true on the frame the button goes down.
 */

bool PollRestart()
{
	if (IsKeyPressed(KEY_R))
		return true;

	if (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
		return true;

	return false;
}

/**
 * DeleteInputManager - Gives the input system a polite send-off.
 *
 * Currently does nothing, but here's where you'd unplug whatever needs unplugging
 * or sweep up after yourself if you added anything fancy like a neural mind reader or the
 * new Valve headset yoke.
 */

void DeleteInputManager()
{
	// Cleanup resources if needed
}