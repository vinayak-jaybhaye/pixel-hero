#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include "constants.h"

class Player {
public:
    float x, y;
    float vx, vy;
    float targetVx;
    bool onGround;
    bool wasOnGround;
    Color color;
    float animationTimer;
    float squashScale;
    bool facingRight;
    int jumpCount;
    static const int maxJumps = 2;
    
    Player();
    void update();
    void jump();
    void moveLeft();
    void moveRight();
    void stopMoving();
    void reset();
};

#endif