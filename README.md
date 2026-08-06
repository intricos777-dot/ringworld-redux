# TE Halo-Inspired

Original FPS game built on the Twilight Elysium engine, inspired by classic arena shooters.

## Structure

```
src/
  core/       - Game loop, world, entity manager
  player/     - First-person controller, camera
  weapons/    - Weapon system, inventory, projectiles
  vehicles/   - Vehicle controller, physics
  ai/         - Bot behavior, navmesh
  world/      - Map loader, spawn points
  ui/         - HUD, menus
  shaders/    - Original shader pipeline
```

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Status

Scaffolded. No Microsoft code, no ripped assets.
