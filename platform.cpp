#include "platform.h"
#include <cmath>

Platform::Platform(float px, float py, float w, float h, Color c, bool moving, float speed, float range) 
    : x(px), y(py), width(w), height(h), color(c), isMoving(moving), 
      moveSpeed(speed), moveRange(range), originalX(px) {}

void Platform::update() {
    if (isMoving) {
        static float time = 0;
        time += moveSpeed;
        x = originalX + sin(time) * moveRange;
    }
}

void initializePlatforms(std::vector<Platform>& platforms) {
    platforms.clear();
    
    // Ground platforms
    platforms.push_back(Platform(0, 50, 400, 25, Color(0.2f, 0.7f, 0.2f)));
    platforms.push_back(Platform(450, 50, 300, 25, Color(0.2f, 0.7f, 0.2f)));
    platforms.push_back(Platform(800, 50, 400, 25, Color(0.2f, 0.7f, 0.2f)));
    
    // Floating platforms - some moving
    platforms.push_back(Platform(250, 180, 120, 18, Color(0.8f, 0.5f, 0.2f)));
    platforms.push_back(Platform(450, 250, 100, 18, Color(0.8f, 0.5f, 0.2f), true, 0.02f, 50));
    platforms.push_back(Platform(200, 320, 90, 18, Color(0.8f, 0.5f, 0.2f)));
    platforms.push_back(Platform(600, 380, 130, 18, Color(0.8f, 0.5f, 0.2f), true, 0.03f, 80));
    platforms.push_back(Platform(100, 450, 80, 18, Color(0.8f, 0.5f, 0.2f)));
    platforms.push_back(Platform(850, 200, 100, 18, Color(0.8f, 0.5f, 0.2f)));
    platforms.push_back(Platform(1000, 320, 120, 18, Color(0.8f, 0.5f, 0.2f), true, 0.025f, 60));
    
    // High platforms
    platforms.push_back(Platform(400, 500, 100, 18, Color(0.6f, 0.3f, 0.8f)));
    platforms.push_back(Platform(700, 450, 90, 18, Color(0.6f, 0.3f, 0.8f)));
}