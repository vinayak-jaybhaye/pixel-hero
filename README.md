# 🎮 Pixel Hero

A **2D platformer game** built with **C++ and OpenGL (GLUT)**, featuring sprite-based rendering with a custom texture manager.

> **Branch:** `master` — Sprite-based rendering using PNG textures loaded with [stb_image](https://github.com/nothings/stb).  
> See the [`procedural`](../../tree/procedural) branch for the version using only custom CG algorithms (no textures).

---

## 🎬 Gameplay

- Run, jump, and double-jump across 5 level sections
- Wall slide and wall jump off platform edges
- Stomp enemies from above to defeat them (or get knocked back!)
- Collect all 27 coins to win
- 3 lives — falling off the map or touching enemies costs a life

## 🎮 Controls

| Key | Action |
|-----|--------|
| `A` / `D` or `←` / `→` | Move left / right |
| `W` / `Space` / `↑` | Jump (press again for double jump) |
| `P` | Pause |
| `ESC` | Quit / Back to menu |
| `R` | Restart (on Game Over / Win screen) |
| `Q` | Quit to menu (on Pause / Game Over / Win) |

## 🖼️ Rendering

This branch uses a **hybrid rendering approach**:

| Element | Rendering Method |
|---------|-----------------|
| Player | Sprite sheet (4 frames: idle, run×2, jump) |
| Coins | Animated sprite sheet (6-frame rotation) |
| Enemies | Sprite sheet (2-frame walk cycle) |
| Platforms | Tiled textures (grass, stone, moving) |
| Clouds | Sprite with parallax scrolling |
| Particles | Soft circle sprite with tinting |
| Background | Procedural (gradient sky, mountains, sun) |
| UI / HUD | Procedural (hearts, text, overlays) |

Sprites are **pixel art PNGs** generated programmatically by `tools/gen_sprites.cpp` and loaded at runtime via `stb_image` with `GL_NEAREST` filtering for crisp pixel-art scaling.

## 🏗️ Project Structure

```
pixel-hero/
├── include/            # Header files
│   ├── game.h          # Game state machine & logic
│   ├── player.h        # Player physics & movement
│   ├── platform.h      # Platform definitions
│   ├── enemy.h         # Enemy AI (patrol behavior)
│   ├── collectible.h   # Coin collectibles
│   ├── particle.h      # Particle system
│   ├── renderer.h      # Sprite + procedural rendering
│   ├── texture.h       # TextureManager (sprite loading/drawing)
│   ├── graphics.h      # Core CG algorithm declarations
│   ├── types.h         # Color, Point, shared types
│   └── constants.h     # Game constants & physics tuning
├── src/                # Source files
│   ├── main.cpp        # GLUT setup & callbacks
│   ├── game.cpp        # State machine, collision, input
│   ├── player.cpp      # Movement, jumping, wall mechanics
│   ├── platform.cpp    # Level layout (5 sections)
│   ├── enemy.cpp       # Patrol enemies & placement
│   ├── collectible.cpp # Coin placement & animation
│   ├── particle.cpp    # Particle effects
│   ├── renderer.cpp    # Sprite-based drawing
│   ├── texture.cpp     # PNG loading & textured quads
│   └── graphics.cpp    # CG algorithm implementations
├── assets/
│   └── sprites/        # Generated PNG sprite sheets
│       ├── player.png  # 128×32 (4 frames of 32×32)
│       ├── coin.png    # 96×16  (6 frames of 16×16)
│       ├── enemy.png   # 48×24  (2 frames of 24×24)
│       ├── tile_grass.png   # 32×32 ground tile
│       ├── tile_stone.png   # 32×32 floating platform tile
│       ├── tile_moving.png  # 32×32 moving platform tile
│       ├── cloud.png        # 64×32 cloud
│       └── particle.png     # 8×8 soft circle
├── vendor/             # Third-party header-only libraries
│   ├── stb_image.h     # PNG/JPEG loader (public domain)
│   └── stb_image_write.h  # PNG writer (for sprite generation)
├── tools/
│   └── gen_sprites.cpp # Sprite generator (creates all PNGs)
├── build/              # Compiled output (gitignored)
├── .clang-format       # Code formatting config
├── Makefile            # Build system
└── README.md
```

## 🔧 Build & Run

### Dependencies

- **g++** (C++11)
- **FreeGLUT** (`freeglut`)
- **Mesa OpenGL** (`mesa`, `glu`)
- **clang-format** (optional, for `make format`)

On Arch Linux:
```bash
sudo pacman -S freeglut mesa glu
```

On Ubuntu/Debian:
```bash
sudo apt install freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Build

```bash
make            # Build the game
make run        # Build and run
make clean      # Remove build artifacts
make rebuild    # Clean + build
make sprites    # Regenerate sprite PNGs
make format     # Format code with clang-format
```

## 🎨 Features

### Game States
- **Menu** — Animated starfield, bobbing title, player sprite preview, spinning coins
- **Playing** — Full gameplay with HUD (hearts, animated coin icon, timer, jump counter)
- **Paused** — Dark overlay with resume/quit options
- **Game Over** — Red-tinted screen with shaking text, score display
- **Win** — Golden celebration particles, final score

### Visual Effects
- Parallax scrolling background (mountains, hills, cloud sprites)
- Gradient sky with animated sun and rays
- Tiled platform textures (grass-topped ground, stone bricks, glowing moving platforms)
- Animated player sprite (idle, run cycle, jump)
- Animated coin sprites (6-frame rotation with glow)
- Enemy sprites with walk cycle and bounce
- Soft-circle particle sprites with color tinting
- Camera shake on hard landings, stomps, and damage
- Screen flash effects (gold on coin collect, red on damage)

### Gameplay
- Double jump with jump counter
- Wall sliding (slow fall when pressing into a wall)
- Wall jumping (launch away from wall)
- Patrol enemies with stomp-to-kill mechanic
- Camera look-ahead based on movement direction
- 5 level sections with increasing difficulty (~3200px wide)

## 🔀 Branches

| Branch | Rendering | Description |
|--------|-----------|-------------|
| `master` | Sprites + textures | PNG sprite sheets loaded via stb_image |
| `procedural` | Pure algorithms | DDA, Bresenham, Midpoint Circle, Scanline Fill |

---

*Originally a computer graphics project, now extended with sprite-based rendering for a polished pixel-art look.*
