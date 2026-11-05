# Animated FSM Command Pattern StarterKit Guide

This _StarterKit_ is the starting point for your project. It's a [raylib](https://www.raylib.com/) game in C that brings together everything from the practicals: an [**Animated Finite State Machine (FSM)**](https://www.codeproject.com/articles/State-Machine-Design-in-C#comments-section), command-based input, a mediator, [collision detection](https://github.com/RandyGaul/cute_headers/blob/master/cute_c2.h) with cute_c2, and sprite-sheet animation. A player and an NPC fight it out with swords, and the FSM decides what each of them can do at any moment.

It's designed for learning core game development patterns: event handling, state transitions, animated sprites and game object management. Your job is to turn it into your own game.

_StarterKit_ provides:

- A Finite State Machine (FSM) for player and NPC behaviour, driven by transition tables
- The Command pattern for input, passed on by a Mediator
- Sprite-sheet animations, set up when each state starts
- Circle-based collision detection, with collision events sent to the FSM
- Sword damage on the frame the blade lands, and shield blocking
- A HUD, action sounds, and a new background every game

> The project was generated with [raylib project creator](https://raysan5.itch.io/raylib-project-creator) by raysan5.

---

## Short version

If you just want to get going, this is the whole thing in a few lines:

**1. Clone**

```bash
mkdir -p ~/Projects
cd ~/Projects
git clone https://github.com/MuddyGames/GPPI_project
```

**2. Set up raylib**

*Linux:*

```bash
cd ~/Projects/GPPI_project/setup
bash setup_linux.sh
```

or make the script executable and run it directly:

```bash
cd ~/Projects/GPPI_project/setup
chmod +x setup_linux.sh
./setup_linux.sh
```

*Windows* (install [raylib](https://raysan5.itch.io/raylib) first, and then use w64devkit terminal):

```bash
cd ~/Projects/GPPI_project/setup
sh setup_windows.sh
```

**3. Build and run**

```bash
cd ~/Projects/GPPI_project
make run                            # desktop
```

The rest of this page explains each step, and what to do when the computer says no.

---

## Step 1: Get the project

Keep all your work in a `Projects` folder in your home directory.

- **Linux:** open a terminal.
- **Windows:** open the **w64devkit** terminal (`C:\raylib\w64devkit\w64devkit.exe`).

Then type the same commands on both:

```bash
mkdir -p ~/Projects
cd ~/Projects
git clone https://github.com/MuddyGames/GPPI_project
```

Your project now lives in `~/Projects/GPPI_project`.

---

## Step 2: Install raylib and Emscripten (once only)

If you set these up for an earlier practical, skip this step. They're shared by every raylib project.

### Linux (Debian, Ubuntu, Pop!_OS or Arch)

```bash
cd ~/Projects/GPPI_project/setup
bash setup_linux.sh
```

It will ask for your password to install packages. It takes a few minutes, especially on lab machines.

### Windows

Install the **[raylib installer](https://raysan5.itch.io/raylib)** and keep the default location, `C:\raylib`. Then in the **w64devkit** terminal:

```bash
cd ~/Projects/GPPI_project/setup
sh setup_windows.sh
```

When it finishes, close w64devkit and open it again.

> No more MSYS2. Everything now runs in the w64devkit terminal that comes with the raylib installer.

For the manual steps, see the [drawing_primitives README](https://github.com/MuddyGames/drawing_primitives#manual-setup-only-if-the-script-didnt-work-for-you).

---

## Step 3: Build and run

Every command runs from the project folder:

```bash
cd ~/Projects/GPPI_project
```

### On your desktop

The same commands work on Linux and Windows (w64devkit):

| Command | What it does |
|---|---|
| `make` | Builds the game |
| `make run` | Builds it and runs it |
| `make BUILD_MODE=DEBUG` | Builds with debug info, for stepping through in the debugger |
| `make clean` | Deletes the `build` folder, so the next build starts fresh |

A window opens on a field of grass (a new one every game), with the **Player Hero** in the middle and **The Villan** at the top. The Villan will come for you.

| Key | Action |
|---|---|
| Arrow keys, D-pad or left thumbstick | Walk |
| S, Z, J or SPACE, or the right trigger | Sword swipe (hold for one swipe after another) |
| R | Restart, after game over |
| ESC | Quit |

Movement is on the arrow keys only, so the letters are free for actions. There are three sets of action keys, and they all work at once:

| Action | Brief | Left hand | Right hand |
|---|---|---|---|
| Sword | **S** | **Z** | **J** |
| Shield | **D** | **X** | **K** |
| Mega Blast | **M** | **C** | **L** |

Only the sword keys do something so far (plus SPACE, for now). Shield and Mega Blast are yours to add (see Task 1). Z/X/C sits next to the arrow keys for the other hand, which is how most players will find it comfortable.

The HUD in the top left shows the command bits and the player's current state. Keep the terminal where you can see it too: every state change and every hit is printed there.

> No sound? Some lab machines and WSL have no audio device. The game still runs, just quietly; raylib says so in the terminal.

### In your browser

```bash
make run_web
```

This builds the web version, starts a small local web server and opens
**<http://127.0.0.1:8000/GPPI_project.html>** in your browser.

Press **Ctrl+C** in the terminal to stop the server.

| Command | What it does |
|---|---|
| `make web` | Builds the web version only |
| `make run_web` | Builds it and opens it in your browser |
| `make stop_web` | Stops a web server left running from earlier |

Everything in `resources/` (sprite sheets and sounds) is packaged into the web build automatically. Browsers stay silent until you've pressed a key, so the first sound may not play. That's normal.

### In VS Code

Open `projects/VSCode/main.code-workspace` (**File → Open Workspace from File…**).

- **Ctrl+Shift+B** builds.
- **F5** builds and starts the debugger. The first time, choose **Debug** in the Run and Debug drop-down.
- **Terminal → Run Task** lists Run, Run Web, Stop Web Server and Clean.

Set a breakpoint in `AttackHit()` in `collision.c` and swing at The Villan. Step through and watch which check lets the hit through: reach, facing, or shield.

---

## How the project is laid out

```
GPPI_project/
├── Makefile                build instructions
├── src/
│   ├── main.c              opens the window and the audio, runs the game loop
│   ├── game.c              sets up, updates and draws the game; background; game over
│   ├── fsm.c               the state machine: HandleEvent, ChangeState, UpdateState
│   ├── gameobject.c        what the player and NPC have in common
│   ├── player.c            the player's states, transition tables and animations
│   ├── npc.c               the NPC's states, transition tables and animations
│   ├── animation.c         plays frames from a sprite sheet
│   ├── input_manager.c     turns key, mouse and button presses into commands
│   ├── ai_manager.c        makes up commands for the NPC, once a second
│   ├── mediator.c          passes each command to a game object and the HUD
│   ├── command.c           ExecuteCommand(): turns command bits into FSM events
│   ├── hud.c               shows the command bits and the player's state
│   ├── audio_manager.c     loads and plays the action sounds
│   ├── collision.c         collision checks, pushback, and sword damage (AttackHit)
│   ├── utils.c             directions and movement helpers
│   └── cute_c2_impl.c      builds the cute_c2 collision library; leave it alone
├── include/
│   ├── animation/          animation.h
│   ├── command/            command.h      the command bit flags
│   ├── events/             events.h       the Event enum
│   ├── fsm/                fsm.h          the State enum, StateConfig, EventStateTransition
│   ├── game/               game.h         GameData
│   ├── gameobjects/        gameobject.h, player.h, npc.h
│   ├── hud/                hud.h
│   └── utils/              ai_manager.h, audio_manager.h, collision.h, constants.h,
│                           cute_c2.h, input_manager.h, mediator.h, utils.h
├── resources/              sprite sheets, sounds/ and icon/ (packaged into the web build)
├── assets/production/      grid versions of the sprite sheets, for working out frames
├── build/                  everything make creates
├── platform/               icons and web page template; leave these alone
└── setup/                  setup scripts (run once)
```

Headers live in subfolders, so includes name the folder too: `#include "gameobjects/player.h"`. The `.c` files are all in `src/`.

---

## Architecture

_StarterKit_ is built from several design patterns and systems working together.

### Game System Flow

[![](https://mermaid.ink/img/pako:eNpVkltv4jAQhf-KNSv1KUW5EkillSABygO7ldinTfrgJWNIFdvIcdqmiP--zqVtyIPlOd85M5YyFzjIHCECVsq3w4kqTf4kmSDmW6Rbca412VFBj6ieyf39T7JMd5gXVEv13LuWnRynseScipzcvaLSD2T1ikKTwRN3niRd73eDknTKKl2IglNdSEH2TaWR3-B1uqEcye9_L3jQ1YDWHdqkTyVtUN2Ij-mvp3hQNp2yNc8qy6JqBySoTRtzGxyPvaMvtv0L-6I_K92USBaEFWUZ_UCHBYyNyXIgjDEfnTGJvzPsliRfmbbfmKw-SdCmxmT9Pce-7bYddUMHLOCoOC1y8zcvrS8DfUKOGUTmmiOjdakzyMTVWGmt5b4RB4i0qtECJevjCSJGy8pU9TmnGpOCHhXlX-qZir9S8s_IUbWjhjiKHFUsa6EhcsPOC9EF3iHyfG_ihkHoTj1n5gVz34IGIsefzD3bnob21PXCcO5dLfjomtuTaRD6MzeYOa5jh044t8CsnNm4Xb-p3cJe_wNNUc_m?type=png)](https://mermaid.live/edit#pako:eNpVkltv4jAQhf-KNSv1KUW5EkillSABygO7ldinTfrgJWNIFdvIcdqmiP--zqVtyIPlOd85M5YyFzjIHCECVsq3w4kqTf4kmSDmW6Rbca412VFBj6ieyf39T7JMd5gXVEv13LuWnRynseScipzcvaLSD2T1ikKTwRN3niRd73eDknTKKl2IglNdSEH2TaWR3-B1uqEcye9_L3jQ1YDWHdqkTyVtUN2Ij-mvp3hQNp2yNc8qy6JqBySoTRtzGxyPvaMvtv0L-6I_K92USBaEFWUZ_UCHBYyNyXIgjDEfnTGJvzPsliRfmbbfmKw-SdCmxmT9Pce-7bYddUMHLOCoOC1y8zcvrS8DfUKOGUTmmiOjdakzyMTVWGmt5b4RB4i0qtECJevjCSJGy8pU9TmnGpOCHhXlX-qZir9S8s_IUbWjhjiKHFUsa6EhcsPOC9EF3iHyfG_ihkHoTj1n5gVz34IGIsefzD3bnob21PXCcO5dLfjomtuTaRD6MzeYOa5jh044t8CsnNm4Xb-p3cJe_wNNUc_m)

1. **Input Manager** captures keyboard and gamepad input (the **AI Manager** does the same job for the NPC).
2. **Mediator** passes each command on to the game object and the HUD.
3. **Command System** (`ExecuteCommand()`) translates the command bits into FSM events.
4. **FSM** looks up the event in the current state's transition table, and changes state if there's a match.
5. **Animation System** plays the animation that each state sets up when it starts.
6. **Game Objects** (Player and NPC) run their state's behaviour every frame.
7. **Collision Detection** checks for contact and sword hits, and sends collision events to the FSM.

### Command Flow

[![](https://mermaid.ink/img/pako:eNptk21v2jAQx7-K5VetlFICgTxoQqpSylCVUS10Lyak6UiuIVNiZ47TljG--84JdCCWF_HD_3d3-p_tHU9kijzgNf5qUCR4n0OmoFwJRl8FSudJXoHQbB4xqNlcVI1mEQjIUF1CLRNhmoOW_5HD2OihLEsQ6aX8ELfxNFxqs4WRZlDiYv0TE90R3X8e3Uwm8yhgT7Io2CNu1xJUemvgCtIThpCv8HYwcQ8aOs1IYUzhSiZYH0x2UhgftKUCURegkWl5dMAeCsjqT2t1O7mKFt-mP56f2B92t1zehY_XJ_FU9syzqUcmAzZ9x6ShlGcqKUc9bJRCMh9rU_gzIQVOX2nn6vqUnS0C9lylhvnXoC6ow2aLY8YuU7gBkWF62sMvkvblKyrqlGUSnp00u5l8HKuZHxtAU3NoNJjCrKtcc4uXqErIU7pYO5N_xfUGS1zxgKYpvkBT6BVfiT2h0GgZb0XCA60atLiSTbbhwQsUNa2a1tfhVn7s0pX4LmV5DMmUKXUIR5GiCmUjNA_svt_CPNjxd1p6fs8e-bbtjUeDoTN0Rxbf8sAb9lzH9R137A8817b3Fv_dZu_33PHItgejodd3fGIci1MXqAlR92rax7P_Czt8BME?type=png)](https://mermaid.live/edit#pako:eNptk21v2jAQx7-K5VetlFICgTxoQqpSylCVUS10Lyak6UiuIVNiZ47TljG--84JdCCWF_HD_3d3-p_tHU9kijzgNf5qUCR4n0OmoFwJRl8FSudJXoHQbB4xqNlcVI1mEQjIUF1CLRNhmoOW_5HD2OihLEsQ6aX8ELfxNFxqs4WRZlDiYv0TE90R3X8e3Uwm8yhgT7Io2CNu1xJUemvgCtIThpCv8HYwcQ8aOs1IYUzhSiZYH0x2UhgftKUCURegkWl5dMAeCsjqT2t1O7mKFt-mP56f2B92t1zehY_XJ_FU9syzqUcmAzZ9x6ShlGcqKUc9bJRCMh9rU_gzIQVOX2nn6vqUnS0C9lylhvnXoC6ow2aLY8YuU7gBkWF62sMvkvblKyrqlGUSnp00u5l8HKuZHxtAU3NoNJjCrKtcc4uXqErIU7pYO5N_xfUGS1zxgKYpvkBT6BVfiT2h0GgZb0XCA60atLiSTbbhwQsUNa2a1tfhVn7s0pX4LmV5DMmUKXUIR5GiCmUjNA_svt_CPNjxd1p6fs8e-bbtjUeDoTN0Rxbf8sAb9lzH9R137A8817b3Fv_dZu_33PHItgejodd3fGIci1MXqAlR92rax7P_Czt8BME)

1. **Input Manager** polls the keyboard and gamepad (the **AI Manager** chooses the NPC's input).
2. **Mediator** receives the command bits and hands them on.
3. **Command System** turns the bits into events, such as `EVENT_MOVE` or `EVENT_ATTACK`.
4. **FSM** checks the event against the current state's transition table, in `HandleEvent()`.
5. **GameObject** changes state if the table allows it: the old state's `Exit()` runs, then the new state's `Entry()`.

### Control Flow (Game Loop)

[![](https://mermaid.ink/img/pako:eNqdlV1vmzAUhv8KclSpk5IKEkKASpsI2Emar2ntNm2kF25wEjawIzBruzT_fcZAirJdjHKBbM77vOdwbPABrFlAgA02EXtc73DClTtvRVdUEdfFhdLpdBQ0W35VLtcs3uM1VyL8QKL0XR4pVLdcUJf-CMdEmTG2Lx7c54r3iuN_ZFE0ofuMt3T1uqVr1_cF5si4689JEGLOkjGmQURcFsdikGtPwxPiSsTzRQJOlAKAvwiV1vALXNzVxJ4Uw0MhvkswTUMeMvrhWMSLO8xVL99I-qIgHz6F0koiNSskrUY-pJwk_xKMpGDsf94HIlBG_5YVyRbsRRnXaxhLelLSDg1jnBeaO-RNXT78IGtes5lI_Vx21pmctXUugzelmeIKTRiQJD2TFfcbKZ4epCqtNWf62paZX61MqTlzmkmPhe8l-DEv9yw8Pb3zotpWC0ksD58ySkO6rXIuX3PKHVR_mvOQBvVdJqb37yrLlD9HpMCUTRhFdstSIbTUdsoT9pPYrZ6uW31YTjuPYcB3trZ_qtPCsGQRGhqu0Yg9kVAf9puQ0zeTyzeTTkl6BnSQ2YR0SxIOUR8Nm5BeRZqeA1ETElXv6XhDr1G1o4p0Uc9tlHNckRBZcNCEnFRkF-lIb0LOq1XRIfIaVXtT5fSgB90m5Oy1twOn0deyOO0hsSr_US1og5gkMQ4DcdgccqcV4DsSkxWwxTAgG5xFfAVW9CikOOPs9pmugc2TjLRBwrLtDtgbHKVilsl_mxfibYLjSrLH9Dtjp-k2yTOVNKHiF-iyjHJga31DioF9AE_A7g7MK3NgGj2tb2kDy7Da4BnYna5qXamappuWaZiaoavGsQ1-S3_tSld7mtrtWwIyu1av3wbiDBNH2Lw4SuWJevwDFQ0-7g?type=png)](https://mermaid.live/edit#pako:eNqdlV1vmzAUhv8KclSpk5IKEkKASpsI2Emar2ntNm2kF25wEjawIzBruzT_fcZAirJdjHKBbM77vOdwbPABrFlAgA02EXtc73DClTtvRVdUEdfFhdLpdBQ0W35VLtcs3uM1VyL8QKL0XR4pVLdcUJf-CMdEmTG2Lx7c54r3iuN_ZFE0ofuMt3T1uqVr1_cF5si4689JEGLOkjGmQURcFsdikGtPwxPiSsTzRQJOlAKAvwiV1vALXNzVxJ4Uw0MhvkswTUMeMvrhWMSLO8xVL99I-qIgHz6F0koiNSskrUY-pJwk_xKMpGDsf94HIlBG_5YVyRbsRRnXaxhLelLSDg1jnBeaO-RNXT78IGtes5lI_Vx21pmctXUugzelmeIKTRiQJD2TFfcbKZ4epCqtNWf62paZX61MqTlzmkmPhe8l-DEv9yw8Pb3zotpWC0ksD58ySkO6rXIuX3PKHVR_mvOQBvVdJqb37yrLlD9HpMCUTRhFdstSIbTUdsoT9pPYrZ6uW31YTjuPYcB3trZ_qtPCsGQRGhqu0Yg9kVAf9puQ0zeTyzeTTkl6BnSQ2YR0SxIOUR8Nm5BeRZqeA1ETElXv6XhDr1G1o4p0Uc9tlHNckRBZcNCEnFRkF-lIb0LOq1XRIfIaVXtT5fSgB90m5Oy1twOn0deyOO0hsSr_US1og5gkMQ4DcdgccqcV4DsSkxWwxTAgG5xFfAVW9CikOOPs9pmugc2TjLRBwrLtDtgbHKVilsl_mxfibYLjSrLH9Dtjp-k2yTOVNKHiF-iyjHJga31DioF9AE_A7g7MK3NgGj2tb2kDy7Da4BnYna5qXamappuWaZiaoavGsQ1-S3_tSld7mtrtWwIyu1av3wbiDBNH2Lw4SuWJevwDFQ0-7g)

**Each frame, in `UpdateGame()` and `GameLoop()`:**

1. **PollInput()**: read the keyboard and gamepad
2. **MediatorHandleCommand()**: pass the command to the player and the HUD
3. **ExecuteCommand()**: turn the command into events
4. **HandleEvent()**: look the event up in the current state's transition table
5. **ChangeState()**: if there's a match, `Exit()` the old state, then `Entry()` the new one
6. **UpdateState()**: run the current state's `Update()`, which also calls `UpdateAnimation()`
7. **PollAI()** and the same steps for the NPC
8. **Collision detection**: collision start, ongoing or end, with pushback and events
9. **AttackHit()**: does either sword swipe land this frame?
10. **Game over check**
11. **DrawGame()**: background, characters, health bars and the HUD

### Key Components

1. **Finite State Machine (FSM)**: state-based behaviour for game objects, with transitions set in tables
2. **Command Pattern**: input as bit flags, so several commands (and diagonal movement) work at once
3. **Animation System**: sprite-sheet animations, started by each state's `Entry()`
4. **Collision Detection**: circle colliders with cute_c2, plus hit-frame sword damage
5. **Mediator Pattern**: commands go to the game object and the HUD, which never know about each other
6. **Event System**: events drive every state change, including collisions and finished animations
7. **Cross-Platform Support**: Linux, Windows (w64devkit), WSL and the browser

---

## Core Systems

### 1. Finite State Machine (FSM)

The FSM manages each object's behaviour through states and transitions, kept in step with its animations.

**Available States:**

| State | Player | NPC |
|---|---|---|
| `STATE_IDLE` | Idle animations (picks a random one) | Idle animation |
| `STATE_WALKING` | Walks where you steer | Chases the player |
| `STATE_ATTACKING` | One sword swipe, feet planted | One sword swipe, when it catches the player |
| `STATE_SHIELD` | Set up, but no key does DEFEND yet | Shields when the AI decides to |
| `STATE_DEAD` | Not finished (see the tasks) | Death animation |
| `STATE_RESPAWN` | Not finished (see the tasks) | Not used |

**State Configuration:**

Each state has three 'protocols' (**function pointers**):

- **Entry**: called once when entering the **STATE** (sets up its animation, plays a sound, resets anything that starts fresh)
- **Update**: called every frame while in the **STATE** (updates the animation and the GameObject)
- **Exit**: called once when leaving the **STATE** (resets GameObject variables if required)

Which event leads to which state is set separately, in the state's **transition table**: a list of `{EVENT, STATE}` pairs. When an event arrives, `HandleEvent()` in `fsm.c` looks for it in the current state's table. If it's there, the object changes state; if not, nothing happens.

**Example State Setup:**

```c
// ---- STATE_IDLE state configuration ----
// Define valid transitions from STATE_IDLE
EventStateTransition idleValidTransitions[] = {
	// EVENT -> STATE
	{EVENT_MOVE, STATE_WALKING},
	{EVENT_ATTACK, STATE_ATTACKING},
	{EVENT_DEFEND, STATE_SHIELD},
	{EVENT_DIE, STATE_DEAD}};

// Set up the state configuration for STATE_IDLE
InitStateConfig(object, STATE_IDLE, "Player_Idle", PlayerEnterIdle, PlayerUpdateIdle, PlayerExitIdle);

// Configure valid transitions for STATE_IDLE
StateTransitions(&object->stateConfigs[STATE_IDLE], idleValidTransitions, sizeof(idleValidTransitions) / sizeof(EventStateTransition));
```

> **One table instead of two locks.** In the FSM practical, a state's `HandleEvent` function decided which state to ask for, and a separate list said which states were allowed. Here the table does both jobs: the event and the state it leads to sit side by side. `AllocateStateConfigs()` also sets every unused state to `UNIMPLEMENTED_STATE_CONFIG`, so a new state can't crash the game when it closes.

A state can also end itself. A sword swipe plays once; when it finishes, the state's `Update()` sends `EVENT_ANIMATION_END`, and the table decides what comes next:

```c
EventStateTransition attackValidTransitions[] = {
	// EVENT -> STATE
	{EVENT_ANIMATION_END, STATE_IDLE},
	{EVENT_DIE, STATE_DEAD}};
```

**FSM State Diagram:**

[![](https://mermaid.ink/img/pako:eNqNlF1vgjAUhv8KOZcLGhFB7MUSAt1GhrAI0WRjMY1UJRFqsCzbjP99-AU6VOSKHp6np-dNyhomLKSAYMUJp2ZEZimJG1_tIBHy5-PhU2g0HgXP1308tkwb7-vl-uTzSLdfLedZQAIeYscf993hLVz3fd04E_aVG4r3YmHbLHgTP2HHvMGbWD-hrcNpToXjmc-HLBzHdfBt_J4pqlbtIFXl8izXaMO1bcuzXKdQiso4JwZ-NYpykkoYehLFhEcsEQwWLxeU0zqxPvlDBHcGX6HrG5QRXOlRJvIv_Etmfb8dUQoD7L3pozL_w_rUOCJ35A0ixDSNSRTmV3W93SQAPqcxDQDlryGdkmzBAwiSTY6SjDPvJ5kA4mlGRUhZNpsDmpLFKl9ly7C86kV1SZJ3xuKjMku3rQ46TUKaGixLOCBJ3rGA1vANSFOaSrunqVJHaUk9RZZE-AHUkZuS2lW7eU2TpZaibET43W3eamqSrEptVZU1Re51NUUEGkacpf39b2j3N9r8Ab3WRuc?type=png)](https://mermaid.live/edit#pako:eNqNlF1vgjAUhv8KOZcLGhFB7MUSAt1GhrAI0WRjMY1UJRFqsCzbjP99-AU6VOSKHp6np-dNyhomLKSAYMUJp2ZEZimJG1_tIBHy5-PhU2g0HgXP1308tkwb7-vl-uTzSLdfLedZQAIeYscf993hLVz3fd04E_aVG4r3YmHbLHgTP2HHvMGbWD-hrcNpToXjmc-HLBzHdfBt_J4pqlbtIFXl8izXaMO1bcuzXKdQiso4JwZ-NYpykkoYehLFhEcsEQwWLxeU0zqxPvlDBHcGX6HrG5QRXOlRJvIv_Etmfb8dUQoD7L3pozL_w_rUOCJ35A0ixDSNSRTmV3W93SQAPqcxDQDlryGdkmzBAwiSTY6SjDPvJ5kA4mlGRUhZNpsDmpLFKl9ly7C86kV1SZJ3xuKjMku3rQ46TUKaGixLOCBJ3rGA1vANSFOaSrunqVJHaUk9RZZE-AHUkZuS2lW7eU2TpZaibET43W3eamqSrEptVZU1Re51NUUEGkacpf39b2j3N9r8Ab3WRuc)

**The NPC's tables** use the collision events: touching the player while walking or idle starts a swipe, and the swipe sends it back to chasing.

```c
EventStateTransition walkingValidTransitions[] = {
	// EVENT -> STATE
	{EVENT_NONE, STATE_IDLE},
	{EVENT_COLLISION_START, STATE_ATTACKING},
	{EVENT_DEFEND, STATE_SHIELD},
	{EVENT_DIE, STATE_DEAD}};
```

### 2. Event System

Events trigger state transitions. They're defined in `events.h`:

```c
typedef enum
{
    EVENT_NONE,             // No action (no movement, no attack)
    EVENT_MOVE,             // Movement input (triggers Walk)
    EVENT_ATTACK,           // Attack action (triggers a sword swipe)
    EVENT_DEFEND,           // Shield action (triggers Shield)
    EVENT_DIE,              // Death condition (triggers Death)
    EVENT_RESPAWN,          // Respawn trigger (triggers Respawn)
    EVENT_COLLISION_START,  // Collision begins
    EVENT_COLLISION_END,    // Collision ends
    EVENT_ANIMATION_END,    // An animation that plays once (a sword swipe) has finished
    EVENT_COUNT             // Total event count
} Event;
```

`events.h` also lists commented-out events (`EVENT_JUMP`, `EVENT_HURT`, `EVENT_PICKUP`…) as ideas for your own game. Add new events **before** `EVENT_COUNT`.

### 3. Command

The command system supports directional movement and other input commands with bitwise flags, as in the Command Pattern practical:

**Command Flags:**

```c
// Define the Command enum
typedef enum
{
	NONE 				= 0,		// No command (used to represent idle state)
	MOVE_UP 			= 1 << 0,	// Binary: 000001 Command to move up
	MOVE_DOWN 			= 1 << 1,	// Binary: 000010 Command to move down
	MOVE_LEFT 			= 1 << 2,	// Binary: 000100 Command to move left
	MOVE_RIGHT 			= 1 << 3,	// Binary: 001000 Command to move right
	ATTACK				= 1 << 4,	// Binary: 010000 Command to perform an attack action (e.g., slash with sword)
	DEFEND				= 1 << 5,	// Binary: 100000 Command to perform an defend action (e.g., defend with sheild)
	COMMAND_COUNT 		= 6			// Total number of commands, useful for looping or limits
} Command;
```

`ExecuteCommand()` in `command.c` turns them into events:

- any movement bit sets `inputAxis` and the facing direction, and sends `EVENT_MOVE` (unless attacking),
- no movement and no action sends `EVENT_NONE`,
- `ATTACK` sends `EVENT_ATTACK`, and `DEFEND` sends `EVENT_DEFEND`.

During a sword swipe the facing direction doesn't change, so a swipe can't turn halfway through.

### 4. Animation System

The animation system plays sprite-sheet frames, kept in step with the FSM: each state's `Entry()` sets up its animation, and its `Update()` moves it on.

**Animation Structure:**

```c
typedef struct
{
    Texture2D texture;   // Animated Sprite Sheet Texture
    Rectangle *frames;   // Array of frames (rectangles)
    int currentFrame;    // Current frame index
    int frameCount;      // Total number of frames
    float frameDuration; // Duration of each frame
    float frameTimer;    // Timer to track frame duration
    bool active;         // Is the animation active?
    bool loop;           // Should the animation loop?
    bool finished;       // Has an animation that doesn't loop reached its end?
} AnimationData;
```

**Key Features:**

- Sprite sheet support with configurable frame rectangles (animation atlas)
- Frame-based timing system
- Looping animations (idle, walk) and animations that play once (sword swipe)
- `finished`, so a state knows when a one-off animation is done
- Different frame durations for each animation

**Animation Integration:**

```c
// In a state's Entry(): set up its animation
InitGameObjectAnimation(object, walk_UP, 9, 0.055f);                 // loops
InitGameObjectAnimationOnce(object, sword_attack_UP, 6, 0.035f);     // plays once

// In a state's Update(): move the animation on
UpdateAnimation(&object->animation, deltaTime);

if (object->animation.finished)                                      // a one-off animation is done
    HandleEvent(object, EVENT_ANIMATION_END, deltaTime);

// In DrawGame(): draw the current frame, centred on the GameObject
DrawAnimation(&data->player->base.animation, data->player->base.position, WHITE);
```

### 5. Collision Detection

Circle-based collision using the [cute_c2](https://github.com/RandyGaul/cute_headers/blob/master/cute_c2.h) library:

**Collision Features:**

- Circle-to-circle collision detection
- Collision manifolds for collision response
- Collision events integrated with the FSM (the NPC attacks on `EVENT_COLLISION_START`)
- Pushback / separation response
- Sword damage on the hit frame, blocked by a shield

**Collision Flow:**

```c
// Colliders follow the GameObjects: GameObjectMove() in utils.c keeps them in step

// Check collision
bool isColliding = CheckCollision(&data->player->base, &data->npc->base);

// ENTER: first frame of the collision
if (isColliding && !data->player->base.isColliding)
{
    HandleEvent(&data->player->base, EVENT_COLLISION_START, deltaTime);
    HandleEvent(&data->npc->base, EVENT_COLLISION_START, deltaTime);

    CollisionEntry(&data->player->base, &data->npc->base);
    HandleCollision(&data->player->base, &data->npc->base);   // pushback
}

// ... EXIT and ONGOING collision

// Sword swipes: damage lands once per swipe, on the hit frame
AttackHit(&data->player->base, &data->npc->base);
AttackHit(&data->npc->base, &data->player->base);
```

**How a sword hit works** (`AttackHit()` in `collision.c`): bumping into each other doesn't hurt. A swipe can land **once**, on the frame the blade comes round (`ATTACK_HIT_FRAME`, frame 4 of 6), and only if the target is:

- in reach (the two colliders plus `ATTACK_REACH`),
- in front of the attacker,
- not shielding.

The damage, reach and hit frame are all in `constants.h`.

### 6. GameObject

**GameObject Structure:**

```c
struct GameObject
{
	const char *name; // The name of the game object (e.g., "Player", "Enemy")

	// Positions and Transforms
	Vector2 position;			 // Gameobjects position in the game world
	Vector2 inputAxis;			 // Input Axis (Arrow Keys / D-Pad and Thumbstick)
	Direction currentDirection;	 // Used to face animations in correct direction
	Direction previousDirection; // Used to face animations in correct direction

	// Collision components
	c2Circle collider;	 // Circle collider used for collision detection
	c2Manifold manifold; // Manifold for finding incident normals
	bool isColliding;	 // Is Colliding

	// FSM
	StateConfig *stateConfigs; // Pointer to the array of state configurations for this game object
	State previousState;	   // The state the game object was previously in
	State currentState;		   // The current state of the game object

	// Animation
	AnimationData animation; // Player Animation
	Color color;			 // Gameobject's color, changes based on currentState
	Texture2D keyframes;	 // Sprite Sheet Texture

	// Gameplay Components
	int health;		// The health of the game object
	bool attackHit; // Has the current sword swipe already landed (one hit per swipe)
	float timer; // Can be used during updates
};
```

(`typedef struct GameObject GameObject;` is in `fsm.h`, so the FSM can use it before the struct is defined.)

**Player and NPC Entities:**

- **Player**: controlled by keyboard or gamepad. Has `stamina` and `mana`, ready for you to use.
- **NPC**: controlled by the AI manager. Has `aggression` (it gets faster the longer it chases you), a `target` to chase, and `facing`.

Both use the same FSM system and animation pipeline, but have their own states, tables and animations. Because `base` is the **first** member of both, a `Player *` can be cast to a `GameObject *` and back.

### 7. Input System

**Keyboard and gamepad mapping:**

- Arrow keys, D-pad or left thumbstick: directional movement
- S, Z, J or SPACE, or the right trigger: attack (sword)
- D, X or K: shield (not bound yet)
- M, C or L: Mega Blast (not bound yet, and it needs its own command)

**Input Processing:**

```c
Command PollInput()
{
	Command command = NONE;

	// ... gamepad first (D-pad, thumbstick, right trigger)

	// Movement: arrow keys
	if (IsKeyDown(KEY_UP))
		command |= MOVE_UP;
	if (IsKeyDown(KEY_DOWN))
		command |= MOVE_DOWN;
	if (IsKeyDown(KEY_LEFT))
		command |= MOVE_LEFT;
	if (IsKeyDown(KEY_RIGHT))
		command |= MOVE_RIGHT;

	// Sword: S, Z, J or SPACE (held down: one swipe after another)
	if (IsKeyDown(KEY_S) || IsKeyDown(KEY_Z) || IsKeyDown(KEY_J) || IsKeyDown(KEY_SPACE))
		command |= ATTACK;

	// Shield: D, X or K .... not bound yet: that's one for you

	// Mega Blast: M, C or L .... not bound yet: it needs its own command, event and state

	// ... logs the command bits when they change

	return command;
}
```

The sword keys use `IsKeyDown`, so holding one gives one swipe after another. Each swipe always plays to the end before the next one starts.

---

## Animation Sprite Sheets

Player and NPC sprite sheets are in `resources/` (`player_sprite_sheet.png` and `npc_sprite_sheet.png`). The `assets/production/` folder has <a href="./assets/production/grid_player_sprite_sheet.png">grid_player_sprite_sheet</a>, with each animation laid out on a numbered grid, to help you pick the right frames. It's kept out of `resources/`, so it isn't packaged into the web build.

On entry to a state, the entry method sets up the animation atlas (coordinates) for that state's animation. The example below is `InitWalkAnimation`.

### Setting Up New Animations

Follow these steps to correctly extract animation frames from the sprite sheet and configure them in the game.

#### Step 1: Identify the Animation

Find the right animation row on the production grid (e.g. Walk Up, Idle, Attack, Death).

#### Step 2: Identify the Starting Position (x, y) for Frame 1

Each frame begins at the top-left corner of its cell on the sprite sheet.

- Most player and NPC animations use 64x64 sprites.
- Sword attack animations use 192x192 sprites, exactly 3x3 grid cells (3 x 64 = 192).
- Frame sizes vary based on your chosen sprite sheet.

> **Important:** All frames in a given animation must use the same width and height.

Example: Standard Walk Animation (64x64)
<img src="./assets/production/InitWalkAnimation.png" alt="InitWalkAnimation Animation Sprites" />

Example: Walk Up Animation (64x64), from `player.c`:

```c
// WALK UP (Row 9, Pixel : Y = 8 x 64 = 512)
// Each frame is 64 x 64, so Pixel X = (Column - 1) x 64
static Rectangle walk_UP[9] = {
	{0, 512, 64, 64},     // Frame 1: Column 1 (Pixel : X 0)
	{64, 512, 64, 64},    // Frame 2: Column 2 (Pixel : X 64)
	{128, 512, 64, 64},   // Frame 3: Column 3 (Pixel : X 128)
	{192, 512, 64, 64},   // Frame 4: Column 4 (Pixel : X 192)
	{256, 512, 64, 64},   // Frame 5: Column 5 (Pixel : X 256)
	{320, 512, 64, 64},   // Frame 6: Column 6 (Pixel : X 320)
	{384, 512, 64, 64},   // Frame 7: Column 7 (Pixel : X 384)
	{448, 512, 64, 64},   // Frame 8: Column 8 (Pixel : X 448)
	{512, 512, 64, 64}    // Frame 9: Column 9 (Pixel : X 512)
};
```

Example: Sword Attack Animation (192x192), from `player.c`:

```c
// ATTACK UP (Rows 47-49, Pixel : Y = 46 x 64 = 2944)
// Each frame is 192 x 192, so Pixel X = (Column - 1) x 192
static Rectangle sword_attack_UP[6] = {
	{0, 2944, 192, 192},    // Frame 1: Column 1 (Pixel : X 0)
	{192, 2944, 192, 192},  // Frame 2: Column 2 (Pixel : X 192)
	{384, 2944, 192, 192},  // Frame 3: Column 3 (Pixel : X 384)
	{576, 2944, 192, 192},  // Frame 4: Column 4 (Pixel : X 576)
	{768, 2944, 192, 192},  // Frame 5: Column 5 (Pixel : X 768)
	{960, 2944, 192, 192}   // Frame 6: Column 6 (Pixel : X 960)
};
```

> **Note:** Since 192px = 3 x 64px, sword attack frames advance by 192px along the x-axis (0, 192, 384, 576...). But they start on the **64px** grid going down (rows 47–49 start at y = 2944), not on a 192px one: 2944 isn't a multiple of 192.

### Initialising an Animation

All animations are started with one of:

```c
InitGameObjectAnimation(object, frameArray, frameCount, frameSpeed);       // loops (idle, walk)
InitGameObjectAnimationOnce(object, frameArray, frameCount, frameSpeed);   // plays once (sword swipe)
```

- `object`: Player, NPC, or any animated GameObject
- `frameArray`: the Rectangle array defining the animation frames
- `frameCount`: number of frames in that array
- `frameSpeed`: time between frames (lower = faster animation)

Complete Example:

```c
// Define the frame array
static Rectangle walk_UP[9] = {
	{0, 512, 64, 64},
	{64, 512, 64, 64},
	// ... remaining frames
};

// Initialise the animation
InitGameObjectAnimation(object, walk_UP, 9, 0.055f);
```

Timing Guidelines:

| Animation Type           | Speed (float)     | Use Case                    |
| ------------------------ | ----------------- | --------------------------- |
| Walk / Run               | `0.05f - 0.07f`   | Character movement          |
| Attack / Sword           | `0.03f - 0.05f`   | Fast action animations      |
| Other Actions            | `0.08f - 0.12f`   | General purpose animations  |
| Subtle animations        | `0.10f - 0.20f`   | Idle, breathing, ambient    |

> **Tip:** Start with the recommended ranges above and adjust based on testing and how it feels.

### Coordinate Calculation Rules

The grid on the production sheets counts **from 1**, and so do the comments in `player.c` and `npc.c`.

#### For 64x64 Animations:

- **Formula:**
    - `x = (column - 1) x 64` (e.g. column 4 -> x = 192)
    - `y = (row - 1) x 64` (e.g. row 9 -> y = 512)

#### For 192x192 Sword Animations:

- **Formula:**
    - `x = (column - 1) x 192` (e.g. column 3 -> x = 384)
    - `y = (first grid row - 1) x 64` (e.g. rows 47–49 -> y = 2944)

> **Critical:** All frames in a single animation must have identical width and height, otherwise the sprite atlas will not render correctly.

### Naming Conventions

**Recommended patterns for frame arrays:**

```c
static Rectangle walk_UP[9];          // Direction-based movement
static Rectangle sword_attack_UP[6];  // Action-based animations
static Rectangle idle_DOWN[4];        // State-based animations
static Rectangle death_GENERIC[8];    // Generic animations
```

> Use descriptive, UPPERCASE suffixes for clarity (this will help you when debugging).

### Pitfalls & Debugging

#### Animation Issues

- **Frame "jumps" or glitches:** the width or height is probably wrong. Check all frames use the same size.
- **Animation doesn't advance:**
    - Confirm `frameCount` matches the actual number of frames in your array
    - Make sure the state's entry function calls `InitGameObjectAnimation` (or `…Once`)
    - Make sure the state's update function calls `UpdateAnimation`
- **Wrong frames display:** double-check the x and y offsets against the grid. Animation bugs are often misaligned rectangles.
- **Character slides:** the state moves the character, but the animation has no walking legs (sword swipes don't). Don't move in that state.

#### Large Sprite Handling (192x192)

When using large sprites like _sword attacks_:

- **Render offset:** `DrawAnimation()` centres every frame on the character, so a 192x192 frame lines up with a 64x64 one
- **Collider offset:** the collider stays the character's size, not the full sprite; sword reach is `ATTACK_REACH`, not the sprite size
- **Visual drift:** large sprites can appear to _"float"_ if their rectangles are a few pixels off the grid

#### Verification Checklist

- [ ] All frames in the animation have identical width / height
- [ ] X-coordinate advances by the sprite width (64 or 192)
- [ ] Y-coordinate is the same for all frames in the row, and on the 64px grid
- [ ] `frameCount` parameter matches array length
- [ ] `frameSpeed` is within a reasonable range (0.03f - 0.20f)

---

## The tasks

Already working in the _StarterKit_:

- [x] Player: Idle (random idle animations), Walking and Attacking, with animations in four directions
- [x] Sword swipes that stand still, play once, and land damage on the hit frame
- [x] NPC: chases the player (Walking), swipes when it catches you, shields when the AI decides
- [x] Shield blocks damage
- [x] Health bars, game over, and R to restart
- [x] HUD: command bits, the player's state and the active commands
- [x] Action sounds: swipe, hit, block and game over
- [x] A new background every game

The following tasks are suggested to help with your project.

### Task 1: Implement Player Combat Actions

**Objective:** Create the three core player actions with animations and FSM integration.

1. **Add combat states to `fsm.h`** (before `STATE_COUNT`):

   ```c
   STATE_SWORD_ATTACK,   // Plasma Sword attack
   STATE_SHIELD_DEFEND,  // Energy Shield defense
   STATE_MEGA_BLAST,     // Mega Blast special
   ```

2. **Implement state functions in `player.c`:**

   ```c
   // Plasma Sword Attack
   void PlayerEnterSword(GameObject *object, float deltaTime);
   void PlayerUpdateSword(GameObject *object, float deltaTime);
   void PlayerExitSword(GameObject *object, float deltaTime);

   // Energy Shield Defense
   void PlayerEnterShield(GameObject *object, float deltaTime);
   void PlayerUpdateShield(GameObject *object, float deltaTime);
   void PlayerExitShield(GameObject *object, float deltaTime);

   // Mega Blast
   void PlayerEnterMegaBlast(GameObject *object, float deltaTime);
   void PlayerUpdateMegaBlast(GameObject *object, float deltaTime);
   void PlayerExitMegaBlast(GameObject *object, float deltaTime);
   ```

3. **Configure animations for each action using the provided Alien/Robot sprite sheets.** Then give each state its `InitStateConfig()` and transition table in `InitPlayerFSM()`, and add the new states to the tables of the states they can be reached from.

4. **Add commands and events.** Each action needs a command bit in `command.h` (update `COMMAND_COUNT`), an event in `events.h`, and a line in `ExecuteCommand()` that sends the event. Then bind the keys in `input_manager.c`:

   ```c
   if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_J))
       command |= SWORD;
   if (IsKeyDown(KEY_D) || IsKeyDown(KEY_X) || IsKeyDown(KEY_K))
       command |= SHIELD;
   if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_C) || IsKeyPressed(KEY_L))
       command |= MEGA;
   ```

   Use the keys your project brief asks for (S / D / M), and keep the Z / X / C and J / K / L alternatives. Movement is on the arrow keys, so none of them clash.

   > **`IsKeyPressed` or `IsKeyDown`?** A one-off action, like a blast, uses `IsKeyPressed`: true once, on the frame the key goes down. Something you hold, like a shield, uses `IsKeyDown`: true every frame the key is held.

### Task 2: Implement Enemy AI with FSM

**Objective:** Create strategic AI for the Alien NPC using a Finite State Machine.

1. **Design Alien AI states in `npc.c`.** Some of these already exist:

   - `STATE_IDLE`: waiting / observing the player (done)
   - `STATE_CHASE`: moving toward the player (done, as `STATE_WALKING`)
   - `STATE_ATTACKING`: executing an attack (done)
   - `STATE_DEFENDING`: blocking / retreating (blocking done, as `STATE_SHIELD`)
   - `STATE_DEAD`: death state (animation done; nothing sends `EVENT_DIE` yet)

2. **Implement AI decision-making in `ai_manager.c`.** At the moment `PollAI()` picks a random command once a second. Make it smarter:

   - Distance-based state transitions
   - Health-based defensive behaviour
   - Attack pattern variations
   - Strategic timing for attacks

3. **Add state transition logic.** For example, based on the distance to the player:

   ```c
   // Example: Transition based on distance to player
   float distanceToPlayer = Vector2Distance(npc->base.position, player->base.position);

   if (distanceToPlayer < ATTACK_RANGE)
   {
       HandleEvent(&npc->base, EVENT_ATTACK, deltaTime);
   }
   else if (distanceToPlayer < CHASE_RANGE)
   {
       HandleEvent(&npc->base, EVENT_MOVE, deltaTime);
   }
   ```

   Send **events** rather than calling `ChangeState()` directly, so the transition tables stay in charge. Remember to add the matching `{EVENT, STATE}` pairs to the NPC's tables.

### Task 3: Extend the Health and Collision System

**Objective:** Build on the health and hit system that's already there.

Already working: health bars for both, damage on the hit frame of a sword swipe, and shield blocking (`AttackHit()` in `collision.c`).

1. **Death and respawn:**

   - Send `EVENT_DIE` when health reaches 0, so the Dead state's animation plays before game over
   - Finish the player's Dead and Respawn states, which change state immediately at the moment
   - Health regeneration while shielding (optional)

2. **Collision response in `collision.c`:**

   - Damage based on attack type (sword, mega blast)
   - Block detection for each defence
   - Knockback effects (for advanced implementations)

### Task 4: Add Visual and Audio Feedback

**Objective:** Provide clear feedback for player and NPC actions.

1. **Visual feedback (Options):**

   - Hit effects / particles on successful attacks
   - Shield glow during defence
   - Screen shake on heavy hits
   - Colour changes on damage
   - Victory / defeat animations

2. **Audio feedback (Options).** The audio manager already plays placeholder sounds for swipes, hits, blocks and game over:

   - Replace the placeholder `.wav` files in `resources/sounds/` with your own ([rFXGen](https://raylibtech.itch.io/rfxgen) by raysan5, or [Kenney](https://kenney.nl/assets/category:Audio))
   - Shield and magic cast sounds (add to `ActionSound` in `audio_manager.h` and to `soundFiles[]`)
   - Background music (raylib's `LoadMusicStream()`, updated every frame with `UpdateMusicStream()`)

   Play one-off sounds from a state's `Entry()`, like the swipe sound, so they play once per action.

### Task 5: Implement Combat Mode

**Objective:** Choose and implement either turn-based or real-time combat.

**Option A: Turn-Based Combat**

1. Implement a turn system:

   - Player turn: wait for input (S / D / M)
   - Execute the player's action with its animation
   - Enemy turn: AI decides its action
   - Execute the enemy's action with its animation
   - Repeat until game over

2. Add turn indicators and UI

**Option B: Real-Time Melee Combat**

1. Build on the real-time loop that's already there:

   - Both the player and NPC can act at the same time
   - Add cooldowns for actions
   - Implement timing-based combat
   - Add dodge / movement mechanics

2. Balance action speeds and cooldowns

**MAKE SURE TO BUILD A STATE DIAGRAM e.g.**

[![](https://mermaid.ink/img/pako:eNqNlN9LwzAQx_-VkEdZX3zsgyCrysAp2IGg9SFrrm00P0qaTMbY_26SrjYb6zQP5XL55PK95K47XCoKOMWdIQYyRmpNRLK5LiRy4_3qAyXJDVpQDihFC8kMIxzlnu2J_hs2owci4Hn9CaVBu97vR9jrg7wS_sVk7eIs1QbQ3QakOYPdGkPKA9jbk2jeMODUcRlUIOkpN1rD0VEuTyrIEEc7Yu5vHTH9bylj2EjMIf5ciZaDATqFn1xhZrWf9cy5ww6qjtNeNRqIOQvFOc-VlQb0hei_75ABCakzuHRF09RxktNcpHQaGq3AePgFupZ8S8cP1smewR1d1OAa3qSH93HVS-WKXrO6MUhVUfVHgkNfrDSRnescJTsXThqtOAeK1tt0JBM0t1o7UXFz9QsLWSrhLyeI7uKl-3yJHlXNyt7pC8-LwjMsQAvCqGvs0IoFNo2r9QKnzqRQEctNgQu5dyixRuVbWeLUaAszrJWtG5xWhHduZls6_hgGpCXyTSlxgPY_DLtCBw?type=png)](https://mermaid.live/edit#pako:eNqNlN9LwzAQx_-VkEdZX3zsgyCrysAp2IGg9SFrrm00P0qaTMbY_26SrjYb6zQP5XL55PK95K47XCoKOMWdIQYyRmpNRLK5LiRy4_3qAyXJDVpQDihFC8kMIxzlnu2J_hs2owci4Hn9CaVBu97vR9jrg7wS_sVk7eIs1QbQ3QakOYPdGkPKA9jbk2jeMODUcRlUIOkpN1rD0VEuTyrIEEc7Yu5vHTH9bylj2EjMIf5ciZaDATqFn1xhZrWf9cy5ww6qjtNeNRqIOQvFOc-VlQb0hei_75ABCakzuHRF09RxktNcpHQaGq3AePgFupZ8S8cP1smewR1d1OAa3qSH93HVS-WKXrO6MUhVUfVHgkNfrDSRnescJTsXThqtOAeK1tt0JBM0t1o7UXFz9QsLWSrhLyeI7uKl-3yJHlXNyt7pC8-LwjMsQAvCqGvs0IoFNo2r9QKnzqRQEctNgQu5dyixRuVbWeLUaAszrJWtG5xWhHduZls6_hgGpCXyTSlxgPY_DLtCBw)

Click the diagram to open it in the Mermaid Live Editor, then change it to match your game.

---

## Cheat sheet

```c
// The FSM functions (fsm.c)
HandleEvent(obj, EVENT_ATTACK, deltaTime);   // send an event: the current state's table decides
ChangeState(obj, STATE_IDLE, deltaTime);     // change state directly (must still be in the table)
UpdateState(obj, deltaTime);                 // run the current state's Update
PrintStateConfigs(obj->stateConfigs, STATE_COUNT);   // print every state and its table

// Where am I?
obj->currentState                            // e.g. STATE_ATTACKING
obj->stateConfigs[obj->currentState].name    // e.g. "Player_Attacking"

// Setting up a state
EventStateTransition xValidTransitions[] = {
	// EVENT -> STATE
	{EVENT_NONE, STATE_IDLE},
	{EVENT_DIE, STATE_DEAD}};
InitStateConfig(object, STATE_X, "Player_X", PlayerEnterX, PlayerUpdateX, PlayerExitX);
StateTransitions(&object->stateConfigs[STATE_X], xValidTransitions, sizeof(xValidTransitions) / sizeof(EventStateTransition));

// Animations
InitGameObjectAnimation(object, frames, count, 0.055f);       // loops
InitGameObjectAnimationOnce(object, frames, count, 0.035f);   // plays once, then animation.finished
UpdateAnimation(&object->animation, deltaTime);

// Sounds
PlayActionSound(SOUND_SWIPE);                // SOUND_HIT, SOUND_BLOCK, SOUND_GAMEOVER

// Input
IsKeyDown(KEY_UP);                           // true every frame the key is held (walking, shield)
IsKeyPressed(KEY_M);                         // true once, on the frame it goes down (one-off actions)
```

---

## When things go wrong

Don't panic. Nine times out of ten it's one of these:

| You see… | What's going on | Fix |
|---|---|---|
| `raylib.h: No such file or directory` | make can't find raylib | Run the setup script again (Step 2) |
| `gameobjects/player.h: No such file or directory` | An include is missing its folder, or the file is in the wrong folder | Headers go in `include/<folder>/`, included as `"folder/file.h"` |
| A state never starts | Its event isn't in the current state's transition table | Add the `{EVENT, STATE}` pair to the table |
| `Invalid state transition from A to B` | `ChangeState()` was called for a state not in A's table | Add B to A's table, or send an event instead |
| A state never ends | Nothing in its `Update()` sends an event or changes state | End it on a timer, or on `EVENT_ANIMATION_END` |
| The character slides | The state moves the character, but the animation has no walking legs | Don't move in that state (see Attacking) |
| Animation stuck on one frame | `UpdateAnimation()` isn't called in the state's `Update()`, or the animation is set up again every frame | Set it up once in `Entry()`, update it in `Update()` |
| Something happens 60 times a second | One-off code (a sound, damage) in `Update()` | Move it to `Entry()`, or use a flag like `attackHit` |
| A new key does two things | It's already used by another command | Check `PollInput()`: movement is on the arrows, and each letter in the key table does one thing |
| No sound | No audio device (WSL, some lab machines), or the browser is waiting for a key press | The game still works; press a key first in the browser |
| Sprites don't show in the browser | The sprite sheet isn't in `resources/` | Everything the game loads must be in `resources/` |
| Wrong frames or a frame "jumps" | A rectangle is off the grid | Check it with the formulas in [Coordinate Calculation Rules](#coordinate-calculation-rules) |
| `Emscripten not found in …` (Linux) | Emscripten isn't in `~/emsdk` | Run the setup script (Step 2) |
| `emcc: No such file or directory` (Windows) | Emscripten isn't loaded in this terminal | Close w64devkit and open it again |
| `Address already in use` | A web server from earlier is still running | `make stop_web`, or use another port: `make run_web WEB_PORT=8080` |
| Browser says it can't connect | The page opened before the server was ready | Refresh the page |
| The program still won't build | Old leftover files | `make clean`, then `make` again |

Still stuck? Read the **first** error message, not the last. The first one is usually the real problem; the rest are just complaining about it.

### Memory Management

Every `malloc` needs a `free`, and every `Load…` needs an `Unload…`. Check for leaks on Linux with valgrind, using a debug build:

```sh
# Build with debug info
make BUILD_MODE=DEBUG

# Run the game and check for leaks
valgrind ./build/desktop-debug/GPPI_project

# Valgrind with suppression file
valgrind -s \
  --leak-check=full \
  --show-leak-kinds=definite \
  --track-origins=yes \
  --suppressions=./gpp.supp \
  ./build/desktop-debug/GPPI_project
```

The suppression file hides leaks reported inside the graphics and audio drivers, which aren't yours. Play for a bit, restart with R a few times, then close the game: the starter kit frees everything it creates, so anything valgrind reports as "definitely lost" is from your code.

---

## Further reading

- [raylib](https://www.raylib.com): the [cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html) and [examples](https://www.raylib.com/examples.html)
- [cute_headers](https://github.com/RandyGaul/cute_headers) by Randy Gaul: cute_c2 and friends
- [Game Programming Patterns](http://gameprogrammingpatterns.com) by Robert Nystrom: especially [State](https://gameprogrammingpatterns.com/state.html) and [Command](https://gameprogrammingpatterns.com/command.html)
- [rFXGen](https://raylibtech.itch.io/rfxgen) by raysan5: make your own sound effects

Who do I talk to? **muddygames**

---

## Before you submit

- [ ] It compiles and runs with no errors or warnings
- [ ] Your player has its new combat actions, each with its own state, animation and command
- [ ] Your NPC makes decisions, not just random choices
- [ ] Death works: health at 0 plays the Dead state before game over
- [ ] Your actions have visual and audio feedback
- [ ] Your state diagram matches your transition tables
- [ ] valgrind reports nothing "definitely lost" from your code
- [ ] Your code is commented and tidy

*This _StarterKit_ was created to help students understand game development patterns and create their own projects.*

Commit early and commit often. Your deadline, like the sword swipe, always plays to the end.

Best of luck. Now go make something worth fighting for.
