#include "player.h"
#include <cmath>

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
    onWall = false;
    wallSliding = false;
    wallDirection = 0;
    wallJumpCooldown = 0;
}

void Player::update() {
    wasOnGround = onGround;
    
    // Update animation timer
    animationTimer += 0.1f;
    
    // Decrease wall jump cooldown
    if (wallJumpCooldown > 0) wallJumpCooldown -= 0.016f;
    
    // Apply physics
    if (!onGround) {
        if (wallSliding) {
            // Slow fall while wall sliding
            vy -= GRAVITY * 0.3f;
            if (vy < -3.0f) vy = -3.0f;  // Slow terminal velocity on wall
            vx *= 0.9f;
        } else {
            vy -= GRAVITY;
            if (vy < MAX_FALL_SPEED) {
                vy = MAX_FALL_SPEED;
            }
            vx *= AIR_RESISTANCE;
        }
    } else {
        vx *= GROUND_FRICTION;
        jumpCount = 0;
        wallSliding = false;
        onWall = false;
    }
    
    // Smooth acceleration towards target velocity (skip during wall jump cooldown)
    if (wallJumpCooldown <= 0) {
        float velocityDiff = targetVx - vx;
        vx += velocityDiff * ACCELERATION;
    }
    
    // Update position
    x += vx;
    y += vy;
    
    // Update squash and stretch animation
    if (onGround && fabs(vx) > 0.5f) {
        squashScale = 1.0f + sin(animationTimer * 0.3f) * 0.1f;
    } else if (wallSliding) {
        squashScale = 0.9f;  // Slightly compressed on wall
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
    
    // Reset wall state each frame (game.cpp will re-set it during collision)
    onWall = false;
    wallSliding = false;
}

void Player::jump() {
    if (jumpCount < maxJumps) {
        vy = JUMP_VELOCITY;
        jumpCount++;
        onGround = false;
        wallSliding = false;
        squashScale = 0.8f;
    }
}

void Player::wallJump() {
    if (!wallSliding) return;
    
    vy = JUMP_VELOCITY * 0.9f;
    vx = wallDirection * (-PLAYER_SPEED * 2.5f);  // Jump away from wall
    jumpCount = 1;  // Allow one more jump in air
    onGround = false;
    wallSliding = false;
    onWall = false;
    wallJumpCooldown = 0.2f;  // Brief cooldown to prevent immediate re-sticking
    facingRight = (wallDirection < 0);  // Face away from wall
    squashScale = 0.75f;
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