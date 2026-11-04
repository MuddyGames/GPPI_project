#ifndef CONSTANTS_H
#define CONSTANTS_H

// Screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// FPS Target
#define TARGET_FPS 60

// Default Pushback Distance
#define PUSHBACK_DISTANCE 0.5f

// Default Font Size
#define DEFAULT_FONT_SIZE 20

// Default Y Offset for Text
#define DEFAULT_TEXT_Y_OFFSET 30

// Default GameObject Size
#define DEFAULT_GAMEOBJECT_RADIUS 25

// Default Player Damage
#define DAMAGE_DEFAULT 15

// Buffer zone to avoid stuck states in collision detection
#define COLLISION_BUFFER 2.0f
#define COLLISION_PUSH_BACK 2.0f

// Firing Cooldown (0.1 seconds)
#define COMMAND_FIRE_COOLDOWN 0.1f

// Firing Trigger Treshold
#define FIRING_TRIGGER_TRESHOLD 0.1f

// Tumbstick deadzone
#define TUMBSTICK_DEADZONE_THRESHOLD 0.2f

// Thresholds for directional movement
#define MOVE_VERTICAL_THRESHOLD 0.5f
#define MOVE_HORIZONTAL_THRESHOLD 0.5f
#define MOVE_DIAGONAL_THRESHOLD 0.5f

// Default Movement Speed
#define DEFAULT_MOVEMENT_SPEED 130.0f

// NPC Aggression Factors for movement speed scaling
#define NPC_MIN_AGGRESSION 0.3f
#define NPC_MAX_AGGRESSION 0.8f

// Aggression adjustment rates for Building up and Decaying aggression
#define NPC_AGGRESSION_BUILD 0.2f
#define NPC_AGGRESSION_DECAY 0.3f

#endif // CONSTANTS_H