#include "platform.h"
#include <cmath>

Platform::Platform(float px, float py, float w, float h, Color c, bool moving, float speed,
                   float range)
    : x(px),
      y(py),
      width(w),
      height(h),
      color(c),
      isMoving(moving),
      moveSpeed(speed),
      moveRange(range),
      originalX(px),
      moveTimer(0) {}

void Platform::update() {
    if (isMoving) {
        moveTimer += moveSpeed;
        x = originalX + sin(moveTimer) * moveRange;
    }
}

void initializePlatforms(std::vector<Platform>& platforms) {
    platforms.clear();

    Color groundColor(0.2f, 0.7f, 0.2f);
    Color floatColor(0.8f, 0.5f, 0.2f);
    Color highColor(0.6f, 0.3f, 0.8f);
    Color moveColor(0.5f, 0.4f, 0.7f);

    // ── Section 1: Starting area (0 - 800) ──
    platforms.push_back(Platform(0, 50, 400, 25, groundColor));
    platforms.push_back(Platform(450, 50, 300, 25, groundColor));

    platforms.push_back(Platform(250, 180, 120, 18, floatColor));
    platforms.push_back(Platform(450, 250, 100, 18, moveColor, true, 0.02f, 50));
    platforms.push_back(Platform(200, 320, 90, 18, floatColor));
    platforms.push_back(Platform(100, 450, 80, 18, floatColor));
    platforms.push_back(Platform(400, 500, 100, 18, highColor));

    // ── Section 2: Gap challenge (800 - 1400) ──
    platforms.push_back(Platform(800, 50, 400, 25, groundColor));
    platforms.push_back(Platform(850, 200, 100, 18, floatColor));
    platforms.push_back(Platform(1000, 320, 120, 18, moveColor, true, 0.025f, 60));
    platforms.push_back(Platform(700, 450, 90, 18, highColor));
    platforms.push_back(Platform(600, 380, 130, 18, moveColor, true, 0.03f, 80));

    // Stepping stones over a big gap
    platforms.push_back(Platform(1250, 150, 60, 15, floatColor));
    platforms.push_back(Platform(1350, 220, 60, 15, moveColor, true, 0.035f, 40));
    platforms.push_back(Platform(1450, 150, 60, 15, floatColor));

    // ── Section 3: Vertical climb (1400 - 2000) ──
    platforms.push_back(Platform(1300, 50, 350, 25, groundColor));
    platforms.push_back(Platform(1700, 50, 300, 25, groundColor));

    platforms.push_back(Platform(1500, 180, 100, 18, floatColor));
    platforms.push_back(Platform(1350, 280, 80, 18, floatColor));
    platforms.push_back(Platform(1550, 360, 110, 18, moveColor, true, 0.02f, 45));
    platforms.push_back(Platform(1400, 440, 90, 18, floatColor));
    platforms.push_back(Platform(1600, 520, 100, 18, highColor));

    platforms.push_back(Platform(1800, 200, 120, 18, floatColor));
    platforms.push_back(Platform(1900, 320, 80, 18, moveColor, true, 0.028f, 55));

    // ── Section 4: Danger zone (2000 - 2600) ──
    platforms.push_back(Platform(2050, 50, 300, 25, groundColor));
    platforms.push_back(Platform(2400, 50, 250, 25, groundColor));

    // Narrow platforms with gaps
    platforms.push_back(Platform(2100, 180, 70, 15, floatColor));
    platforms.push_back(Platform(2220, 250, 70, 15, moveColor, true, 0.03f, 35));
    platforms.push_back(Platform(2340, 180, 70, 15, floatColor));
    platforms.push_back(Platform(2450, 300, 90, 18, floatColor));
    platforms.push_back(Platform(2300, 400, 100, 18, highColor));
    platforms.push_back(Platform(2500, 450, 80, 18, moveColor, true, 0.025f, 50));

    // ── Section 5: Final stretch (2600 - 3200) ──
    platforms.push_back(Platform(2700, 50, 500, 25, groundColor));

    platforms.push_back(Platform(2750, 180, 100, 18, floatColor));
    platforms.push_back(Platform(2900, 280, 120, 18, moveColor, true, 0.02f, 60));
    platforms.push_back(Platform(3050, 180, 80, 18, floatColor));
    platforms.push_back(Platform(2850, 400, 100, 18, highColor));
    platforms.push_back(Platform(3000, 480, 120, 18, highColor));

    // End platform (goal)
    platforms.push_back(Platform(3100, 50, 150, 25, Color(0.8f, 0.7f, 0.2f)));
}