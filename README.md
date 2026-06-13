# Unrealium-Launcher

**Unrealium-Launcher** is a lightweight Linux desktop application that lets you register multiple Unreal Engine installations as `.desktop` entries — without the Epic Games Launcher. Point it to your engine root folder, give it a name, optionally configure launch arguments or environment variables, and Unrealium-Launcher creates a proper application shortcut in `~/.local/share/applications/`. It also includes a built-in **Blueprint Node Assistant** for quick reference to common Visual Scripting nodes.

![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![C++](https://img.shields.io/badge/language-C%2B%2B17-blue)
![Qt](https://img.shields.io/badge/Qt-6-green)

---

## Features

### Editor Management
- **Add Editor** — Browse and select any Unreal Engine root folder; validates `Engine/Binaries/Linux/UnrealEditor` exists.
- **Card Grid View** — All registered editors displayed as visual cards in a scrollable 5-column grid.
- **Inline Renaming** — Click any editor name to edit it directly; desktop entry is updated automatically.
- **Safe Deletion** — Two-level delete: *Delete App* (removes the shortcut only) or *Delete Files* (removes shortcut + recursively deletes the engine directory with confirmation).
- **Favorites System** — Star your most-used editors; favorites are automatically moved to the top of the list and persisted.
- **Custom Launch Arguments** — Pass any command-line arguments to the Unreal Editor (e.g. `-game -project=/path/to/project.uproject`).
- **Environment Variable Support** — Set environment variables like `SDF_NO_STENCIL=1` directly in the launch args field; a wrapper script is automatically generated to apply them.

### Blueprint Node Assistant
- **Node Database** — Built-in collection of common Blueprint nodes (Branch, BeginPlay, Tick, Make Vector, Break Vector).
- **Live Search** — Instant search with three-tier matching (name → keywords → description), results capped at 50.
- **Detail Viewer** — Click any search result to open a detailed description dialog.

### UI & Experience
- **Minimal Dark UI** — Clean black-grey Qt6 interface with custom dark stylesheet.
- **Wayland & X11** — Wayland-native with automatic XWayland fallback.
- **App Menu Integration** — XDG-compliant `.desktop` files work with GNOME, KDE, XFCE, Rofi, and any launcher.
- **UE Icon** — Every shortcut uses the Unreal Engine icon, auto-installed to `~/.local/share/icons/`.
- **No Root Required** — Everything installs to user directories (`~/.local/bin`, `~/.local/share/`, `~/.config/`).

---

## Prerequisites

| Dependency     | Version            |
| -------------- | ------------------ |
| Qt 6 (Widgets) | 6.x                |
| CMake          | 3.20+              |
| GCC / Clang    | C++17 capable      |
| make           | any recent version |

---

## Installation

### Arch Linux / CachyOS / Manjaro (Recommended)

One script handles dependency installation, building, and system integration:

```bash
git clone https://github.com/AliGalipALATLI/Unrealium-Launcher.git
cd Unrealium-Launcher
chmod +x install.sh
./install.sh
```

**What the script does:**

1. Installs `qt6-base`, `qt6-imageformats`, `cmake`, `make`, `gcc` via `yay` (AUR) or `pacman`.
2. Copies `UE.png` to `~/.local/share/icons/`.
3. Builds the project with CMake in Release mode.
4. Installs the binary to `~/.local/bin/Unrealium-Launcher`.
5. Copies app data (`assets/`, `assistant/`) to `~/.local/share/Unrealium-Launcher/`.
6. Creates `~/.local/share/applications/Unrealium-Launcher.desktop`.

After installation, search for **Unrealium-Launcher** in your application menu.

---

### Ubuntu / Debian / Linux Mint

```bash
sudo apt update
sudo apt install -y qt6-base-dev cmake make g++

git clone https://github.com/AliGalipALATLI/Unrealium-Launcher.git
cd Unrealium-Launcher

mkdir -p ~/.local/share/icons
cp assets/UE.png ~/.local/share/icons/UE.png

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

mkdir -p ~/.local/bin
cp build/Unrealium-Launcher ~/.local/bin/Unrealium-Launcher
chmod +x ~/.local/bin/Unrealium-Launcher

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
```

> **Note:** If `qt6-base-dev` is unavailable (older Ubuntu < 22.04), try `qtbase5-dev` for Qt 5.

---

### Fedora / RHEL / CentOS Stream

```bash
sudo dnf install -y qt6-qtbase-devel cmake make gcc-c++

git clone https://github.com/AliGalipALATLI/Unrealium-Launcher.git
cd Unrealium-Launcher

mkdir -p ~/.local/share/icons
cp assets/UE.png ~/.local/share/icons/UE.png

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

mkdir -p ~/.local/bin
cp build/Unrealium-Launcher ~/.local/bin/Unrealium-Launcher
chmod +x ~/.local/bin/Unrealium-Launcher

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
```

---

### openSUSE Tumbleweed / Leap

```bash
sudo zypper install -y cmake make gcc-c++ libqt6-qtbase-devel

git clone https://github.com/AliGalipALATLI/Unrealium-Launcher.git
cd Unrealium-Launcher

mkdir -p ~/.local/share/icons
cp assets/UE.png ~/.local/share/icons/UE.png

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

mkdir -p ~/.local/bin
cp build/Unrealium-Launcher ~/.local/bin/Unrealium-Launcher
chmod +x ~/.local/bin/Unrealium-Launcher

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
```

---

### Making `~/.local/bin` available in PATH

```bash
# Bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc && source ~/.bashrc

# Zsh
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc && source ~/.zshrc
```

---

## Usage

### Adding an Editor
1. Launch **Unrealium-Launcher** from your app menu.
2. Click **Add Editor**.
3. Click **Browse** and select your Unreal Engine root folder (e.g. `/home/user/UE_5.6`).
4. Enter a name (e.g. `UE5.6`).
5. Optionally enter **Launch Args** — command-line arguments or environment variables.
6. Click **Add** — the `.desktop` shortcut is created instantly.

### Managing Editors
- **View Editors** — Click the button on the main page to see all registered editors as a card grid.
- **Rename** — Click any editor name to edit it inline.
- **Favorite** — Click the star on any card to toggle favorite; favorites appear first.
- **Delete** — Click the trash icon, then choose *Delete App* (remove shortcut) or *Delete Files* (remove shortcut + engine files).

### Launch Arguments

The **Launch Args** field accepts both regular arguments and environment variables:

| Input | Result |
|-------|--------|
| `-game` | Passed as argument to UnrealEditor |
| `-project=/path/to/proj.uproject` | Opens a specific project |
| `SDF_NO_STENCIL=1` | Sets environment variable before launch |
| `QT_SCALE_FACTOR=1 GDK_SCALE=1` | Multiple env vars (uses `env` wrapper) |

When environment variables are detected, a launch wrapper script is created at `~/.local/share/Unrealium-Launcher/wrappers/`.

### Blueprint Node Assistant
1. Type in the search box on the left panel.
2. Results update live, prioritized by match type (name → keywords → description).
3. Click any result to open a detailed view.

---

## How It Works

### Desktop Entry Generation
Each editor registration creates a file at `~/.local/share/applications/unreal-{name}.desktop`:

```ini
[Desktop Entry]
Type=Application
Name=UE5.6
Exec=/path/to/UE_5.6/Engine/Binaries/Linux/UnrealEditor
Icon=/home/user/.local/share/icons/UE.png
Terminal=false
Categories=Development;
```

With custom arguments:
```ini
Exec=/path/to/UE_5.6/Engine/Binaries/Linux/UnrealEditor -game -project=test.uproject
```

With environment variables (wrapper script):
```bash
# ~/.local/share/Unrealium-Launcher/wrappers/unreal-ue5.6.sh
#!/bin/bash
cd "/path/to/UE_5.6"
exec env SDF_NO_STENCIL=1 ./Engine/Binaries/Linux/UnrealEditor
```

### Configuration
All editor entries are stored in `~/.config/Unrealium-Launcher/editors.json`. On first run, existing `unreal-*.desktop` files are automatically imported.

---

## Project Structure

```
Unrealium-Launcher/
├── CMakeLists.txt
├── install.sh
├── README.md
├── LICENSE
├── assets/
│   ├── UE.png              ← Unreal Engine icon
│   ├── AR.png              ← Back arrow icon
│   ├── TRA.png             ← Delete/trash icon
│   ├── empty.png           ← Empty star (not favorited)
│   └── full.png            ← Filled star (favorited)
├── assistant/
│   └── nodes/              ← Blueprint node database (JSON)
│       ├── branch.json
│       ├── event_begin_play.json
│       ├── event_tick.json
│       ├── make_vector.json
│       └── brake_vector.json
├── web/                    ← Marketing landing page
│   ├── index.html
│   ├── install.sh
│   └── src/
│       ├── main.js
│       └── style.css
└── src/
    ├── main.cpp
    ├── MainWindow.h/.cpp
    ├── AddEditorDialog.h/.cpp
    ├── ConfigManager.h/.cpp
    ├── DesktopEntryWriter.h/.cpp
    ├── EditorCardWidget.h/.cpp
    ├── EditorEntry.h
    ├── NodeDetailDialog.h/.cpp
    ├── NodeManager.h/.cpp
    └── ViewEditorsPage.h/.cpp
```

---

## Tech Stack

| Layer           | Technology     |
| --------------- | -------------- |
| Language        | C++17          |
| GUI Framework   | Qt 6 (Widgets) |
| Build System    | CMake 3.20+    |
| Config Format   | JSON           |
| Target Platform | Linux x86_64   |

---

## License

[![License: GPL v2](https://img.shields.io/badge/License-GPL_v2-blue.svg)](https://opensource.org/licenses/GPL-2.0)
