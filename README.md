# Ringworld Redux — Public Alpha

Ringworld Redux is a free, open-source Halo-inspired tactical FPS built on a custom C++17 engine with SDL2 and OpenGL. It targets desktop Linux, Windows, and macOS, and is designed to run under Proton Experimental for handheld/console-style play.

**Status:** Public Alpha — campaign, multiplayer, Legend mode, boss fights, and save system are functional. Expect bugs and rough edges.

**Contact / bug reports:** `intricos777@gmail.com`

## Features

- 24-mission campaign with dynamic objectives, enemy scaling, and boss fights
- Legend mode with unrestricted AI, hidden spawns, and panic chains
- 3-slot weapon system: real US military + theoretical space weapons
- Multiplayer: local server and P2P up to 16 players
- Save system with checkpoints and mission progression
- Audio script triggers for dialogue and events
- Per-mission achievements
- Final arc destruction bossfight with mutation mechanics past level 7

## Quick Start (Linux / Steam Deck)

### Prerequisites

```bash
# Debian/Ubuntu
sudo apt install build-essential cmake git libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libgl-dev libglew-dev

# Fedora
sudo dnf install gcc-c++ cmake git SDL2-devel SDL2_image-devel SDL2_mixer-devel mesa-libGL-devel glew-devel

# Arch/Garuda
sudo pacman -S base-devel cmake git sdl2 sdl2_image sdl2_mixer mesa glew
```

### Build

```bash
git clone https://github.com/intricos777-dot/ringworld-redux.git
cd ringworld-redux
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Run

```bash
./ringworld-redux-test
```

For Proton Experimental / Steam:

1. Add the built binary as a non-Steam game
2. Set launch options: `PROTON_NO_D3D11=1 %command%`
3. Compatibility tool: Proton Experimental

## Controls

| Key | Action |
|-----|--------|
| WASD | Move |
| Space | Up |
| Ctrl | Down |
| LShift | Sprint |
| Mouse | Look / aim |
| Left click | Fire |
| 1/2/3 | Weapon slots |

## Test / Debug Environment Variables

| Variable | Effect |
|----------|--------|
| `RR_LEGEND=1` | Enable Legend mode |
| `RR_TEST_MOVE=1` | Auto-move player for input testing |
| `RR_TEST_FIRE=1` | Auto-fire for combat testing |
| `RR_TEST_PLAY=1` | Skip menus and auto-start campaign |
| `RR_TEST_HOST=1` | Start multiplayer host |
| `RR_CAMPAIGN_SKIP=N` | Jump to mission index N (0-based) |
| `RR_AUTO_HOST=PORT` | Host multiplayer on specific port |

## Console / Emulator Notes

### Steam Deck (Proton)

The game runs under Proton Experimental. Performance is best with `PROTON_NO_D3D11=1` and `DXVK_ASYNC=1` if available. Touch controls are not mapped yet — use the built-in keyboard/mouse controls or a Bluetooth controller.

### RPCS3 / PS3

Not natively supported yet. A Linux build can run under PS3 homebrew if ported to PS3 SDK.

### Yuzu / Switch

Requires a Linux build running through Yuzu’s Linux executable support. Not officially tested.

### Xbox / Windows

Build on Windows with MSVC or MinGW. SDL2 and OpenGL paths are already cross-platform in the engine layer.

## Engine Architecture

Ringworld Redux uses a modular engine layout:

- `src/core/` — Game loop, initialization, campaign wiring, Legend mode
- `src/campaign/` — Mission loader, objectives, boss spawning, scaling
- `src/player/` — Controller, camera, movement
- `src/world/` — Entity spawning, map loading, panic system
- `src/ai/` — Alien AI, squads, boss behaviors
- `src/weapons/` — Inventory, 3-slot weapon registry
- `src/multiplayer/` — Coop session, network layer
- `src/audio/` — Audio engine and script triggers
- `src/ui/` — HUD and menus

All platform-specific code is isolated behind the engine core, making ports to new platforms straightforward.

## Cross-Platform Compatibility via Twilight Elysium

Ringworld Redux is designed to integrate with the [Twilight Elysium](https://github.com/intricos777-dot/twilight-elysium) engine for true cross-platform portability. Twilight Elysium provides:

- Vulkan-preferred renderer abstraction with OpenGL fallback
- Resolution scaler with automatic integer scaling
- Input system abstraction supporting keyboard, mouse, and gamepad
- Entity-component system for data-driven gameplay objects
- Memory and threading utilities optimized for multi-platform builds

By layering Ringworld Redux on top of Twilight Elysium, the game targets:

- **Linux/Steam Deck** — primary development platform
- **Windows** — MSVC/MinGW builds via CMake
- **macOS** — MoltenVK backend through Twilight Elysium renderer
- **Handheld/Console** — Steam Deck verified; PS3/Xbox ports possible through homebrew toolchains

### Twilight Elysium Integration Status

The engine libraries are built and validated in `/home/sin/Projects/twilight-elysium`:

```
libte-engine.a
libte-renderer.a
libte-asset.a
libte-pipeline.a
libte-shader.a
```

These are not yet linked into Ringworld Redux's main CMake target. Planned integration steps:

1. Add `add_subdirectory(../twilight-elysium ${CMAKE_BINARY_DIR}/twilight-elysium)` to top-level `CMakeLists.txt`
2. Replace direct SDL2/OpenGL initialization in `src/renderer/sdl_gl_backend.*` with `te::Renderer` backend
3. Migrate input polling from SDL to `te::InputSystem`
4. Use `te::ResolutionScaler` for automatic display scaling on handhelds

Once wired, the game will require only Twilight Elysium + a C++17 compiler on any supported platform.

## Contributing

This repo is public. Issues and discussions are enabled. If you find a bug, please:

1. Check existing issues first
2. Open an issue with:
   - OS / hardware
   - Build config
   - Steps to reproduce
   - Log output if available
3. For sensitive disclosures: `intricos777@gmail.com`

Pull requests are welcome. Please keep patches minimal and follow the existing C++17 style.

## Roadmap

- [ ] Full controller rebind support
- [ ] Netcode stabilization for 16-player P2P
- [ ] Vulkan renderer backend
- [ ] Steam Workshop map support
- [ ] Linux/Windows/macOS release binaries
