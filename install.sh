#!/bin/bash

# Color definitions
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color
Git=0

echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}       Unrealium Launcher Installer v1.0         ${NC}"
echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}  Linux Unreal Engine Desktop Linker   ${NC}"
echo -e "${BLUE}=======================================${NC}"

# ── Step 0: Auto-clone from GitHub if not already cloned ─────
if [ "$(basename "$PWD")" != "Unrealium-Launcher" ]; then
    echo -e "\n${YELLOW}[0/4] Cloning Unrealium-Launcher from GitHub...${NC}"
    if command -v git &> /dev/null; then
        git clone https://github.com/AliGalipALATLI/Unrealium-Launcher.git
        cd Unrealium-Launcher
        echo -e "${GREEN}    ✓ Cloned successfully. Restarting installer...${NC}"
        # Restart the script from the new directory
        exec ./install.sh
    else

        echo -e "${RED}[!] git not found. Please clone the repository manually and try again.${NC}"
        exit 1
        Git = 1
    fi
fi

# ── Step 1: Dependencies ──────────────────────────────────────
echo -e "\n${YELLOW}[1/4] Checking and installing required dependencies...${NC}"
if [ "$Git" -eq 1 ]; then
    if command -v yay &> /dev/null; then
        echo -e "    Installing git "
        yay -S --needed --noconfirm git
    elif command -v pacman &> /dev/null; then
        echo -e "    Installing git "
        sudo pacman -S --needed --noconfirm git
    fi
fi

if command -v yay &> /dev/null; then
    echo -e "      Package manager: ${GREEN}yay${NC} (AUR)"
    if ! sudo -n true 2>/dev/null; then
        echo -e "${YELLOW}    sudo requires a password. Skipping package install.${NC}"
        echo -e "    Make sure these are installed: qt6-base qt6-imageformats cmake make gcc"
    else
        yay -S --needed --noconfirm qt6-base qt6-imageformats cmake make gcc
    fi
elif command -v pacman &> /dev/null; then
    echo -e "      Package manager: ${GREEN}pacman${NC}"
    if ! sudo -n true 2>/dev/null; then
        echo -e "${YELLOW}    sudo requires a password. Skipping system package install.${NC}"
        echo -e "    Make sure these are installed: qt6-base qt6-imageformats cmake make gcc"
    else
        sudo pacman -S --needed --noconfirm qt6-base qt6-imageformats cmake make gcc
    fi
else
    echo -e "${RED}[!] Neither yay nor pacman found.${NC}"
    echo -e "    Please install the following packages manually: qt6-base qt6-imageformats cmake make gcc"
    exit 1
fi
echo -e "${GREEN}    ✓ Dependencies are ready.${NC}"

# ── Step 2: Icon ──────────────────────────────────────────────
echo -e "\n${YELLOW}[2/4] Copying UE icon to system...${NC}"
mkdir -p ~/.local/share/icons
if [ -f "assets/UE.png" ]; then
    cp assets/UE.png ~/.local/share/icons/UE.png
    echo -e "${GREEN}    ✓ UE.png copied to ~/.local/share/icons/${NC}"
else
    echo -e "${RED}[!] UE.png not found in current directory. Continuing without icon.${NC}"
fi

# ── Step 3: Build ─────────────────────────────────────────────
echo -e "\n${YELLOW}[3/4] Building Unrealium-Launcher with CMake...${NC}"
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PROJECT_NAME=Unrealium-Launcher
cmake --build build -j$(nproc)
echo -e "${GREEN}    ✓ Build successful.${NC}"

# ── Step 4: Install ───────────────────────────────────────────
echo -e "\n${YELLOW}[4/4] Installing Unrealium-Launcher to your system...${NC}"
mkdir -p ~/.local/bin
mkdir -p ~/.local/share/Unrealium-Launcher

# Stop running instances before replacing the binary
pkill -f "Unrealium-Launcher" 2>/dev/null || true; sleep 1

# Support both possible binary names
if [ -f "build/Unrealium-Launcher" ]; then
    cp build/Unrealium-Launcher ~/.local/bin/Unrealium-Launcher
elif [ -f "build/UnrealLauncher" ]; then
    cp build/UnrealLauncher ~/.local/bin/Unrealium-Launcher
else
    echo -e "${RED}[!] Build binary not found. Build may have failed.${NC}"
    exit 1
fi
chmod +x ~/.local/bin/Unrealium-Launcher

# Copy assets and assistant data to local share
echo -e "      Copying application data..."
cp -r assets ~/.local/share/Unrealium-Launcher/
cp -r assistant ~/.local/share/Unrealium-Launcher/

# Create .desktop entry so Unrealium-Launcher appears in app menus
mkdir -p ~/.local/share/applications
cat <<EOF > ~/.local/share/applications/Unrealium-Launcher.desktop
[Desktop Entry]
Type=Application
Name=Unrealium-Launcher
Comment=Register and launch Unreal Engine editors on Linux
Exec=$HOME/.local/bin/Unrealium-Launcher
Icon=$HOME/.local/share/icons/UE.png
Terminal=false
Categories=Development;Utility;
EOF

# Create autostart .desktop for the project scanner background service
mkdir -p ~/.config/autostart
cat <<EOF > ~/.config/autostart/Unrealium-ProjectScanner.desktop
[Desktop Entry]
Type=Application
Name=Unrealium Project Scanner
Comment=Background service that scans Unreal Projects and creates desktop entries
Exec=$HOME/.local/bin/Unrealium-Launcher --daemon
Hidden=false
NoDisplay=true
X-GNOME-Autostart-enabled=true
EOF

echo -e "${GREEN}    ✓ Unrealium-Launcher installed to ~/.local/bin/Unrealium-Launcher${NC}"
echo -e "${GREEN}    ✓ Application data copied to ~/.local/share/Unrealium-Launcher/${NC}"
echo -e "${GREEN}    ✓ Desktop entry created at ~/.local/share/applications/Unrealium-Launcher.desktop${NC}"
echo -e "${GREEN}    ✓ Autostart entry created at ~/.config/autostart/Unrealium-ProjectScanner.desktop${NC}"

# Start the project scanner daemon immediately
nohup $HOME/.local/bin/Unrealium-Launcher --daemon > /dev/null 2>&1 &
echo -e "${GREEN}    ✓ Project scanner service started in background${NC}"

# ── Done! ─────────────────────────────────────────────────────
echo -e "\n${GREEN}=======================================${NC}"
echo -e "${GREEN}  Installation Complete! 🎉             ${NC}"
echo -e "${GREEN}=======================================${NC}"
echo -e "  You can now find ${BLUE}Unrealium-Launcher${NC} in your application menu."
echo -e "  Run it, click ${YELLOW}Add Editor${NC}, and point it to your"
echo -e "  Unreal Engine root folder to create a desktop shortcut."
echo -e "${GREEN}=======================================${NC}\n"
