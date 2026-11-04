#!/bin/sh
# Run in the w64devkit terminal. Needs: raylib installer (C:\raylib), Git, Python 3 on PATH.
set -e

RAYLIB_SRC="C:/raylib/raylib/src"
EMSDK="C:/raylib/emsdk"

for tool in gcc make git python; do
    if ! command -v $tool >/dev/null 2>&1; then
        echo "$tool not found. Install it, reopen w64devkit and run this again."
        exit 1
    fi
done

if [ ! -f "$RAYLIB_SRC/raylib.h" ]; then
    echo "raylib not found. Run the raylib installer to C:\\raylib first."
    exit 1
fi

if [ ! -f "$RAYLIB_SRC/libraylib.a" ]; then
    (cd "$RAYLIB_SRC" && make PLATFORM=PLATFORM_DESKTOP MAKE=make -B)
fi

if [ -f "$EMSDK/upstream/emscripten/emcc.bat" ]; then
    echo "Emscripten already installed in $EMSDK"
else
    git clone https://github.com/emscripten-core/emsdk.git "$EMSDK"
    cmd.exe /c "cd /d C:\\raylib\\emsdk && emsdk.bat install latest && emsdk.bat activate latest"
fi

PY=$(ls -d "$EMSDK"/python/*_64bit | head -n 1)
NODE=$(ls -d "$EMSDK"/node/*_64bit | head -n 1)/bin

cat > "$HOME/.emsdk_w64devkit.sh" <<EOF
export EMSDK="$EMSDK"
export EMSDK_PYTHON="$PY/python.exe"
export PATH="$EMSDK;$EMSDK/upstream/emscripten;$EMSDK/upstream/bin;$NODE;$PY;\$PATH"
EOF

if ! grep -qF ".emsdk_w64devkit.sh" "$HOME/.profile" 2>/dev/null; then
    echo '. "$HOME/.emsdk_w64devkit.sh"' >> "$HOME/.profile"
fi

. "$HOME/.emsdk_w64devkit.sh"

(cd "$RAYLIB_SRC" && make PLATFORM=PLATFORM_WEB -B EMSDK_PATH="$EMSDK" PYTHON_PATH="$PY" NODE_PATH="$NODE")
rm -f "$RAYLIB_SRC"/*.o

ls -l "$RAYLIB_SRC/libraylib.a" "$RAYLIB_SRC/libraylib.web.a"
emcc --version | head -n 1

echo
echo "Setup complete. Close and reopen w64devkit, then from your project's src folder:"
echo "  make            # desktop"
echo "  make run_web    # web"