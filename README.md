# 🎮 Pixel Hero

A **2D platformer game** built with **C++ and OpenGL (GLUT)**, featuring entirely procedural graphics rendered using custom-implemented computer graphics algorithms.

> **Branch:** `procedural` — All visuals are drawn using raw graphics primitives (no textures/sprites).  
> See the [`master`](../../tree/master) branch for the sprite-based version.

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

## 🖥️ Graphics Algorithms

All rendering is done using custom implementations of fundamental CG algorithms — no texture loading, no sprite sheets, no image files.

| Algorithm | Used For |
|-----------|----------|
| **DDA Line Drawing** | Platform edges, sun rays, HUD elements |
| **Bresenham's Line** | Player body outline, enemy outlines, legs/arms |
| **Midpoint Circle** | Player head, eyes, coins, clouds, sun, particles |
| **Scanline Fill** | Player body, platform bodies, mountains, hearts |
| **Cohen-Sutherland Clipping** | Off-screen platform culling |

## 🏗️ Project Structure

```
pixel-hero/
├── include/          # Header files
│   ├── game.h        # Game state machine & logic
│   ├── player.h      # Player physics & movement
│   ├── platform.h    # Platform definitions
│   ├── enemy.h       # Enemy AI (patrol behavior)
│   ├── collectible.h # Coin collectibles
│   ├── particle.h    # Particle system
│   ├── renderer.h    # Procedural rendering
│   ├── graphics.h    # Core CG algorithm declarations
│   ├── types.h       # Color, Point, shared types
│   └── constants.h   # Game constants & physics tuning
├── src/              # Source files
│   ├── main.cpp      # GLUT setup & callbacks
│   ├── game.cpp      # State machine, collision, input
│   ├── player.cpp    # Movement, jumping, wall mechanics
│   ├── platform.cpp  # Level layout (5 sections)
│   ├── enemy.cpp     # Patrol enemies & placement
│   ├── collectible.cpp # Coin placement & animation
│   ├── particle.cpp  # Particle effects
│   ├── renderer.cpp  # All drawing (procedural)
│   └── graphics.cpp  # CG algorithm implementations
├── build/            # Compiled output (gitignored)
├── Makefile          # Build system
└── README.md
```

## 🔧 Build & Run

### Dependencies

- **g++** (C++11)
- **FreeGLUT** (`freeglut`)
- **Mesa OpenGL** (`mesa`, `glu`)

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
```

## 🎨 Features

### Game States
- **Menu** — Animated starfield, bobbing title, blinking start prompt
- **Playing** — Full gameplay with HUD (hearts, score, timer, jump counter)
- **Paused** — Dark overlay with resume/quit options
- **Game Over** — Red-tinted screen with shaking text, score display
- **Win** — Golden celebration particles, final score

### Visual Effects
- Parallax scrolling background (mountains, hills, clouds)
- Gradient sky with animated sun and rays
- Grass-topped ground platforms with grass blades
- Brick-textured floating platforms (purple tint for moving ones)
- Player character with head, body, arms, legs, and run/jump animations
- Wall slide dust particles and pose
- Camera shake on hard landings, stomps, and damage
- Screen flash effects (gold on coin collect, red on damage)
- Dust trail particles when running

### Gameplay
- Double jump with jump counter
- Wall sliding (slow fall when pressing into a wall)
- Wall jumping (launch away from wall)
- Patrol enemies with stomp-to-kill mechanic
- Camera look-ahead based on movement direction
- 5 level sections with increasing difficulty (~3200px wide)

---

*Built as a computer graphics project demonstrating CG algorithms in a real-time game context.*
