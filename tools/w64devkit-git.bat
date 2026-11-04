@echo off
setlocal

:: ------------------------------------------------------------------
:: w64devkit shell (BusyBox sh) with git available inside it
:: Same setup as the "w64devkit" terminal in main.code-workspace.
::
:: Usage:  w64devkit-git.bat            (opens in ..\src from this .bat)
::         w64devkit-git.bat C:\Projects\my_game\src
:: ------------------------------------------------------------------
set "W64DEVKIT=C:\raylib\w64devkit"

if not exist "%W64DEVKIT%\bin\busybox.exe" (
    echo w64devkit not found at %W64DEVKIT%
    pause
    exit /b 1
)

:: w64devkit has no git of its own, so borrow one. Checked in order:
::   1. C:\raylib\git  - portable MinGit unzipped next to w64devkit
::   2. Git for Windows, system-wide install
::   3. Git for Windows, per-user install
set "GIT_ROOT="
if exist "C:\raylib\git\cmd\git.exe" set "GIT_ROOT=C:\raylib\git"
if not defined GIT_ROOT if exist "C:\Program Files\Git\cmd\git.exe" set "GIT_ROOT=C:\Program Files\Git"
if not defined GIT_ROOT if exist "%LOCALAPPDATA%\Programs\Git\cmd\git.exe" set "GIT_ROOT=%LOCALAPPDATA%\Programs\Git"

if not defined GIT_ROOT (
    echo git not found. Unzip MinGit to C:\raylib\git or install Git for Windows.
    pause
    exit /b 1
)

:: Folder to open in: first argument, otherwise ..\src from this .bat
set "START_DIR=%~1"
:: if not defined START_DIR set "START_DIR=%~dp0..\src"
if not defined START_DIR set "START_DIR=%~dp0..\"
if not exist "%START_DIR%\" (
    echo Folder not found: %START_DIR%
    pause
    exit /b 1
)

:: w64devkit tools first, then git's \cmd folder only.
:: (Git's \usr\bin would put its own sh/make/ls ahead of w64devkit's.)
set "W64DEVKIT_HOME=%W64DEVKIT%"
set "PATH=%W64DEVKIT%\bin;%GIT_ROOT%\cmd;%PATH%"

:: Writable HOME for .gitconfig / .profile (lab accounts)
set "HOME=%USERPROFILE%"
if not exist "%HOME%\" set "HOME=C:\raylib\home\%USERNAME%"
if not exist "%HOME%\" mkdir "%HOME%"

:: Bypass the campus proxy (proxycw3.itcarlow.ie) for git in this shell.
:: Same as running every git command as "git -c http.proxy=".
:: An empty http.proxy means "no proxy" and overrides every .gitconfig.
:: The Windows proxy variables are cleared too.
set "HTTP_PROXY="
set "HTTPS_PROXY="
set "ALL_PROXY="
set "GIT_CONFIG_PARAMETERS='http.proxy='"

:: Avoid "dubious ownership" errors on repos cloned by another account
git config --global --get-all safe.directory 2>nul | findstr /x "*" >nul || git config --global --add safe.directory "*"

:: Save "no proxy" in the student's git settings so VS Code's Source Control works too
git config --global http.proxy ""

:: Start the w64devkit login shell in the chosen folder.
:: Runs busybox directly rather than w64devkit.exe (which always jumps
:: to HOME), and passes -d so the login shell doesn't jump there either.
:: %CD% is used because it has no trailing backslash.
cd /d "%START_DIR%"
start "w64devkit" "%W64DEVKIT%\bin\busybox.exe" sh -l -d "%CD%"

endlocal