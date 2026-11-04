#!/usr/bin/env bash
# Installs raylib (desktop + web) and Emscripten. Debian/Ubuntu/Pop!_OS or Arch.
set -euo pipefail

RAYLIB_DIR="$HOME/raylib"
EMSDK_DIR="$HOME/emsdk"

if command -v apt-get >/dev/null 2>&1; then
    sudo apt-get update
    sudo apt-get install -y build-essential gdb git python3 \
        libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev \
        libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
elif command -v pacman >/dev/null 2>&1; then
    sudo pacman -S --needed --noconfirm base-devel gdb git python \
        alsa-lib mesa glu libx11 libxrandr libxi libxcursor libxinerama \
        wayland libxkbcommon
else
    echo "Install gcc, make, git, python3 and the X11/GL dev packages, then run this again."
    exit 1
fi

if [ -d "$RAYLIB_DIR/.git" ]; then
    git -C "$RAYLIB_DIR" pull --ff-only
else
    git clone --depth 1 https://github.com/raysan5/raylib.git "$RAYLIB_DIR"
fi

make -C "$RAYLIB_DIR/src" PLATFORM=PLATFORM_DESKTOP -B

if [ -d "$EMSDK_DIR/.git" ]; then
    git -C "$EMSDK_DIR" pull --ff-only
else
    git clone https://github.com/emscripten-core/emsdk.git "$EMSDK_DIR"
fi

"$EMSDK_DIR/emsdk" install latest
"$EMSDK_DIR/emsdk" activate latest

set +u
source "$EMSDK_DIR/emsdk_env.sh" >/dev/null 2>&1
set -u

make -C "$RAYLIB_DIR/src" PLATFORM=PLATFORM_WEB -B
rm -f "$RAYLIB_DIR"/src/*.o

if ! grep -qF "$EMSDK_DIR/emsdk_env.sh" "$HOME/.bashrc" 2>/dev/null; then
    echo "source \"$EMSDK_DIR/emsdk_env.sh\" > /dev/null 2>&1" >> "$HOME/.bashrc"
fi

ls -l "$RAYLIB_DIR/src/libraylib.a" "$RAYLIB_DIR/src/libraylib.web.a"
emcc --version | head -n 1

echo
echo "Setup complete. Open a new terminal, then from your project's src folder:"
echo "  make            # desktop"
echo "  make run_web    # web"