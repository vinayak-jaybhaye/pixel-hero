#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include <vector>

class Platform {
public:
    float x, y, width, height;
    Color color;
    bool isMoving;
    float moveSpeed, moveRange, originalX;
    
    Platform(float px, float py, float w, float h, Color c, 
             bool moving = false, float speed = 0, float range = 0);
    void update();
};

void initializePlatforms(std::vector<Platform>& platforms);

#endif