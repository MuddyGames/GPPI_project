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

// HUD panel
#define HUD_PADDING 8
#define HUD_PANEL_ALPHA 0.55f
#define HUD_TEXT_OUTLINE 2

// Default GameObject Size
#define DEFAULT_GAMEOBJECT_RADIUS 25

// Default Player Damage
#define DAMAGE_DEFAULT 15

// Sword swipe: the frame the blade lands on (counting from 0, so 3 = Frame 4 of 6)
#define ATTACK_HIT_FRAME 3

// Sword swipe: how far past the edge of the attacker's collider the blade reaches
#define ATTACK_REACH 20.0f

// Buffer zone to avoid stuck states in collision detection
#define COLLISION_BUFFER 2.0f
#define COLLISION_PUSH_BACK 2.0f

// Sword swipe recovery: after the swipe animation ends, the fighter holds the last
// frame (sword down) this how long before it can act again.
#define ATTACK_RECOVERY 0.1f

// Firing Cooldown (0.1 seconds)
// #define FIRE_COOLDOWN 0.1f

// Firing Trigger Threshold
#define FIRING_TRIGGER_THRESHOLD 0.1f

// Thumbstick deadzone
#define THUMBSTICK_DEADZONE_THRESHOLD 0.2f

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

// Command Names buffer size for Debugging
#define COMMAND_NAMES_SIZE 64

// Backing Track Default Volume (0.0f to 1.0f)
#define BACKING_TRACK_VOLUME 0.6f

#endif // CONSTANTS_H