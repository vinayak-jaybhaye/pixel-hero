#include "player.h"

Player::Player() {
    reset();
}

void Player::reset() {
    x = 100.0f;
    y = 400.0f;
    vx = 0.0f;
    vy = 0.0f;
    targetVx = 0.0f;
    onGround = false;
    wasOnGround = false;
    color = Color(0.8f, 0.4f, 0.4f, 1.0f);  // Reddish color
    animationTimer = 0.0f;
    squashScale = 1.0f;
    facingRight = true;
    jumpCount = 0;
}

void Player::update() {
    wasOnGround = onGround;
    
    // Update animation timer
    animationTimer += 0.1f;
    
    // Apply physics
    if (!onGround) {
        vy -= GRAVITY;  // Apply gravity
        if (vy < MAX_FALL_SPEED) {
            vy = MAX_FALL_SPEED;  // Terminal velocity
        }
        vx *= AIR_RESISTANCE;  // Air resistance
    } else {
        vx *= GROUND_FRICTION;  // Ground friction
        jumpCount = 0;  // Reset jump count when on ground
    }
    
    // Smooth acceleration towards target velocity
    float velocityDiff = targetVx - vx;
    vx += velocityDiff * ACCELERATION;
    
    // Update position
    x += vx;
    y += vy;
    
    // Update squash and stretch animation
    if (onGround && abs(vx) > 0.5f) {
        squashScale = 1.0f + sin(animationTimer * 0.3f) * 0.1f;
    } else if (!onGround) {
        squashScale = 1.0f + (vy * 0.02f);
        if (squashScale < 0.7f) squashScale = 0.7f;
        if (squashScale > 1.3f) squashScale = 1.3f;
    } else {
        squashScale = 1.0f;
    }
    
    // Update facing direction
    if (vx > 0.1f) facingRight = true;
    else if (vx < -0.1f) facingRight = false;
}

void Player::jump() {
    if (jumpCount < maxJumps) {
        vy = JUMP_VELOCITY;
        jumpCount++;
        onGround = false;
        squashScale = 0.8f;  // Squash effect when jumping
    }
}

void Player::moveLeft() {
    targetVx = -PLAYER_SPEED;
    facingRight = false;
}

void Player::moveRight() {
    targetVx = PLAYER_SPEED;
    facingRight = true;
}

void Player::stopMoving() {
    targetVx = 0.0f;
}