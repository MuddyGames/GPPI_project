#include "gameobjects/gameobject.h"
#include "utils/constants.h"
/**
 * InitGameObject : Sets up a fresh GameObject with all the basics.
 *
 * Gives the poor creature a name, a position, a collider, some colour,
 * a pile of health, and whatever else it needs to exist without falling
 * apart immediately.
 *
 * @object:       The GameObject being brought into the world.
 * @name:         Its fancy new name.
 * @position:     Where it starts on the screen.
 * @currentState: Its starting state (usually IDLE unless it's dramatic).
 * @color:        The default draw colour.
 * @collider:     For bumping into things.
 * @keyframes:    The sprite sheet used for animations.
 * @health:       Starting health ... try not to waste it.
 */

void InitGameObject(GameObject *object,
					const char *name,
					Vector2 position,
					State currentState,
					Color color,
					c2Circle collider,
					Texture2D keyframes,
					int health)
{
	// No mediator yet: CreateMediator() connects it once both fighters exist
	// See mediator.c CreateMediator()
	object->mediator = NULL;

	// Set the GameObject's name
	object->name = name;

	object->position = position;

	// Initialize the previous and current states to currentState (normally IDLE)
	object->currentState = currentState;
	object->previousState = STATE_COUNT; // So that at initial load idle animation is loaded

	object->color = color;
	object->collider = collider;
	object->keyframes = keyframes;
	object->health = health;

	// Initialise Animation Data
	object->animation.frames = NULL;
	object->animation.frameCount = 0;
	object->animation.currentFrame = 0;
	object->animation.frameTimer = 0.0f;
	object->animation.active = false;
	object->animation.loop = true;
	object->animation.finished = false;

	// Initialise Other Data
	object->timer = 0.0f;

	object->isColliding = false;
	object->attackHit = false;
	object->stateConfigs = NULL;
	object->inputAxis = (Vector2){0.0f, 0.0f};
	object->currentDirection = DOWN;
	object->previousDirection = DOWN;
}

/**
 * ClampGameObjectOnScreen - Keeps the object from wandering off stage.
 *
 * Ensures the object stays within screen bounds, instead of hoofing it
 * straight into the void like it has somewhere better to be, searching for
 * a swan feather and some squid ink to take notes in class.
 *
 * @object: The GameObject being reined in.
 */

void ClampGameObjectOnScreen(GameObject *object)
{
	if (object->position.x < object->collider.r)
		object->position.x = object->collider.r;
	if (object->position.x > SCREEN_WIDTH - object->collider.r)
		object->position.x = SCREEN_WIDTH - object->collider.r;
	if (object->position.y < object->collider.r)
		object->position.y = object->collider.r;
	if (object->position.y > SCREEN_HEIGHT - object->collider.r)
		object->position.y = SCREEN_HEIGHT - object->collider.r;
}

/**
 * InitGameObjectAnimation - Gives the GameObject a working animation.
 *
 * Sets up looping animation using the object's sprite sheet and a list
 * of frames. Once this is done, the object won't just stand there like
 * a cardboard cut-out.... or even worse like a lecturer stuck on pause.
 *
 * @object:     The object that's getting animated.
 * @frames:     Rectangles describing each frame on the sprite sheet.
 * @frameCount: How many frames to play through.
 * @speed:      How fast the animation moves (seconds per frame).
 */

void InitGameObjectAnimation(GameObject *object, Rectangle *frames, int frameCount, float speed)
{
	// Free any existing animation resources
	FreeAnimation(&object->animation);

	// Initialize a new animation
	InitAnimation(&object->animation, object->keyframes, frames, frameCount, speed, true);
}

/**
 * InitGameObjectAnimationOnce - Gives the GameObject an animation that plays once.
 *
 * Same as InitGameObjectAnimation, except it stops on the last frame and
 * sets animation.finished, instead of going round again like a lad stuck
 * on the Carlow ring road. Just the job for a sword swipe.
 *
 * @object:     The object that's getting animated.
 * @frames:     Rectangles describing each frame on the sprite sheet.
 * @frameCount: How many frames to play through.
 * @speed:      How fast the animation moves (seconds per frame).
 */

void InitGameObjectAnimationOnce(GameObject *object, Rectangle *frames, int frameCount, float speed)
{
	// Free any existing animation resources
	FreeAnimation(&object->animation);

	// Initialize a new animation that doesn't loop
	InitAnimation(&object->animation, object->keyframes, frames, frameCount, speed, false);
}

/**
 * DeleteGameObject - Frees all memory linked to the GameObject.
 *
 * Cleans up state transition tables and then the object itself,
 * so you don't end up leaking memory all over the shop....see purge notes
 *
 * @object: The GameObject to be sent off outa the big heap into the sky.
 */

void DeleteGameObject(GameObject *object)
{
	if (object == NULL)
		return;

	// Check if state configurations exist for this GameObject
	if (object->stateConfigs)
	{
		// Free each state's nextStates array if it exists
		for (int i = 0; i < STATE_COUNT; i++)
		{
			if (object->stateConfigs[i].eventStateTransitions != NULL)
			{
				free(object->stateConfigs[i].eventStateTransitions);
				object->stateConfigs[i].eventStateTransitions = NULL; // Nullify the pointer after freeing
			}
		}

		// Free state configurations
		free(object->stateConfigs);
		object->stateConfigs = NULL; // Nullify after freeing
	}

	// Free animation resources
	FreeAnimation(&object->animation);

	// Unload keyframes texture
	if (object->keyframes.id != 0)
	{
		UnloadTexture(object->keyframes);
		object->keyframes.id = 0;
	}

	// Free the GameObject
	free(object);
	object = NULL; // Nullify
}

// Health bar above the object: green for what's left, red for what's lost
static void DrawGameObjectHealthBar(const GameObject *object)
{
	// Healthbar Dimensions
	const int healthBarWidth = 100;
	const int healthBarHeight = 10;

	// Center above GameObject
	const int healthBarX = object->position.x - (healthBarWidth / 2);
	const int healthBarY = object->position.y - 40;

	// Make sure health is between 0 and 100
	int health = object->health;
	if (health < 0)
		health = 0;
	if (health > 100)
		health = 100;

	// Bar width
	float healthPercentage = health / 100.0f;
	int currentHealth = (int)(healthBarWidth * healthPercentage);
	int lostHealth = healthBarWidth - currentHealth;
	lostHealth = Clamp(lostHealth, 0, healthBarWidth);

	// Colours
	Color background = ColorAlpha(GRAY, 0.5f);
	Color currentHealthColor = ColorAlpha(GREEN, 0.5f);
	Color lostHealthColor = ColorAlpha(RED, 0.5f);

	// Background (gray)
	DrawRectangle(healthBarX, healthBarY, healthBarWidth, healthBarHeight, background);

	// Current health (green)
	DrawRectangle(healthBarX, healthBarY, currentHealth, healthBarHeight, currentHealthColor);

	// Lost health (red)
	DrawRectangle(healthBarX + currentHealth, healthBarY, lostHealth, healthBarHeight, lostHealthColor);
}

// Draw the GameObject in this example a simple Circle which is a collider
static void DrawGameObjectColliderCircle(const GameObject *object)
{
	// Set alpha for collisision circle
	Color background = ColorAlpha(object->color, 0.5f);

	// Circle Thickness
	int thickness = 5;

	// GameObject Circle
	DrawRing(
		object->position,					// Vector2 (x, y)
		object->collider.r - thickness / 2, // inner radius
		object->collider.r + thickness / 2, // outer radius
		0.0f, 360.0f,						// start angle, end angle
		64,									// segments
		background							// Color
	);
}

// Draw position information at the bottom of the GameObject
static void DrawGameObjectPositionInfo(const GameObject *object)
{
	const char *information = TextFormat("(%.f, %.f)",
										 object->position.x,
										 object->position.y);
	DrawText(
		information,
		object->position.x - (MeasureText(information, DEFAULT_FONT_SIZE) / 2), // Measure the text with with MeasureText
		object->position.y + 30,												// Place it below the GameObject
		DEFAULT_FONT_SIZE, DARKGRAY);
}

/**
 * DrawGameObject : Draws one game object, with its bits and pieces.
 *
 * Collider ring underneath, then the sprite, then its position and health
 * bar on top. The player and the NPC are drawn exactly the same way.
 *
 * @object: The game object to draw.
 */

void DrawGameObject(const GameObject *object)
{
	DrawGameObjectColliderCircle(object);                       // Ring underneath
	DrawAnimation(&object->animation, object->position, WHITE); // The sprite
	DrawGameObjectPositionInfo(object);                         // (x, y) underneath
	DrawGameObjectHealthBar(object);                            // Health above
}
