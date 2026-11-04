# Practical 9: The Finite State Machine (FSM) Pattern

In this practical, the player and an NPC each get a **finite state machine**. At any moment each one is in exactly one **state** (Idle, Moving, Firing, Jumping, Dead or Respawn), and it only changes state when an **event** arrives and its **transition table** allows the move. It's the last two practicals put together: the **Command** and **Mediator** patterns handle input, **cute_c2** handles collisions, and the FSM decides what each object does about it all.

Your job is to fix a transition that's missing, make the player and NPC die and come back, add states and commands of your own, and give the NPC a bit of a brain.

By the end you'll understand why games use state machines instead of a pile of `bool isJumping, isFiring, isDead` flags, and how function pointers let each state bring its own behaviour.

> The project was generated with [raylib project creator](https://raysan5.itch.io/raylib-project-creator) by raysan5.

---

## Short version

If you just want to get going, this is the whole thing in a few lines:

**1. Clone**

```bash
mkdir -p ~/Projects
cd ~/Projects
git clone https://github.com/MuddyGames/fsm_pattern
```

**2. Set up raylib**

*Linux:*

```bash
cd ~/Projects/fsm_pattern/setup
bash setup_linux.sh
```

or make the script executable and run it directly:

```bash
cd ~/Projects/fsm_pattern/setup
chmod +x setup_linux.sh
./setup_linux.sh
```

*Windows* (install [raylib](https://raysan5.itch.io/raylib) first, and then use w64devkit terminal):

```bash
cd ~/Projects/fsm_pattern/setup
sh setup_windows.sh
```

**3. Build and run**

```bash
cd ~/Projects/fsm_pattern
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
git clone https://github.com/MuddyGames/fsm_pattern
```

Your project now lives in `~/Projects/fsm_pattern`.

---

## Step 2: Install raylib and Emscripten (once only)

If you set these up for an earlier practical, skip this step. They're shared by every raylib project.

### Linux (Debian, Ubuntu, Pop!_OS or Arch)

```bash
cd ~/Projects/fsm_pattern/setup
bash setup_linux.sh
```

It will ask for your password to install packages. It takes a few minutes, especially on lab machines.

### Windows

Install the **[raylib installer](https://raysan5.itch.io/raylib)** and keep the default location, `C:\raylib`. Then in the **w64devkit** terminal:

```bash
cd ~/Projects/fsm_pattern/setup
sh setup_windows.sh
```

When it finishes, close w64devkit and open it again.

> No more MSYS2. Everything now runs in the w64devkit terminal that comes with the raylib installer.

For the manual steps, see the [drawing_primitives README](https://github.com/MuddyGames/drawing_primitives#manual-setup-only-if-the-script-didnt-work-for-you).

---

## Step 3: Build and run

Every command runs from the project folder:

```bash
cd ~/Projects/fsm_pattern
```

### On your desktop

The same commands work on Linux and Windows (w64devkit):

| Command | What it does |
|---|---|
| `make` | Builds the game |
| `make run` | Builds it and runs it |
| `make BUILD_MODE=DEBUG` | Builds with debug info, for stepping through in the debugger |
| `make clean` | Deletes the `build` folder, so the next build starts fresh |

A window should open with two circles: the **player** (green, in the middle) and an **NPC guard** (grey, top right). The HUD in the top left shows the command bits and the player's current state.

| Key | Action |
|---|---|
| WASD or arrow keys, D-pad | Move |
| SPACE, or the bottom face button | Jump |
| Left mouse button, or the right trigger | Fire |

Bump into the NPC and both health bars drop.

Keep the terminal where you can see it. Every state change is printed there, and it's the best way to watch the FSM working.

### In your browser

```bash
make run_web
```

This builds the web version, starts a small local web server and opens
**<http://127.0.0.1:8000/fsm_pattern.html>** in your browser.

Press **Ctrl+C** in the terminal to stop the server.

| Command | What it does |
|---|---|
| `make web` | Builds the web version only |
| `make run_web` | Builds it and opens it in your browser |
| `make stop_web` | Stops a web server left running from earlier |

In the browser, the state changes appear in the developer console (**F12**).

### In VS Code

Open `projects/VSCode/main.code-workspace` (**File → Open Workspace from File…**).

- **Ctrl+Shift+B** builds.
- **F5** builds and starts the debugger. The first time, choose **Debug** in the Run and Debug drop-down.
- **Terminal → Run Task** lists Run, Run Web, Stop Web Server and Clean.

Set a breakpoint in `ChangeState()` in `fsm.c` and press SPACE. Look at `obj->currentState` and `newState`, then step into `currentConfig->Exit` and watch it land in the right state's function. That's function pointers doing their job.

---

## How the project is laid out

```
fsm_pattern/
├── Makefile               build instructions
├── src/
│   ├── main.c             opens the window, runs the game loop
│   ├── game.c             sets up, updates and draws the game; checks collisions
│   ├── fsm.c              the state machine: HandleEvent, ChangeState, UpdateState
│   ├── gameobject.c       what the player and NPC have in common
│   ├── player.c           the player's states and transition tables
│   ├── npc.c              the NPC's states and transition tables
│   ├── input_manager.c    turns key, mouse and button presses into commands
│   ├── ai_manager.c       makes up commands for the NPC
│   ├── mediator.c         passes each command to a game object and the HUD
│   ├── command.c          ExecuteCommand(): turns command bits into FSM events
│   ├── hud.c              shows the command bits and the player's state on screen
│   ├── collision.c        collision checks, pushback and damage
│   └── cute_c2_impl.c     builds the cute_c2 collision library; leave it alone
├── include/
│   ├── fsm.h              the State enum and the StateConfig struct
│   ├── events.h           the Event enum
│   ├── command.h          the command bit flags
│   ├── gameobject.h       the GameObject struct
│   ├── player.h, npc.h    Player and NPC structs, and their state functions
│   ├── hud.h              Hud struct
│   ├── constants.h        screen size, damage and other constants
│   ├── cute_c2.h          the cute_c2 collision library
│   └── …                  one header for each .c file
├── resources/             sprites and images
├── build/                 everything make creates
├── platform/              icons and web page template; leave these alone
└── setup/                 setup scripts (run once)
```

Each frame, `UpdateGame()` in `game.c` does the same three things for the player:

```c
data->activeCommand = PollInput();                                             // 1. keys -> command bits
MediatorHandleCommand(data->playerMediator, data->activeCommand, deltaTime);   // 2. command bits -> events -> maybe a new state
UpdateState((GameObject *)data->player, deltaTime);                            // 3. run the current state's Update
```

The mediator passes the command to its two **colleagues**, just like the last practical:

```c
void MediatorHandleCommand(Mediator *mediator, Command command, float deltaTime)
{
    ExecuteCommand(command, mediator->object, deltaTime);   // 1. the game object acts on it (through its FSM)

    if (mediator->hud)                                      // 2. the HUD shows it, with the state's name
    {
        GameObject *object = mediator->object;
        UpdateHud(mediator->hud, command, object->stateConfigs[object->currentState].name);
    }
}
```

The NPC does exactly the same, except its commands come from `PollAI()` instead of the keyboard, and its mediator has no HUD (`hud` is `NULL`). The FSM can't tell the difference.

**What changed from the command pattern?** Only `ExecuteCommand()`. Before, it called the player's actions directly (`MoveLeft()`, `Jump()`). Now it sends **events** (`EVENT_MOVE`, `EVENT_JUMP`), and the **current state** decides what they mean.

---

## How it all fits together

[![](https://mermaid.ink/img/pako:eNqdlF2PojAUhv9K04nrDSogCrKbSRRxZHf8yKhzsTqZFFqUFYqpZUbX-N-3FtTMTtyLpSHpgfO8520L5wCDFBNow1LpENGI2-BQ5iuSkLINyhixdfl4LJUWdMnQZgWmnQUF4mrPPbrJOOiStygg228-q93_IHs_RQzXnJRylsYxYS-gUrkHnSJ5gChaEiaTx-K9fPpSCHrztvcpo-3lCk6e1MmDuZMmCaIYdCL-Hm2JTB-Mnt3X2Rh8eSOMfwU978ktpB1JdecDgiPE01z9HPSFTkwKwQLoSsC9lHF3JMh4lFJJ5tFnpAo2MdoTBlIa70FVKPRn3bm4JTXbYMRJPzsDrqgB3Gd3OH09OVeK-ffZYKyAarUqLfTmvclA4rlN941QDmrAWSG6JBMuFAu5nsx_mD-ghIz8XyTg-R5KR7Xh2Mmti3PZCz43IybuLjofwINU6ItFx3G0FYsFk_2Wk0SCzooE68ubguhfl-CMHh-9iTcavk6m7afpSXmY72JvQfPsLd_HBLRBGMWxfaf6RtgMlK34TtbEvjPCoB6aSpDGKbPvQnl9wLz_4zoFZiLDV9ULFoa-aeDbmHPBtLBxrRaqTb2l38a6BWYgHZnogvn1Fsb-bcwtMM1vEP1q0tIC0zJuY-LLOtfTT-NaD5_GbbBXYJavIe1qk4Qt1EK3sYcCa-IG_rCXoWY2_2HzfHJqU23615PDWCXa31gOxhFdTySMSYiymIOLR_U0CpHKe4T5ytY3uwWFClyyCEObs4woMCEsQacQHk6SCyg72gLaYsoIznaVU2eryOoLuKBHwW8Q_ZmmyVmCpdlyBe0QxVsRZfKP6UZItMHk8pQRiglz0oxyaDeslhSB9gHuoK23zGrLaOq6pepm3aibDQXuoV3RLLXaMFRLM_Wmahq6flTgb1lXq-qaalhWvW5aakMVrAJRxtPJngZnV6JriaY1yDu2bNzHP4ui0VM?type=png)](https://mermaid.live/edit#pako:eNqdlF1v2jAUhv-K5YpxEyAJgYRsqgQhlGzlQwV6MagqJ3YgI3GQcVoY4r_POAHUVexisZB8kvO85_UH5wCDFBNow1LpENGI2-BQ5iuSkLINyhixdfl4LJUWdMnQZgWmnQUF4mnPPbrJOOiStygg228-q93_IHs_RQzXnJRylsYxYS-gUrkHnSJ5gChaEiaTx-K7fPtSCHrztvcpo-3lCk6e1MmDuZMmCaIYdCL-Hm2JTB-Mnt3X2Rh8eSOMfwU978ktpB1JdecDgiPE01z9HPSFTkwKwQLoSsC9lHF3JMh4lFJJ5tFnpAo2MdoTBlIa70FVKPRn3bn4SWq2wYiTfnYGXFEDuM_ucPp6cq4U8--zwVgB1WpVWujNe5OBxHOb7huhHNSAs0J0SSZcKBZyPZn_MH9ACRn5v0jA8z2UjmrDsZNbF-eyF3xuRkzcXXQ-gAep0BeLjuNoKxYLJvstJ4kEnRUJ1pcvBdG_LsEZPT56E280fJ1M20_Tk_Iw38XegubZW76PCWiDMIpj-071jbAZKFtxT9bEvjPCoB6aSpDGKbPvQvl8wLz_4zoFZiLDV9ULFoa-aeDbmHPBtLBxrRaqTb2l38a6BWYgHZnogvn1Fsb-bcwtMM1vEP1q0tIC0zJuY-Jmnevpp3Gth0_jNtgrMMvXkHa1ScIWaqHb2EOBNXEDf9jLUDOb_7B5Pjm1qTb968lhrBLtbywH44iuJxLGJERZzMHFo3oahUjlPcJ8Zeub3YJCBS5ZhKHNWUYUmBCWoFMIDyfJBZQdbQFtMWUEZ7vKqbNVZPUFXNCj4DeI_kzT5CzB0my5gnaI4q2IMvmP6UZItMHk8pYRiglz0oxyaDe0lhSB9gHuoF1XzWrLaOpWS9yahgL30K5YWrVhqJZm6k3VNHStdVTgb1lTq-qaalhWvW5aakMVnAJRxtPJngZnR6JjiYY1yLu1bNrHP17V0IE)

Four systems work together:

1. **Finite State Machine (FSM):** keeps track of what each game object is doing (Idle, Moving, Firing, Jumping, Dead, Respawn). Events cause state changes, and each state has its own Entry, Update and Exit functions.
2. **Command system:** each input is a bit flag, so several commands can happen at once (e.g. `MOVE_LEFT | JUMP`). Input knows nothing about game logic.
3. **Collision detection:** circle-to-circle collisions with cute_c2, using a manifold to push objects apart. Collisions send events too (`EVENT_COLLISION_START`, `EVENT_COLLISION_END`).
4. **Mediator:** passes each command to the game object and the HUD, so they never need to know about each other.

---

## The state machine

### States

- `STATE_IDLE`: at rest
- `STATE_MOVING`: in motion
- `STATE_FIRING`: attacking
- `STATE_JUMPING`: jumping
- `STATE_DEAD`: defeated
- `STATE_RESPAWN`: coming back

[![](https://mermaid.ink/img/pako:eNqVk-9rgkAYx_8VeV4OjTJNvReDmG44lkW1BptjHN2tZOnFdY5a9L_vtCyjuDFf-f3hx-cRbwtTRiggWAksqJ_gGcep8W3GmSavt5t3zTButdG4Ow4-Qv8p2PsnXYt7_UkYPWhICyZBNC7kWfsQn-OO7agfqdj34bDOllLN_qN_iC9mEUlKuWKMx-feoM4tdL1e5f8G-0HXP1L9UL2cqnyxmap8Oa6qXWan6jAYDbov0bXlquj6ZwAdZjwhgATPqQ7SSXEhYVswYhBzmtIYkLzllORrg2D-ZUzZgvEY4mwnn1_i7JWxtEJwls_mgD7xYiVVviSnX_nocpoRyu9YnglAtlsyAG1hDcj0vEazY7mW59ptu2V3dNgAMsyGY7mObbZajvQ817R2OvyUr202Oq5tWZbdbkvbM5uODjgXbLTJptVQlCSC8d7-dJWHbPcL5_73Gw?type=png)](https://mermaid.live/edit#pako:eNqVk1FvgjAUhf8KuY8LGCkCpQ9LzGALy0SjziUby9JIp2RCTS2LzvjfByiK0bCsT5x7Tj_ubdotTHnEgMBKUsncmM4ETbRvFKZKvt5u3hVNu1VG4-7Y-_DdJ29fP-ma3etP_OBBIYo38YJxIc_SB_scd0wH_aCJfe8P6-xcNrP_yB_si15knDDR0Mbjc29Q5xa6Hq_8f4Ndr-seqa7fPFxT-GKypvBlu03p0jtFh95o0H0Jrg1XWdePAVSYiTgCIkXGVMgrCS0kbAtGCHLOEhYCyT8Fi7K1FlHxpU35gosQwnSX71_S9JXzpEIIns3mQD7pYpWrbBmdrvKxKlgaMXHHs1QCMY2SAWQLayAI45aFkYOQYRkI68hUYQNE03HLQLqBbNR2OjayTGunwk_533bLNpwOQraDddOyDZwDaSb5aJNOq65YFEsuevvnVb6y3S_Q0vc-)

The object is always in exactly **one** state, stored in `currentState`. The one before it is kept in `previousState`.

### State configuration

Every state is described by a `StateConfig`: a name, four function pointers and its transition table.

```c
typedef struct StateConfig
{
    const char *name;          // e.g. "Player_Idle"
    EventFunction HandleEvent; // what this state does when an event arrives
    StateFunction Entry;       // runs once, when the state starts
    StateFunction Update;      // runs every frame while in the state
    StateFunction Exit;        // runs once, when the state ends
    State *nextStates;         // the states it may change to
    int nextStatesCount;
} StateConfig;
```

- **HandleEvent:** decides what to do with each event, usually by asking for a new state with `ChangeState()`.
- **Entry:** called **once**, when the state starts. Reset timers, set the colour.
- **Update:** called **every frame** while in the state. Movement and timers go here.
- **Exit:** called **once**, when the state ends. Tidy up.

### Example state setup

The player's Idle state is set up like this in `InitPlayerFSM()`:

```c
// Define valid transitions from IDLE
State idleValidTransitions[] = {STATE_MOVING, STATE_FIRING, STATE_JUMPING, STATE_DEAD};

// Configure IDLE state
object->stateConfigs[STATE_IDLE].name = "Player_Idle";
object->stateConfigs[STATE_IDLE].HandleEvent = PlayerIdleHandleEvent;
object->stateConfigs[STATE_IDLE].Entry = PlayerEnterIdle;
object->stateConfigs[STATE_IDLE].Update = PlayerUpdateIdle;
object->stateConfigs[STATE_IDLE].Exit = PlayerExitIdle;

// Set valid transitions
StateTransitions(&object->stateConfigs[STATE_IDLE], idleValidTransitions,
                 sizeof(idleValidTransitions) / sizeof(State));
```

When a state's `HandleEvent` asks for a new state, `ChangeState()` checks the table first. If the move is allowed, it runs the old state's `Exit()`, then the new state's `Entry()`. If not, it prints `Invalid state transition` and nothing changes.

### The player's state diagram

[![](https://mermaid.ink/img/pako:eNqFlFFvmzAUhf-KdZ_WClBwIQE_TIoKm1I1NGpYKm1MlRW8xGrBkTFrsij_fSYpCy2TyxPn-jv3niuE97AUOQMClaKKRZyuJC3s3zgrkX5-XP5Etv0ZzdNxGj9Ootv4VD_rzvH0bjFJvhIUL-IkbZQJ_jK578BameCbb9NZh26kgY7icdSi0UT37bKnkO-WaunkLomNsDl1D_8gd483Jj_N7iVPecEkircbLlmFPg0c168ujL72O_3PaaEXrtaIl5tambsYs74u_nFY_C5s32ic0xx22Pt4Phs_JASNayVQKmlZccXFG8sr04vWs4AFK8lzIErWzAIdvKCNhH3TLgO1ZgXLgOhXyfJ6a-dUPtlL8SxkBll50P4NLb8LUbQtpKhXayC_6HOlVb3Jz3_cv6pkZc7ktahLBcQdHHsA2cMWCA5DZzD0Ai8M_Cvf9YcW7IDY2Bl5wcjHrjvStTDA3sGCP8exA2cY-J7n-RgHYYh9C6jecr4rl20klnMl5PR0BRxvgsNfnQ8rtQ?type=png)](https://mermaid.live/edit#pako:eNqNlFFPwjAUhf9Kc5_UMAJzg9EHE-KmwcggMDHRGdOwCo2uJaVTkPDf7RiTyZLh-rLTfufec7N0G5iKiAKGpSKKuozMJImNTzPkSD_PFy_IMK7QOOgG3mvPvfey_YMuHPcHk55_i5E38fwgVVXwTW9UgLWqgu8e-sMCncoK2vW6bo66vX3dIp8FPRosd_gD36uEq5OX8BPZS_zJ9Fn_UvqAxVQib7Vgki4r-fw7_XGgsy-m5ojxRaLOK-0nA-4n_n_CsuFkjxQo8CNvPOw--hh1EyVQIAlfMsUEL1r2TCnWkQVqMJMsAqxkQmugI8cklbBJi4Wg5jSmIWD9KmmUrIyIyHdjKj6EDCHkW-1fEP4kRJyXkCKZzQG_kY-lVskiOly0X4TyiMprkXAFuLWrAHgDK8Bmp1NvtCzH6jj2pd209eEasGHW25bTts1ms633Oo5pbWvwvWvaqLcc27Is27b0ajiOXQOiZxyv-TTvRyOmhOxnN3_3A9j-AObqKAg)

This is how the player is **meant** to work. Compare it with the transition tables in `player.c`: one arrow is missing from the code. You'll fix it in Task 2.

> **Why not just use `bool`s?** With `isJumping`, `isFiring` and `isDead`, nothing stops all three being `true` at once, and every new flag doubles the number of combinations to check. An FSM is in exactly one state, and the transition tables say, in one place, what can follow what.

---

## Events

Events are what happened. They're defined in `events.h`, and they cause state changes:

```c
typedef enum
{
    EVENT_NONE,             // No action
    EVENT_MOVE,             // Movement input
    EVENT_FIRE,             // Attack action
    EVENT_JUMP,             // Jump action
    EVENT_DIE,              // Death condition
    EVENT_RESPAWN,          // Respawn trigger
    EVENT_COLLISION_START,  // Collision begins
    EVENT_COLLISION_END,    // Collision ends
    EVENT_COUNT             // Total event count
} Event;
```

The same event can mean different things in different states. `EVENT_FIRE` starts firing in Idle, but Jumping ignores it.

---

## Commands as bit flags

The command system is the one from the last practical. Every command gets its own bit, so one number can hold any mix of them:

```c
typedef enum
{
    NONE          = 0,        // No command
    MOVE_UP       = 1 << 0,   // Binary: 000001
    MOVE_DOWN     = 1 << 1,   // Binary: 000010
    MOVE_LEFT     = 1 << 2,   // Binary: 000100
    MOVE_RIGHT    = 1 << 3,   // Binary: 001000
    JUMP          = 1 << 4,   // Binary: 010000
    FIRE          = 1 << 5,   // Binary: 100000
    COMMAND_COUNT = 6         // How many commands there are: NOT a flag
} Command;
```

> **`COMMAND_COUNT` is a number, not a flag.** Never OR it into a command. When you add a command, increase it by one.

### Combining commands

```c
// Move left AND jump simultaneously
Command combo = MOVE_LEFT | JUMP;
// Binary: 000100 | 010000 = 010100

if (IsCommandActive(combo, MOVE_LEFT))   // TRUE: bit 2 is set
    // ...

if (IsCommandActive(combo, JUMP))        // TRUE: bit 4 is set
    // ...
```

### From command to event

[![](https://mermaid.ink/img/pako:eNp1lO1u2jAUhm_FcsX4MWDOBxjcqVKBTGu1dKi0nTSoKid2ICJxIsfpYIi_u4Bd4q5kjhtg7UQsRfbJed7zxsnxFoYZ45DARmMbi1gRsG2qJU95k4Amo3LV3O0ajblYSJovwZfbuQD6upxNErrhElyJvFQfA_nh4tv7aU5DDnLJi4KzR9BuX4DhbJIliUkCZy461zfr3KTfclVKUYBRlqZUsMcX3aGhRrM6Cj4ldFGYfP_rg_d0PwHvnrlU5-D63p-YOLIQQlaNjwzuz3zOYqoyaTImVPt5W8g3meOZt-Zhqfi-3muPw1gVQGXAe-ZCFTU51iTwHrybu6fKk0nUGybqWGXMaHuzT1MffNaqCTcCb8SvdBz8-fUb-NlzLBZmel2muZ7XlTzQ7oA7WqyADTpa8Wb2f6qRkjwq9ZaDUqg4AVG85tVrvogUaqPrXOpokpAzFLhRL2wVSmYrTs7cKHQi3AqzJJPkLDLXv9iwxjB1A4QOWBQF2GWnsdEBs6LusVqEevbAPo35NeZSm2J6wAJnwFhwGhvXmBV0uX002bdC3HdPY16N9QOLWsdqPBrQAT2N3exN2tU4mmTVeI3VD9uMFksqJd0Q0AXd_YdJYrGaGknGI1omChwsoGrs6R8xU0ti52vYggsZM0iULHkLplymtFrCbSU4h6Zr55DoqeSsXLer7m0bR3M4FzvN51R8z7J0LyGzcrGEJKJJoVdlzqji45jqVk8PUckF43KU6X8Lkh42GpBs4RoSa4A7joUd28IYO66Duy24gcR2nQ7qIreH-lbXtQd414I_TVXUwc7Ate2-ZSHHwcjptyAtVTbdiHDvSXeublz_5UwyR9PuL5VdeMs?type=png)](https://mermaid.live/edit#pako:eNp1lO1u2jAUhm_FMmL8GDDnA0LcqVKBVG21dKi0nTSoKid2ICJxIsfpYIi_u4Bd4q5kjhtg7UQsRfbJed7zxsnxFoYZZRDDZnMb81hisG3JJUtZC4MWJWLV2u2azTlfCJIvwZe7OQfquphNErJhAlzzvJSfA_Hp_NvHaU5CBnLBioLRJ9DpnIPhbJIliU4CDRudqZtxptPvmCwFL8AoS1PC6dOr7lBTo1kdBZcJWRQ63__66D0_TMCHFybkGbh58Cc6jgyEkFHjI437M5_RmMhM6IwJUX7eF_J15njmrVlYSrav99bjMJYFkBnwXhiXRU2OFQm8R-_2_rnypBPVhvE6VhnT2t7scuqDK6WaMC3wTvxaxcGfX7-Bn73EfKGnN2Waq3ldyQOdLrgnxQqYoKsUb2f_p2opwaJSbTkouYwTEMVrVr3mq0ghN6rOhYomCW6gwI76YbuQIlsx3LCj0IqcdpglmcCNSF__YsMac4gdIHTAoihwbHoaGx0wI-odq0Wob7rmacyvMZuYxCEHLLBcSoPT2LjGjKDHzKPJgRE6A_s05tXYIDCIcazGIpe45DR2uzdpVuNoklbjLVY_7FBSLIkQZINBD_T2HyaJ-WqqJSmLSJlIcLCAqrGnf8RULrGZr-cctuFCxBRiKUrWhikTKamWcFtJzqHu2znEaioYLdedqn872tMczvlO8Tnh37Ms3UuIrFwsIY5IUqhVmVMi2TgmqtnTQ1QwTpkYZervgrhvaQ2It3ANseE6XctwLNNwHMeyLafXhhuITdvqoh6y-2hg9GzTdXZt-FNXRV3Hcm3THBgGsiwHWYM2JKXMphse7j2p3lWt67-eSvpw2v0Fsqh5kQ)

`ExecuteCommand()` in `command.c` turns the bits into events:

- any movement bit sets `inputAxis` and sends `EVENT_MOVE`; no movement sends `EVENT_NONE`,
- `JUMP` sends `EVENT_JUMP`,
- `FIRE` sends `EVENT_FIRE`.

Several events can arrive in the same frame, one after the other.

---

## Collision detection

The collision system uses cute_c2 circles. `UpdateGame()` tracks when a collision starts, continues and ends:

```c
// Update colliders to match the objects
data->player->base.collider.p.x = data->player->base.x;
data->player->base.collider.p.y = data->player->base.y;
data->player->base.collider.r = data->player->base.r;

// Check collision
bool isColliding = CheckCollision(&data->player->base, &data->npc->base);

if (isColliding && !data->player->base.isColliding)
{
    // ENTER: first frame of the collision
    HandleEvent(&data->player->base, EVENT_COLLISION_START, deltaTime);
    CollisionEntry(&data->player->base, &data->npc->base);   // damage
    HandleCollision(&data->player->base, &data->npc->base);  // pushback
}
else if (!isColliding && data->player->base.isColliding)
{
    // EXIT: first frame after the collision ends
    HandleEvent(&data->player->base, EVENT_COLLISION_END, deltaTime);
    CollisionExit(&data->player->base, &data->npc->base);
}
else if (isColliding && data->player->base.isColliding)
{
    // ONGOING: keep pushing apart
    HandleCollision(&data->player->base, &data->npc->base);
}
```

The NPC gets the same events. Damage happens once, on ENTER, so the health bars don't drain 60 times a second.

---

## Game objects

The player and NPC are both a `GameObject`, with extras:

[![](https://mermaid.ink/img/pako:eNqlV31v4jYY_ypWKtTSBQThJSWrKlVAb0wtVIV20sZUmcQB3yU2cpI7WMd33-MYivPCetvxB8K_5_3V5s1wuUcMx6hU3iijsYPezuMVCcm5g849LL6c73aVypy5AY6iAcVLgcM5Q_CpVNCQJWGkTikdDb8SFqM3BcnP9TUBnpubIzJ8GY5nr-PJeJjHHiYvBexu9FTAfn1-eMxjg1GB7Wk4fbz9bZyH-5P7-9F0NBm_Tme3T7PT5OF4UCQ-j_cSuznTA5_GOCb_HjiYmw1fR4P7YR6DwEfjT3kUQi9BZfAl8GB4O8hjhQQouCwIqOUnHJLJ4jNxYxONH_smwsxDjwHeEoGiWCRunKn0kT0btWLV4_7J5SyKkbvC4hIxENNIfsBxjDYm2moghRYS2vkFjHBhIcrWSXy7oZGJPCoApJzpZqw-FW5AkMuDgHpEV7HgPEA06qcUypYaCTAupAwXBcdiGmbUqCqvBflKeRKlpwLVTYSAISgl9jnz6fIS8vl-iHKBrwgO4pUGjmAoj8m-4IvPZprFqh5DgMP1kWnCpq4ghElmnW1AAhKTrLLq6XZWDv5IedN1cJewtFToF-iogKRYPjHvPEMWi-1J6vPaK8nqUXhDC5rBJ7JR1cin-kjo8-TgUy4Rd9OHbAL8KKy7mfi1sFR1iPwJPUqCGM9otlIqgtSqYi7l6mMGiSBC42PkW3qqItnLOu8KsyUp4dR154VShpnALKIyc9GFm9baRPERM2EoIC3lDbJfDB_1hrYlFjgqTj4MQkgZLuAhzoByBpTFC9lhZroxTCSqez_O2tbPpexQPpkUsK-nV9FGUDW9dlVUr9dLg4V1-AORylbDy6UgUZRdWNJNUJ0PCaCSeAAtDwYI3xMJbPgnEuC0siu63s-C5vX137XaoaxOGgO68KmA0Q5JuCCiWi4hc_N97HOjOTfQJcjMDajvFg76mnEyW_EP7a76U6nSmWu1m_2V62Tm-z3WR8FdIuc34N8UKCe5Xr_RPXLk1oLhiUqW9oE966KLgyBC_mHhrDmVYxplRdQjyEFr6B3QHfPi5tvnuWjAp9IAZSjTw0pIpvoDCdUlWh7k7ZaICF2EOHZXCF52iMOXgBs0fcpF-zpF8RZuTuW51Oic2bjpd1wTWp1_Ic6Z3-haPctM70k4pR9dVFXjf4nKvCnBq0UTN_G7IPF7uIdPC2qlVPJdr-M1GkfDvt-0ux_4vE-kUtDGFraPDixaPc9bnFawL6OSbSzafvcYddt3W759WlZW8z8IGqaxFNQzHFg9xDTgXRJieTTSxTQ30jf73HDgpyBesqnJt3tNPWuMOduB_Bqz3zkPDyoET5Yrw_FxEMEpSS-m_QP_nYUweEelt6PhNK86qQ7DeTM2cGxe1W2r17rqtOxOz-52uqaxNZya1erVG3az3en2Gr2m3bN3pvFXarZZb3ct2-40LavVand6FkjgJObTLXMPFolH4a33oP6SpP9Mdv8AiP7R4g?type=png)](https://mermaid.live/edit#pako:eNqlV31v4jYY_ypWKtTSBcQ7JasqVUBvTC1UhXbSxlSZxAHfJTZykjtYx3ffYxuK88J62_EHwr_n_dXmzXK5RyzHKpXeKKOxg97O4xUJybmDzj0svpzvdqXSnLkBjqIBxUuBwzlD8CmV0JAlYaRPio6GXwmL0ZuG5Of6mgDPzc0RGb4Mx7PX8WQ8zGIPk5ccdjd6ymG_Pj88ZrHBKMf2NJw-3v42zsL9yf39aDqajF-ns9un2WnycDzIE5_He4ndnJmBT2Mck38PHMzNhq-jwf0wi0Hgo_GnLAqhF6Ay-AJ4MLwdZLFcAjRcFATU8hMOyWTxmbixjcaPfRth5qHHAG-JQFEsEjdOVfrIno5as5px_-RyFsXIXWFxiRiIGSQ_4DhGGxttDZBCCwnj_AJGuGggytZJfLuhkY08KgCknJlmGn0q3IAglwcB9YipYsF5gGjUVxTKlgYJMC6kDBc5x2IaptToKq8F-Up5EqlTjuomQsAQFBL7nPl0eQn5fD9EmcBXBAfxygBHMJTHZF_wxWdbZbFsxhDgcH1kmrCpKwhhktlkG5CAxCStrHy6nbWDP1JetQ7uEqZKhX6BjgqIwrKJeecZslhsT1Kf115BVo_CG5rTDD6Rja5GNtVHQp8nB58yibibPqQT4Edh1U3Fb4Slq0PkT-hREsR4RtOV0hEoq5q5kKuPGSSCCIOPkW_qVEayl03eFWZLUsBp6s4KKYaZwCyiMnPRhatqbaP4iNkwFJCW4gbZL4aPesPYEgsc5ScfBiGkDOfwEKdAOQPa4oXsMFttDBuJ8t6Ps1bj50J2KJ9MCtg306tpI6iaWbsyqlarhcHCOvyBSGWr4eVSkChKLyzpJqjOhgRQQTyAFgcDhO-JBDb8EwmwquyKrvezYHh9_Xelciiro2JAFz4VMNohCRdElIslZG6-j31u1ecWugSZuQX13cLBXDNOaiv-YdxVf2pVJnOlcrO_cp3UfL_H-ii4S-T8BvybBuUkV6s3pkeO3FowPFHB0j6wp110cRBEyD8snDWnckyjtIh-BDloDb0DumOe33z7POcN-FQaoAylelgLyVR_IKG7xMiDvN0SEaGLEMfuCsHLDnH4EnCDqqdctK9TFG_h5tSeS43OWRfX_bZrQ6vzL8Q582udRq9hq3sSTupjiupq_C9RmTcteLWo4zp-FyR-D_fwaUGjlFq-47W9Wu1o2Pfr3c4HPu8TqRW0cAN3jw4smj3PW5xWsC-jlq0tWn7nGHXLd5t-97SsrOZ_ELRsaymoZzmweohtwbskxPJoqcU0t9SbfW458FMQL9lU5Nu9op811pztQH6N2e-chwcVgifLleX4OIjglKiLaf_Af2chDN5R6na0nHrnSumwnDdrYzm9VrXb6LXq7ates91udXq2tbWcSvOqW611661WvdOuNXu1zs62_lJW69VWp9HttuuNRrPZavcaHdvCScynW-YeDBKPwlPvQf8jUX9Mdv8AuzvRog)

The `GameObject` struct, grouped by what each field is for:

```c
typedef struct GameObject
{
    const char *name;          // Object identifier

    // Transform
    float x, y;                // Position
    int r;                     // Radius
    Vector2 direction;         // Movement direction
    Vector2 inputAxis;         // Input direction

    // Collision
    c2Circle collider;         // Circle collider
    c2Manifold manifold;       // Collision manifold
    bool isColliding;          // Collision state

    // Visual
    Color color;               // Render color

    // FSM
    State previousState;       // Previous state
    State currentState;        // Current state
    StateConfig *stateConfigs; // State configurations

    // Gameplay
    int health;                // Health points
    float timer;               // General-purpose timer
} GameObject;
```

`Player` adds `stamina` and `mana`, and `NPC` adds `aggression`. Neither is used much yet; that's up to you. Because `base` is the **first** member of both, a `Player *` can be cast to a `GameObject *` and back:

```c
Player *player = (Player *)object;   // inside a state function
player->stamina -= 20.0f;
```

Both use the same FSM code in `fsm.c`, but each has its own states, transition tables and functions in `player.c` and `npc.c`. The NPC only uses Idle, Firing and Dead. Its other states are set to `EMPTY_STATE_CONFIG`.

---

## Walkthrough: adding Crouch to the FSM

Follow these steps to add a new state, **`STATE_CROUCH`**, to the player. Hold **C** to crouch; let go to stand up. Every new state follows the same pattern, so it's worth doing this one carefully before the tasks.

### 1. Add `CROUCH` to `Command`

In **`command.h`**, give it the next free bit, and update `COMMAND_COUNT`:

```c
	FIRE 			= 1 << 5,	// Binary: 100000
	CROUCH 			= 1 << 6,	// Binary: 1000000
	COMMAND_COUNT	= 7 // Total number of commands
```

### 2. Add `EVENT_CROUCH` to `Event`

In **`events.h`**, add it **before** `EVENT_COUNT`:

```c
    EVENT_COLLISION_END,   // Collision end event
    EVENT_CROUCH,          // Crouch event
    EVENT_COUNT            // Total number of events (for counting purposes)
```

### 3. Add `STATE_CROUCH` to the FSM

In **`fsm.h`**, add it **before** `STATE_COUNT`:

```c
    STATE_RESPAWN, // Respawn state (respawns the player)
    STATE_CROUCH,  // Crouch state
    STATE_COUNT    // Total number of states (for counting purposes)
```

### 4. Write the Crouch functions

These are the four function pointers in `StateConfig`. Declare them in **`player.h`**:

```c
// Crouch state
void PlayerCrouchHandleEvent(GameObject *object, Event event, float deltaTime);
void PlayerEnterCrouch(GameObject *object, float deltaTime);
void PlayerUpdateCrouch(GameObject *object, float deltaTime);
void PlayerExitCrouch(GameObject *object, float deltaTime);
```

and write them in **`player.c`**:

```c
// Handles events for the Player when in the Crouch state
void PlayerCrouchHandleEvent(GameObject *object, Event event, float deltaTime)
{
	switch (event)
	{
	case EVENT_NONE:
		// C released, no movement: stand up
		ChangeState(object, STATE_IDLE, deltaTime);
		break;
	// Ignore Events for other cases
	case EVENT_MOVE:
	case EVENT_FIRE:
	case EVENT_JUMP:
	case EVENT_DIE:
	case EVENT_RESPAWN:
	case EVENT_COLLISION_START:
	case EVENT_COLLISION_END:
	case EVENT_CROUCH:
	case EVENT_COUNT:
		break;
	}
}

void PlayerEnterCrouch(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	printf("\n%s -> ENTER -> Crouch\n", object->name);
	object->color = DARKBLUE;
	object->r = DEFAULT_GAMEOBJECT_RADIUS / 2; // smaller while crouching
}

void PlayerUpdateCrouch(GameObject *object, float deltaTime)
{
	(void)object;
	(void)deltaTime;
	// Nothing to do while crouching (yet)
}

void PlayerExitCrouch(GameObject *object, float deltaTime)
{
	(void)deltaTime;
	printf("\n%s <- EXIT <- Crouch\n", object->name);
	object->r = DEFAULT_GAMEOBJECT_RADIUS;
}
```

### 5. Add the state configuration

At the end of `InitPlayerFSM()` in **`player.c`**, register the functions and the transition table:

```c
	// ---- STATE_CROUCH state configuration ----
	State crouchValidTransitions[] = {STATE_IDLE};

	object->stateConfigs[STATE_CROUCH].name = "Player_Crouch";
	object->stateConfigs[STATE_CROUCH].HandleEvent = PlayerCrouchHandleEvent;
	object->stateConfigs[STATE_CROUCH].Entry = PlayerEnterCrouch;
	object->stateConfigs[STATE_CROUCH].Update = PlayerUpdateCrouch;
	object->stateConfigs[STATE_CROUCH].Exit = PlayerExitCrouch;

	StateTransitions(&object->stateConfigs[STATE_CROUCH], crouchValidTransitions, sizeof(crouchValidTransitions) / sizeof(State));
```

### 6. Define how to enter and exit Crouch

[![](https://mermaid.ink/img/pako:eNqFU21r2zAQ_itCIQSGHWzH8Ys6Cl1iaFmbjjbdh81jKPY5MZGlIsshbsh_n2znbR1l98Ho7vQ8z53vtMOJSAET3O_vcp4rgnYDtYICBgQNUirXg_2-3495qaiCaU6Xkhbmxok50vbz0y9kmtfoeX4zj37fTe-jmHeZc-TiwuTp8WVyq3knA7SGGq2ApZ8X8vobozXIu5TBLeX6G22Aq55rXQ2HPde-aq5E36PZ_EBwqdBF3hXRKUhgQEtIDcQFKsRG98TVWW4iRZWs_iM4e5ydWuJCAZL5cqWQyP6S7_KNRVzJmqBOQTtHmTP3-W5ZUMZAGmh68_T1y_1LdEGzbQZxYNHnj0kWNFkjJVBWMYbK_A26FPC0rfdYe8JoWU4hQ7nuFmU5Y6RnL8bgWEappFgD6QV24geukQgmJOllrb0DJ20ZB7i1cDMvOcHdLBll_ofwy4Voavg3dZhkpxFzbOClzFNMlKzAwAXIgjYu3jXQGLc7GmOijxLSams2u2q28jGO-V7jXyn_IURxpNDEyxUmGWWl9qrX9LzQp6jUP07PTFRcYeJYfkuCyQ5vMRn57nAUOK7vBZbnuZZn4BoT0w6HI9cbB57j-K7thaGzN_Bbq2sNQ3ccBuPAtsORHwSOhtBKieeaJ8eqIM2VkA_dG2yf4v4PAaIY-A?type=png)](https://mermaid.live/edit#pako:eNqFU2Fv2jAQ_SuWEUKaAkqCCcGdKnUQqdVaOrV0H7ZMk0kuEOHYleMgKMp_n5MAYZ2q3YfId-f33l3ufMCRjAFT3O0eUpFqig49vYYMehT1YqY2vbLsdkORa6ZhlrKVYll_64YCGfv56Rfq96_R8-JmEfy-m90HoWgybeTiwvTp8WV6a3inPbSBPVoDjz8v1fU3zvag7mIOt0yYb7AFoTvEvhoMOsS5qq4E34P54khwqdBE3hXRKCjgwHKILSQkyuTW9CR0KzdVsojW_xGcP87PLQmpAal0tdZIJn_JN_nKAqHVnqJGwTgnmZa7vZtnjHNQFprdPH39cv8SXNDsqkEcWcz5Y5IlizZIS5QUnKM8fYMmBSKu6z3VHnGW5zNIUGq6RUnKOe04yxG4tpVrJTdAO74TjX1iRZJLRTtJbe_AUV3GEW4vSeJFZzhJomEy_hB-uRBVDf-mjpNsNEKBLbxSaYypVgVYOAOVscrFhwoa4npHQ0zNUUFc7PrVrvZr-RCHojT4VyZ-SJmdKAzxao1pwnhuvOI1bhf6HFXmx5mZyUJoTJ2xW5NgesA7TIcTMhj6Lhl7vu15xPYsvMfUswfeaEhsm7iOQ9yRX1r4rVa1BxMymvgj33Emw7HvuwbACi2f9yI61QRxqqV6aF5g_RDLP9gqGH0)

**Entering Crouch** happens in Idle. Add `STATE_CROUCH` to Idle's transition table:

```c
State idleValidTransitions[] = {STATE_MOVING, STATE_FIRING, STATE_JUMPING, STATE_DEAD, STATE_CROUCH};
```

and handle the event in `PlayerIdleHandleEvent()`:

```c
	case EVENT_CROUCH:
		ChangeState(object, STATE_CROUCH, deltaTime);
		break;
```

**Exiting Crouch** happens on `EVENT_NONE`, in `PlayerCrouchHandleEvent()` from step 4.

### 7. Update the input manager and `ExecuteCommand()`

In **`input_manager.c`**, set the bit while **C** is held:

```c
    if (IsKeyDown(KEY_C))
        command |= CROUCH;
```

In **`command.c`**, in `ExecuteCommand()`, straight after `object->inputAxis = axis;`:

```c
    // Crouch: while CROUCH is held, send only EVENT_CROUCH
    if (IsCommandActive(command, CROUCH))
    {
        HandleEvent(object, EVENT_CROUCH, deltaTime);
        return;
    }
```

> **Why the `return`?** Without it, `ExecuteCommand()` also sends `EVENT_NONE` while you hold C without moving. Crouch would stand you up, and Idle would crouch you again, **every frame**. Watch out for this with any state you hold a key for.

### 8. Tidy up everything else

- **`npc.c`:** in `InitNPCFSM()`, add `object->stateConfigs[STATE_CROUCH] = EMPTY_STATE_CONFIG;`. **Don't skip this one.** Without it, that slot is full of random memory, and the game crashes when it closes.
- **Every `switch (event)`** in `player.c` and `npc.c` needs a `case EVENT_CROUCH:` with the ignored events. The compiler warns you about each one it finds; that's why the switches list every event.
- **`hud.c` (optional):** show it on screen:

  ```c
  DrawActiveCommand(hud->lastCommand, CROUCH, "CROUCH", &y, PURPLE);
  ```

Build it, hold C and watch the console:

```
Player Hero <- EXIT <- Idle
Player Hero -> ENTER -> Crouch
```

Try crouching while you move, and moving while you crouch. Neither works yet, because only Idle can go to Crouch, and Crouch can only go to Idle. Whether they **should** work is a design decision. If they should, add the transitions.

---

## The tasks

Already working in the starter code:

- [x] Player states: Idle, Moving, Firing, Jumping, Dead and Respawn
- [x] NPC states: Idle, Firing and Dead
- [x] Entry, Update and Exit for every state, with a colour and a "breathing" effect so you can see which state is active
- [x] Firing and Jumping end on a timer
- [x] Collision with the NPC: both are pushed apart and lose health
- [x] Health bars above both objects
- [x] A HUD showing the command bits, the player's current state and a jump counter
- [x] Every state change printed to the console, and the command bits printed when they change

### Task 1: Explore the starter code

Run it, then find:

1. in `player.c`, the transition table (`…ValidTransitions[]`) for each state,
2. in `command.c`, where `ExecuteCommand()` turns command bits into events,
3. in `fsm.c`, where `ChangeState()` calls `Exit()` and then `Entry()`, and where it refuses a transition,
4. in `player.c`, how `PlayerUpdateJumping()` decides the jump is over,
5. in `npc.c`, which states are set to `EMPTY_STATE_CONFIG`,
6. in `game.c`, where collisions start and end, and which events they send.

Then move, jump and fire, and watch the console. Each change prints two lines, for example:

```
Player Hero <- EXIT <- Idle
Player Hero -> ENTER -> Jumping
```

> Want to see every state and its table at once? Call `PrintStateConfigs(data->player->base.stateConfigs, STATE_COUNT);` at the end of `InitGame()`.

### Task 2: Jump while moving

Hold **D** to move right, then press **SPACE**. The player doesn't jump, and the console says:

```
Invalid state transition from Player_Moving to Player_Jumping
```

The player's state diagram has an arrow from Moving to Jumping, but the code doesn't.

1. Find out why. `PlayerMovingHandleEvent()` **asks** to change to Jumping, but the Moving transition table doesn't **allow** it.
2. Fix it by adding `STATE_JUMPING` to `movingValidTransitions[]` in `InitPlayerFSM()`.
3. Jump while moving. The player should jump in the direction it's moving, because `PlayerEnterJumping()` copies `inputAxis` into `direction`.

> **Two locks on every door.** The state's `HandleEvent` decides which state to **ask** for. The transition table decides whether that's **allowed**. Both have to agree. When a transition doesn't happen, check both.

Did you notice the HUD's **Jumps** counter went up even when the jump was refused? The HUD counts JUMP **commands**. Whether a jump really happens is up to the FSM. Change the HUD so it only counts real jumps: the mediator already passes it the state's name.

### Task 3: Die and respawn

Health goes down on every collision, but nothing ever sends `EVENT_DIE`. Health just stops at 0.

1. **Send the event.** In `game.c`, after `CollisionEntry()`, check each object's health. When it reaches 0, send the event:

   ```c
   if (data->player->base.health <= 0)
       HandleEvent(&data->player->base, EVENT_DIE, deltaTime);
   ```

   Do the same for the NPC.

2. **Make Dead last.** At the moment, `PlayerUpdateDie()` changes to Respawn on the very next frame, so you never see it. Use `timer` to stay dead for **2 seconds** first. `PlayerEnterDie()` already resets the timer.

3. **Make Respawn do something.** In `PlayerEnterRespawn()`, put the player back in the middle of the screen and set `health` back to 100. Stay in Respawn for **1 second**, then change to Idle.

4. **Bring the NPC back.** The NPC's Dead state only leaves on `EVENT_RESPAWN`. Send it from `NPCUpdateDead()` after a few seconds, and reset the NPC's health.

5. **Finish the handlers.** `PlayerDieHandleEvent()` and `PlayerRespawnHandleEvent()` ignore every event. Decide what, if anything, a dead player should react to. Can a respawning player be hurt?

> **Where does the code go?** Something that happens **once**, like resetting health, goes in `Entry()` or `Exit()`. Something that happens **over time**, like a countdown, goes in `Update()`. Putting a one-off in `Update()` runs it 60 times a second.

### Task 4: Add a new state (Dashing)

Using the Crouch walkthrough as your guide, add a **Dashing** state: a short, fast burst in the direction the player is moving.

1. Add `STATE_DASHING` to `fsm.h`, **before** `STATE_COUNT`.
2. Add a `DASH` command, an `EVENT_DASH` event, and the key binding (e.g. **E**, with `IsKeyPressed`).
3. Write the state functions in `player.h` and `player.c`:

   ```c
   void PlayerDashingHandleEvent(GameObject *object, Event event, float deltaTime);
   void PlayerEnterDashing(GameObject *object, float deltaTime);
   void PlayerUpdateDashing(GameObject *object, float deltaTime);
   void PlayerExitDashing(GameObject *object, float deltaTime);
   ```

4. Configure the state in `InitPlayerFSM()`:

   ```c
   State dashingValidTransitions[] = {STATE_IDLE, STATE_MOVING, STATE_DEAD};
   object->stateConfigs[STATE_DASHING].name = "Player_Dashing";
   // ... set the function pointers
   StateTransitions(&object->stateConfigs[STATE_DASHING], dashingValidTransitions, 3);
   ```

5. Decide which states can **start** a dash, and add `STATE_DASHING` to their tables.
6. In `PlayerUpdateDashing()`, move fast (say 600 units a second) for 0.15 seconds, then go back to Moving or Idle, like `PlayerUpdateFiring()` does.
7. Don't forget step 8 of the walkthrough: the NPC's `EMPTY_STATE_CONFIG` and every `switch`.

### Task 5: Add a new command (Dodge)

Not every command needs its own state. Add **DODGE**: a quick sidestep that the current state handles itself.

1. Add the command flag in `command.h`, and update `COMMAND_COUNT`:

   ```c
   DODGE = 1 << 8,  // or the next free bit
   ```

2. Add the input binding in `input_manager.c`:

   ```c
   if (IsKeyPressed(KEY_LEFT_CONTROL))
       command |= DODGE;
   ```

3. Send an event in `ExecuteCommand()` in `command.c`:

   ```c
   if (IsCommandActive(command, DODGE))
       HandleEvent(object, EVENT_DODGE, deltaTime);
   ```

4. Add `EVENT_DODGE` to `events.h`, and handle it in the state functions. In Idle and Moving, move the player a short fixed distance sideways. In Jumping, ignore it.

When should something be a **state**, and when is an **event** handled inside a state enough? Write one sentence on it in your comments.

### Task 6: Change the collision response

Experiment with how collisions behave:

- Change the damage values (`DAMAGE_DEFAULT` in `constants.h`).
- Change the pushback distance (`PUSHBACK_DISTANCE`).
- Make collisions change state: the FSMs already receive `EVENT_COLLISION_START` and `EVENT_COLLISION_END`, but every handler ignores them. Make the NPC start firing when the player touches it.
- `CollisionExit()` in `collision.c` sets both colours directly, behind the FSM's back. Move the colour change into a state instead.

### Task 7: Give the NPC a brain

Watch the NPC for a few seconds. It fires almost non-stop. `PollAI()` picks a random command **every frame**, so FIRE comes up within a few frames every time the NPC returns to Idle.

1. Make `PollAI()` decide only **once a second**. Keep the time of the last decision in a `static double` and compare it with `GetTime()`. Between decisions, return `NONE`.
2. Use the NPC's `aggression` (0 to 100) to decide how likely it is to fire. Pass it to `PollAI()` as a parameter, so the AI manager still doesn't need to know what an NPC is.
3. Then try some patterns in `ai_manager.c`. Each will need more states for the NPC:
   - **Patrol:** move back and forth between two points.
   - **Chase:** move towards the player when it's in range.
   - **Flee:** run away when health is low.
   - **Wander:** random movement.

### Task 8: Command combos

Make a sequence of inputs trigger something special. For example, **double-tap A** within 0.3 seconds to dash left:

```c
// Double-tap detection
static double lastTapTime = 0.0;

if (IsKeyPressed(KEY_A))
{
    double now = GetTime();
    if (now - lastTapTime < DOUBLE_TAP_WINDOW)   // e.g. #define DOUBLE_TAP_WINDOW 0.3
    {
        // Trigger dash left
    }
    lastTapTime = now;
}
```

Combos need memory between frames, so keep the last tap time in a `static` variable or in a struct, never in a normal local variable.

### Task 9: Visual effects

Give the player more feedback:

- a trail while moving,
- an impact effect on collision,
- different animations for each state (Idle already "breathes"),
- a better health bar: change colour as health drops.

Each effect belongs to a state. Start it in `Entry()`, run it in `Update()`, and stop it in `Exit()`.

### Task 10: Stretch goals

- **Spend stamina and mana:** jumping costs stamina and firing costs mana. Both refill in Idle using `deltaTime`. When there isn't enough, the state's `HandleEvent` doesn't ask for the change.
- **Show the NPC's state:** the HUD only shows the player's. Draw `stateConfigs[currentState].name` above each object, or give the NPC's mediator a HUD of its own.
- **Go back to the previous state:** `previousState` is stored but never used. Use it so that Firing returns to whichever state the player was in before.
- **Load the transition tables from a file:** read `nextStates` from a text file at start-up, so a designer can change what follows what without recompiling.

---

## Cheat sheet

```c
// The FSM functions (fsm.c)
HandleEvent(obj, EVENT_JUMP, deltaTime);   // send an event to the current state
ChangeState(obj, STATE_IDLE, deltaTime);   // ask to change state; false if not allowed
CanEnterState(obj, STATE_DEAD);            // would that change be allowed?
UpdateState(obj, deltaTime);               // run the current state's Update
PrintStateConfigs(obj->stateConfigs, STATE_COUNT);   // print every state and its table

// Where am I?
obj->currentState                          // e.g. STATE_JUMPING
obj->stateConfigs[obj->currentState].name  // e.g. "Player_Jumping"
obj->previousState                         // the state before this one
```

A state's event handler, with a new event:

```c
void PlayerCustomHandleEvent(GameObject *object, Event event, float deltaTime)
{
    switch (event)
    {
    case EVENT_CUSTOM:
        ChangeState(object, STATE_CUSTOM, deltaTime);
        break;
    // Ignore every other event: list each one
    case EVENT_NONE:
    case EVENT_MOVE:
    // ...and the rest
        break;
    }
}
```

A state that ends on a timer:

```c
void PlayerUpdateCustom(GameObject *object, float deltaTime)
{
    object->timer += deltaTime;   // timer was reset to 0 in Entry()

    if (object->timer >= CUSTOM_DURATION)
        ChangeState(object, STATE_IDLE, deltaTime);
}
```

Bit flags and input, from the last practical:

```c
c |= JUMP;                     // turn a bit ON
c &= ~JUMP;                    // turn a bit OFF
if (c & JUMP) { }              // is the bit on? (what IsCommandActive does)

IsKeyDown(KEY_C);              // true every frame the key is held (crouch, move)
IsKeyPressed(KEY_E);           // true once, on the frame it goes down (dash, dodge)
double now = GetTime();        // seconds since the game started (timers, combos)
```

> The starter code lists every event in each `switch` instead of using `default:`. That way the compiler warns you when you add an event and forget a handler. With `default:`, it can't.

---

## When things go wrong

Don't panic. Nine times out of ten it's one of these:

| You see… | What's going on | Fix |
|---|---|---|
| `raylib.h: No such file or directory` | make can't find raylib | Run the setup script again (Step 2) |
| `Invalid state transition from A to B` | The state asked for B, but B isn't in A's table | Add B to A's `…ValidTransitions[]`, or stop asking for it (Task 2) |
| Nothing happens, and no message either | The state's `HandleEvent` ignores that event | Add a `case` that calls `ChangeState()` |
| The state flips in and out every frame | A held key, plus `EVENT_NONE` sent in the same frame | Send only the held key's event, like Crouch does (walkthrough step 7) |
| The state changes, but the player looks the same | `Entry()` doesn't set a colour, or it's not in the config | Check the config in `Init…FSM()` points at the right functions |
| The state never ends | Its `Update()` never calls `ChangeState()`, or the timer isn't reset | Reset `timer` in `Entry()`, add `deltaTime` in `Update()` |
| Something happens 60 times a second | One-off code in `Update()` | Move it to `Entry()` or `Exit()` |
| The terminal scrolls non-stop | A `printf` in an `Update()` or `HandleEvent` function | Print in `Entry()` and `Exit()` only |
| `free(): invalid pointer`, or a crash when the game closes | A new state has no config in `npc.c`, so its `nextStates` is random memory | Set it to `EMPTY_STATE_CONFIG` (walkthrough step 8) |
| `enumeration value 'EVENT_CROUCH' not handled in switch` | A new event isn't in every switch | Add the `case` to each handler |
| A new command does nothing | `COMMAND_COUNT` not updated, or the bit isn't checked in `ExecuteCommand()` | Check `command.h`, `input_manager.c` **and** `command.c` |
| Dash fires every frame while the key is held | `IsKeyDown` used for a one-off action | Use `IsKeyPressed` |
| The NPC fires all the time | `PollAI()` chooses every frame | Task 7 |
| `Emscripten not found in …` (Linux) | Emscripten isn't in `~/emsdk` | Run the setup script (Step 2) |
| `emcc: No such file or directory` (Windows) | Emscripten isn't loaded in this terminal | Close w64devkit and open it again |
| `Address already in use` | A web server from earlier is still running | `make stop_web`, or use another port: `make run_web WEB_PORT=8080` |
| Browser says it can't connect | The page opened before the server was ready | Refresh the page |
| The program still won't build | Old leftover files | `make clean`, then `make` again |

Still stuck? Read the **first** error message, not the last. The first one is usually the real problem; the rest are just complaining about it.

---

## Further reading

- [State](https://gameprogrammingpatterns.com/state.html) in *Game Programming Patterns* by Robert Nystrom: the classic explanation, starting with exactly the `bool` mess this practical avoids
- [State](https://refactoring.guru/design-patterns/state), [Command](https://refactoring.guru/design-patterns/command) and [Mediator](https://refactoring.guru/design-patterns/mediator) on Refactoring.Guru: diagrams and plain-English explanations
- [raylib cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html) and [examples](https://www.raylib.com/examples.html)
- [cute_c2](https://github.com/RandyGaul/cute_headers/blob/master/cute_c2.h) by Randy Gaul: the collision library, with its documentation at the top of the header

---

## Before you submit

- [ ] It compiles and runs with no errors or warnings
- [ ] Crouch works, following the walkthrough
- [ ] The player can jump while moving, and no `Invalid state transition` appears in normal play
- [ ] The HUD's jump counter only counts jumps that really happened
- [ ] The player and the NPC die at 0 health, stay dead for a while and come back with full health
- [ ] Dashing is a state, and Dodge is handled inside states
- [ ] The NPC decides once a second, using its aggression, and has at least one AI pattern
- [ ] The game still closes cleanly, with no crash
- [ ] The state diagrams match your transition tables (click a diagram to edit it in the Mermaid Live Editor)
- [ ] Your code is commented and tidy

Commit early and commit often. Unlike the player, your deadline has no Respawn state.

Best of luck. Now go make something with a bit of character.