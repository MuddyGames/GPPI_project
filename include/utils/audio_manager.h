#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

// The action sounds the game can play
typedef enum
{
	SOUND_LEVEL_START, // Level starting
	SOUND_SWIPE,	   // A sword swipe starts
	SOUND_HIT,		   // A sword swipe lands
	SOUND_BLOCK,	   // A shield stops a sword
	SOUND_GAMEOVER,	   // Someone has won
	SOUND_COUNT		   // Total number of sounds (for counting purposes)
} ActionSound;

// Init Audio Manager: opens the audio device and loads the sounds (call once, after InitWindow)
void InitAudioManager();

// Keep the backing track fed (call once every frame, see main.c)
void UpdateAudioManager();

// Play an action sound
void PlayActionSound(ActionSound sound);

// Play the background music
void PlayBackingTrack();

// Stop the background music
void StopBackingTrack();

// Cleanup: unloads the sounds and closes the audio device (call once, before CloseWindow)
void DeleteAudioManager();

#endif // AUDIO_MANAGER_H
