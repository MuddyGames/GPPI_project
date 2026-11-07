#include <raylib.h>

#include "utils/constants.h"
#include "utils/audio_manager.h"

// The loaded sounds, one per ActionSound. Only this file knows about them.
static Sound sounds[SOUND_COUNT];

// Where each sound lives (in resources/, so the web build packages them too)
static const char *soundFiles[SOUND_COUNT] = {
	"resources/sounds/levelstart.wav", // SOUND_LEVEL_START
	"resources/sounds/swipe.wav",	   // SOUND_SWIPE
	"resources/sounds/hit.wav",		   // SOUND_HIT
	"resources/sounds/block.wav",	   // SOUND_BLOCK
	"resources/sounds/gameover.wav"	   // SOUND_GAMEOVER
};

// Game background music
static Music background;
static const char *backgroundFile = "resources/music/background.ogg";

/**
 * InitAudioManager : Tunes up the band before the session starts.
 *
 * Opens the audio device and loads every sound in soundFiles. Called once
 * from main.c after InitWindow(), not from InitGame(): a restart reuses the
 * sounds instead of loading another copy of each one every time.
 *
 * If a sound file is missing, or the machine has no audio device (some lab
 * machines, WSL), raylib says so in the console and the game just stays
 * quiet ... no harm done, like a tin whistle with no player.
 */

void InitAudioManager()
{
	InitAudioDevice();

	for (int i = 0; i < SOUND_COUNT; i++)
	{
		sounds[i] = LoadSound(soundFiles[i]);
	}

	// Backing track: loaded once, like the sounds. Streams loop by default.
	background = LoadMusicStream(backgroundFile);
	if (IsMusicValid(background))
		SetMusicVolume(background, BACKING_TRACK_VOLUME);
}

/**
 * PlayActionSound : Plays one of the action sounds.
 *
 * The game objects only say what happened (SOUND_SWIPE, SOUND_HIT...),
 * not which file to play or how. Swap the .wav files in resources/sounds/
 * and nobody else needs to change a line.
 *
 * Each play gets a slightly different pitch, so ten swipes in a row don't
 * sound like a stuck record at a wedding.
 *
 * @sound: Which sound to play.
 */

void PlayActionSound(ActionSound sound)
{
	if (sound < 0 || sound >= SOUND_COUNT || !IsSoundValid(sounds[sound]))
	{
		TraceLog(LOG_ERROR, "[AudioManager] Invalid sound index %d", sound);
		return;
	}

	// A little variety: 90% to 110% of the normal pitch
	// SetSoundPitch(sounds[sound], GetRandomValue(90, 110) / 100.0f);
	PlaySound(sounds[sound]);
}

/**
 * PlayBackingTrack : Strikes up the band.
 *
 * Starts the backing track from the top (stopping it first means a restart
 * starts the tune again, rather than carrying on mid-verse). It loops until
 * StopBackingTrack().
 */

void PlayBackingTrack()
{
	if (!IsMusicValid(background))
		return; // No file, or no audio device: carry on quietly

	StopMusicStream(background);
	PlayMusicStream(background);
}

/**
 * StopBackingTrack : The band stops (e.g. at game over, so the game-over
 * sound isn't drowned out).
 */

void StopBackingTrack()
{
	if (IsMusicValid(background))
		StopMusicStream(background);
}

/**
 * UpdateAudioManager : Keeps the backing track fed.
 *
 * A Sound is loaded whole, so it just plays. A Music stream is read a
 * little at a time, so raylib needs topping up every frame: forget this
 * and the track plays a fraction of a second then goes quiet, like a
 * session where nobody buys the next round.
 *
 * Called once a frame from main.c.
 */

void UpdateAudioManager()
{
	if (IsMusicValid(background))
		UpdateMusicStream(background);
}

/**
 * DeleteAudioManager : Packs up the band at closing time.
 *
 * Unloads every sound, then closes the audio device. Called once from
 * main.c, before CloseWindow().
 */

void DeleteAudioManager()
{
	for (int i = 0; i < SOUND_COUNT; i++)
	{
		if (IsSoundValid(sounds[i]))
			UnloadSound(sounds[i]);
	}

	if (IsMusicValid(background))
		UnloadMusicStream(background);

	// Only close what was opened (no audio device on some lab machines or in WSL)
	if (IsAudioDeviceReady())
		CloseAudioDevice();
}
