#**************************************************************************************************
#
#   raylib makefile for Desktop platforms, Web (Wasm), Raspberry Pi (DRM mode) and Android
#
#   Copyright (c) 2013-2026 Ramon Santamaria (@raysan5)
#
#   This software is provided "as-is", without any express or implied warranty. In no event
#   will the authors be held liable for any damages arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose, including commercial
#   applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not claim that you
#     wrote the original software. If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be misrepresented
#     as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.
#
#**************************************************************************************************

#   MODIFIED (MuddyGames / Gameplay Programming I):
#     - Default raylib and emsdk paths per host OS (Windows: C:/raylib, Linux: ~/raylib and ~/emsdk, as set up by setup_linux.sh)
#     - Linux: emcc found automatically, no need to source emsdk_env.sh
#     - Web: asyncify on by default, resources packaged automatically if resources/ has files
#     - Web: emsdk python/node versions detected automatically on Windows
#     - New targets: make run, make web, make run_web, make stop_web
#     - Layout: Makefile in the project root, code in src/ and include/, sprites in resources/,
#       platform files in platform/, everything built goes in build/
#     - Separate object folders for desktop/web and debug/release, so switching never mixes them
#     - Windows: .rc.data built from platform/*.rc with windres
#     - WSL: browser opened on the Windows side
#     - clean: removes build/ (rm, works in w64devkit), no find -executable -delete
#
#**************************************************************************************************

.PHONY: all clean run web run_web stop_web

# Detect the machine we are building on: WINDOWS, LINUX or OSX
#------------------------------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    HOST_OS = WINDOWS
else
    ifeq ($(shell uname),Darwin)
        HOST_OS = OSX
    else
        HOST_OS = LINUX
    endif
endif
# WSL: Linux running under Windows, so the browser is opened on the Windows side
IS_WSL := $(if $(findstring microsoft,$(shell uname -r 2>/dev/null | tr A-Z a-z)),TRUE,FALSE)

# Define required environment variables
#------------------------------------------------------------------------------------------------
# Define target platform: PLATFORM_DESKTOP, PLATFORM_WEB, PLATFORM_DRM, PLATFORM_ANDROID
PLATFORM              ?= PLATFORM_DESKTOP

# Define project variables
PROJECT_NAME          ?= GPPI_project
PROJECT_VERSION       ?= 1.0

# Project layout
SRC_DIR               ?= src
INCLUDE_DIR           ?= include
RESOURCES_DIR         ?= resources
PLATFORM_DIR          ?= platform
BUILD_DIR             ?= build
PROJECT_SOURCE_FILES  ?= $(wildcard $(SRC_DIR)/*.c)

# Web build: optional favicon.ico for the browser tab, if present in resources/
BUILD_WEB_ICON        ?= $(wildcard $(RESOURCES_DIR)/icon/favicon.ico)

# raylib library variables
# Windows: raylib installer location. Linux/macOS: ~/raylib (see setup_linux.sh)
ifeq ($(HOST_OS),WINDOWS)
    RAYLIB_SRC_PATH   ?= C:/raylib/raylib/src
    EMSDK_PATH        ?= C:/raylib/emsdk
else
    # Where setup_linux.sh installs them
    RAYLIB_SRC_PATH   ?= $(HOME)/raylib/src
    EMSDK_PATH        ?= $(HOME)/emsdk
endif
RAYLIB_INCLUDE_PATH   ?= $(RAYLIB_SRC_PATH)
RAYLIB_LIB_PATH       ?= $(RAYLIB_SRC_PATH)
# Library type used for raylib: STATIC (.a) or SHARED (.so/.dll)
RAYLIB_LIBTYPE        ?= STATIC

# Define compiler path on Windows
COMPILER_PATH         ?= C:\raylib\w64devkit\bin

# Build mode for project: DEBUG or RELEASE
BUILD_MODE            ?= RELEASE

# PLATFORM_WEB: Default properties
# ASYNCIFY lets the normal while (!WindowShouldClose()) loop run in the browser
BUILD_WEB_ASYNCIFY    ?= TRUE
BUILD_WEB_SHELL       ?= $(PLATFORM_DIR)/template.html
BUILD_WEB_HEAP_SIZE   ?= 128MB
BUILD_WEB_STACK_SIZE  ?= 1MB
BUILD_WEB_ASYNCIFY_STACK_SIZE ?= 1048576
BUILD_WEB_RESOURCES_PATH ?= $(RESOURCES_DIR)
# Package resources/ only if it contains files (emscripten fails on a missing or empty folder)
BUILD_WEB_RESOURCES   ?= $(if $(wildcard $(BUILD_WEB_RESOURCES_PATH)/*),TRUE,FALSE)

# Build output: build/desktop or build/web, objects in build/obj/<platform>-<mode>
ifeq ($(PLATFORM),PLATFORM_WEB)
    BUILD_NAME         = web
else
    BUILD_NAME         = desktop
endif
BUILD_MODE_NAME        = $(if $(filter DEBUG,$(BUILD_MODE)),debug,release)
PROJECT_BUILD_PATH     ?= $(BUILD_DIR)/$(BUILD_NAME)$(if $(filter DEBUG,$(BUILD_MODE)),-debug)
OBJ_DIR                = $(BUILD_DIR)/obj/$(BUILD_NAME)-$(BUILD_MODE_NAME)

# Local web server for make run_web
WEB_PORT              ?= 8000
WEB_URL                = http://127.0.0.1:$(WEB_PORT)/$(PROJECT_NAME).html

# Determine PLATFORM_OS in case PLATFORM_DESKTOP selected
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    # No uname.exe on MinGW!, but OS=Windows_NT on Windows!
    # ifeq ($(UNAME),Msys) -> Windows
    ifeq ($(OS),Windows_NT)
        PLATFORM_OS = WINDOWS
        export PATH := $(COMPILER_PATH);$(PATH)
    else
        UNAMEOS = $(shell uname)
        ifeq ($(UNAMEOS),Linux)
            PLATFORM_OS = LINUX
        endif
        ifeq ($(UNAMEOS),FreeBSD)
            PLATFORM_OS = BSD
        endif
        ifeq ($(UNAMEOS),OpenBSD)
            PLATFORM_OS = BSD
        endif
        ifeq ($(UNAMEOS),NetBSD)
            PLATFORM_OS = BSD
        endif
        ifeq ($(UNAMEOS),DragonFly)
            PLATFORM_OS = BSD
        endif
        ifeq ($(UNAMEOS),Darwin)
            PLATFORM_OS = OSX
        endif
    endif
endif
ifeq ($(PLATFORM),PLATFORM_DRM)
    UNAMEOS = $(shell uname)
    ifeq ($(UNAMEOS),Linux)
        PLATFORM_OS = LINUX
    endif
endif

ifeq ($(PLATFORM),PLATFORM_WEB)
    # Emscripten required variables
    EMSCRIPTEN_PATH    ?= $(EMSDK_PATH)/upstream/emscripten
    ifeq ($(HOST_OS),WINDOWS)
        # Find the python/node versions that emsdk installed (no hardcoded version numbers)
        CLANG_PATH     ?= $(EMSDK_PATH)/upstream/bin
        PYTHON_PATH    ?= $(firstword $(wildcard $(EMSDK_PATH)/python/*_64bit))
        NODE_PATH      ?= $(firstword $(wildcard $(EMSDK_PATH)/node/*_64bit))/bin
        export EMSDK_PYTHON := $(PYTHON_PATH)/python.exe
        # NOTE: Using := instead of = so Make expands it immediately and avoids
        # creating a recursively-expanded variable that references itself
        export PATH    := $(EMSDK_PATH);$(EMSCRIPTEN_PATH);$(CLANG_PATH);$(NODE_PATH);$(PYTHON_PATH);$(PATH)
    else
        # Linux/macOS: put emcc on the PATH ourselves, no need to source emsdk_env.sh
        ifneq ($(MAKECMDGOALS),clean)
            ifeq ($(wildcard $(EMSCRIPTEN_PATH)/emcc),)
                $(error Emscripten not found in $(EMSDK_PATH). Run setup_linux.sh, or set EMSDK_PATH=/path/to/emsdk)
            endif
        endif
        export PATH    := $(EMSCRIPTEN_PATH):$(PATH)
    endif
endif

# Define default C compiler: CC
#------------------------------------------------------------------------------------------------
CC = gcc

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),OSX)
        # OSX default compiler
        CC = clang
    endif
    ifeq ($(PLATFORM_OS),BSD)
        # FreeBSD, OpenBSD, NetBSD, DragonFly default compiler
        CC = clang
    endif
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    # HTML5 emscripten compiler
    # WARNING: To compile to HTML5, code must be redesigned
    # to use emscripten.h and emscripten_set_main_loop()
    CC = emcc
endif
ifeq ($(PLATFORM),PLATFORM_DRM)
    ifeq ($(USE_RPI_CROSS_COMPILER),TRUE)
        # Define RPI cross-compiler
        #CC = armv6j-hardfloat-linux-gnueabi-gcc
        CC = $(RPI_TOOLCHAIN)/bin/arm-linux-gnueabihf-gcc
    endif
endif


# Define default make program: MAKE
#------------------------------------------------------------------------------------------------
MAKE ?= make

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        MAKE = mingw32-make
    endif
endif

# Define compiler flags: CFLAGS
#------------------------------------------------------------------------------------------------
#  -O1                  defines optimization level
#  -g                   include debug information on compilation
#  -s                   strip unnecessary data from build
#  -Wall                turns on most, but not all, compiler warnings
#  -std=c99             defines C language mode (standard C from 1999 revision)
#  -std=gnu99           defines C language mode (GNU C from 1999 revision)
#  -Wno-missing-braces  ignore invalid warning (GCC bug 53119)
#  -Wno-unused-value    ignore unused return values of some functions (i.e. fread())
#  -D_DEFAULT_SOURCE    use with -std=c99 on Linux and PLATFORM_WEB, required for timespec
CFLAGS = -Wall -D_DEFAULT_SOURCE -Wno-missing-braces -Wno-unused-value -Wno-pointer-sign $(PROJECT_CUSTOM_FLAGS)
#CFLAGS += -Wextra -Wmissing-prototypes -Wstrict-prototypes

ifeq ($(PLATFORM),PLATFORM_WEB)
    CFLAGS += -std=gnu99
else
    CFLAGS += -std=c99
endif

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -D_DEBUG
else
    ifeq ($(PLATFORM),PLATFORM_WEB)
        ifeq ($(BUILD_WEB_ASYNCIFY),TRUE)
            CFLAGS += -O3
        else
            CFLAGS += -Os
        endif
    else
        ifeq ($(PLATFORM_OS),OSX)
            CFLAGS += -O2
        else
            CFLAGS += -s -O2
        endif
    endif
endif
ifeq ($(PLATFORM),PLATFORM_DRM)
    CFLAGS += -std=gnu99 -DEGL_NO_X11
endif

# Define include paths for required headers: INCLUDE_PATHS
#------------------------------------------------------------------------------------------------
INCLUDE_PATHS += -I$(SRC_DIR) -I$(INCLUDE_DIR) -Iexternal -I$(RAYLIB_INCLUDE_PATH)

# Define additional directories containing required header files
ifeq ($(PLATFORM),PLATFORM_DRM)
    # DRM required libraries
    INCLUDE_PATHS += -I/usr/include/libdrm
endif
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),BSD)
        # Consider -L$(RAYLIB_H_INSTALL_PATH)
        INCLUDE_PATHS += -I/usr/local/include
    endif
endif

# Define library paths containing required libs: LDFLAGS
#------------------------------------------------------------------------------------------------
LDFLAGS = -L. -L$(RAYLIB_LIB_PATH)

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        # NOTE: The resource .rc file contains windows executable icon and properties
        # MODIFIED: .rc.data is built from platform/*.rc with windres (it is not in git)
        PROJECT_RC_DATA = $(OBJ_DIR)/$(PROJECT_NAME).rc.data
        LDFLAGS += $(PROJECT_RC_DATA)
        # -Wl,--subsystem,windows hides the console window
        ifeq ($(BUILD_MODE), RELEASE)
            LDFLAGS += -Wl,--subsystem,windows
        endif
    endif
    ifeq ($(PLATFORM_OS),BSD)
        # Consider -L$(RAYLIB_INSTALL_PATH)
        LDFLAGS += -Lsrc -L/usr/local/lib
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        # Reset everything.
        # Precedence: immediately local, installed version, raysan5 provided libs
        #LDFLAGS += -L$(RAYLIB_RELEASE_PATH)
    endif
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    # -Os                        # size optimization
    # -O2                        # optimization level 2, if used, also set --memory-init-file 0
    # -sUSE_GLFW=3               # Use glfw3 library (context/input management)
    # -sALLOW_MEMORY_GROWTH=1    # to allow memory resizing -> WARNING: Audio buffers could FAIL!
    # -sTOTAL_MEMORY=16777216    # to specify heap memory size (default = 16MB) (67108864 = 64MB)
    # -sUSE_PTHREADS=1           # multithreading support
    # -sWASM=0                   # disable Web Assembly, emitted by default
    # -sASYNCIFY                 # lets synchronous C/C++ code interact with asynchronous JS
    # -sFORCE_FILESYSTEM=1       # force filesystem to load/save files data
    # -sASSERTIONS=1             # enable runtime checks for common memory allocation errors (-O1 and above turn it off)
    # -sMINIFY_HTML=0            # minify generated html from shell.html
    # --profiling                # include information for code profiling
    # --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
    # --preload-file resources   # specify a resources folder for data compilation
    # --source-map-base          # allow debugging in browser with source map
    # --shell-file shell.html    # define a custom shell .html and output extension
    LDFLAGS += -sUSE_GLFW=3 -sTOTAL_MEMORY=$(BUILD_WEB_HEAP_SIZE) -sSTACK_SIZE=$(BUILD_WEB_STACK_SIZE) -sFORCE_FILESYSTEM=1 -sMINIFY_HTML=0

    # Build using asyncify
    ifeq ($(BUILD_WEB_ASYNCIFY),TRUE)
        LDFLAGS += -sASYNCIFY -sASYNCIFY_STACK_SIZE=$(BUILD_WEB_ASYNCIFY_STACK_SIZE)
    endif

    # Add resources building if required
    ifeq ($(BUILD_WEB_RESOURCES),TRUE)
        LDFLAGS += --preload-file $(BUILD_WEB_RESOURCES_PATH)
    endif

    # Add debug mode flags if required
    ifeq ($(BUILD_MODE),DEBUG)
        LDFLAGS += -sASSERTIONS=1 --profiling
    endif

    # Define a custom shell .html and output extension
    LDFLAGS += --shell-file $(BUILD_WEB_SHELL)
    EXT = .html
endif

# Define libraries required on linking: LDLIBS
# NOTE: To link libraries (lib<name>.so or lib<name>.a), use -l<name>
#------------------------------------------------------------------------------------------------
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        # Libraries for Windows desktop compilation
        # NOTE: WinMM library required to set high-res timer resolution
        LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32 -lole32
        # Required for physac examples
        LDLIBS += -static -lpthread
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        # Libraries for Debian GNU/Linux desktop compiling
        # NOTE: Required packages: libegl1-mesa-dev
        LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt

        # On Wayland windowing system, additional libraries requires
        ifeq ($(USE_WAYLAND_DISPLAY),TRUE)
            LDLIBS += -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
        else
            # On X11 requires also below libraries
            LDLIBS += -lX11
            # NOTE: It seems additional libraries are not required any more, latest GLFW just dlopen them
            #LDLIBS += -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor
        endif
        # Explicit link to libc
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            LDLIBS += -lc
        endif
    endif
    ifeq ($(PLATFORM_OS),OSX)
        # Libraries for OSX 10.9 desktop compiling
        # NOTE: Required packages: libopenal-dev libegl1-mesa-dev
        LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
    endif
    ifeq ($(PLATFORM_OS),BSD)
        # Libraries for FreeBSD, OpenBSD, NetBSD, DragonFly desktop compiling
        # NOTE: Required packages: mesa-libs
        LDLIBS = -lraylib -lGL -lpthread -lm

        # On XWindow requires also below libraries
        LDLIBS += -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor
    endif
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    # Libraries for web (HTML5) compiling
    LDLIBS = $(RAYLIB_LIB_PATH)/libraylib.web.a
endif
ifeq ($(PLATFORM),PLATFORM_DRM)
    # Libraries for DRM compiling
    # NOTE: Required packages: libasound2-dev (ALSA)
    LDLIBS = -lraylib -lGLESv2 -lEGL -lpthread -lrt -lm -lgbm -ldrm -ldl
endif


# Define all object files from source files
#------------------------------------------------------------------------------------------------
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(PROJECT_SOURCE_FILES))

# Define processes to execute
#------------------------------------------------------------------------------------------------
# For Android platform we call a custom Makefile.Android
ifeq ($(PLATFORM),PLATFORM_ANDROID)
    MAKEFILE_TARGET = -f $(PLATFORM_DIR)/Makefile.Android
    export PROJECT_NAME
    export PROJECT_SOURCE_FILES
else
    MAKEFILE_TARGET = $(PROJECT_NAME)
endif

# Default target entry
all:
	$(MAKE) $(MAKEFILE_TARGET)

# Project target defined by PROJECT_NAME
$(PROJECT_NAME): $(OBJS) $(PROJECT_RC_DATA)
	mkdir -p $(PROJECT_BUILD_PATH)
	$(CC) -o $(PROJECT_BUILD_PATH)/$(PROJECT_NAME)$(EXT) $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)
	$(if $(and $(filter PLATFORM_WEB,$(PLATFORM)),$(BUILD_WEB_ICON)),cp $(BUILD_WEB_ICON) $(PROJECT_BUILD_PATH)/favicon.ico)

# Compile Windows resource file (icon, version info)
$(OBJ_DIR)/$(PROJECT_NAME).rc.data: $(PLATFORM_DIR)/$(PROJECT_NAME).rc
	mkdir -p $(OBJ_DIR)
	windres -I $(PLATFORM_DIR) $< -O coff -o $@

# Compile source files
# NOTE: This pattern will compile every module defined on $(OBJS)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM)

# Web build and local server
#------------------------------------------------------------------------------------------------
# Python used to serve the web build, the command that opens the browser,
# and the command that stops a server left running from an earlier make run_web
ifeq ($(HOST_OS),WINDOWS)
    PYTHON       ?= $(firstword $(wildcard $(EMSDK_PATH)/python/*_64bit/python.exe) python)
    OPEN_BROWSER  = cmd /c start "" "$(WEB_URL)"
    STOP_SERVER   = powershell -NoProfile -Command "Get-CimInstance Win32_Process -Filter \"Name LIKE 'python%'\" | Where-Object { \$$_.CommandLine -like '*http.server $(WEB_PORT)*' } | ForEach-Object { Stop-Process -Id \$$_.ProcessId -Force }"
else
    PYTHON       ?= python3
    # [h] stops pkill matching its own command line
    STOP_SERVER   = pkill -f "[h]ttp.server $(WEB_PORT)" || true
    ifeq ($(HOST_OS),OSX)
        OPEN_BROWSER  = (sleep 1 && open "$(WEB_URL)") &
    else ifeq ($(IS_WSL),TRUE)
        OPEN_BROWSER  = (sleep 1 && cd /mnt/c && cmd.exe /c start "" "$(WEB_URL)" >/dev/null 2>&1) &
    else
        OPEN_BROWSER  = (sleep 1 && xdg-open "$(WEB_URL)" >/dev/null 2>&1) &
    endif
endif

# make run: build and run the desktop game from the project folder (so resources/ is found)
run: all
	./$(PROJECT_BUILD_PATH)/$(PROJECT_NAME)

# make stop_web: stop a web server left running from an earlier make run_web
stop_web:
	$(STOP_SERVER)

# make web: build $(PROJECT_NAME).html into build/web
web:
	$(MAKE) PLATFORM=PLATFORM_WEB $(PROJECT_NAME)

# make run_web: build, serve on 127.0.0.1 and open the game in the browser (Ctrl+C to stop)
run_web: web stop_web
	@echo "Serving $(WEB_URL)  (press Ctrl+C to stop)"
	$(OPEN_BROWSER)
	$(PYTHON) -m http.server $(WEB_PORT) --bind 127.0.0.1 --directory $(BUILD_DIR)/web

# make clean: remove everything that was built (desktop and web)
# MODIFIED: one build/ folder replaces the per-platform file lists; rm works in w64devkit sh
clean:
	rm -rf $(BUILD_DIR)
	@echo Cleaning done