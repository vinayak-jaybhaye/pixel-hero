#include "game.h"
#include "constants.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game() : cameraX(0), cameraTargetX(0), score(0) {
    for (int i = 0; i < 256; i++) {
        keys[i] = false;
    }
}

void Game::init() {
    srand(time(nullptr));
    initializePlatforms(platforms);
    initializeCollectibles(collectibles);
    player.reset();
}

bool Game::checkCollision(float x, float y, float width, float height, const Platform& platform) {
    return x < platform.x + platform.width &&
           x + width > platform.x &&
           y < platform.y + platform.height &&
           y + height > platform.y;
}

void Game::checkCollectibleCollection() {
    for (auto& coin : collectibles) {
        if (coin.collected) continue;
        
        float dx = player.x - coin.x;
        float dy = player.y - coin.y;
        float distance = sqrt(dx*dx + dy*dy);
        
        if (distance < 25) {
            coin.collected = true;
            score += 100;
            particleSystem.createCollectionParticles(coin.x, coin.y);
        }
    }
}

void Game::updateCamera() {
    cameraTargetX = player.x - WINDOW_WIDTH / 2;
    if (cameraTargetX < 0) cameraTargetX = 0;
    cameraX += (cameraTargetX - cameraX) * 0.08f;
}

void Game::update() {
    bool wasOnGroundBefore = player.onGround;
    
    // Update player physics
    player.update();
    
    // Collision detection
    player.onGround = false;
    for (auto& platform : platforms) {
        platform.update();
        
        if (checkCollision(player.x - 12, player.y - 18, 24, 36, platform)) {
            // Landing on top
            if (player.vy <= 0 && player.y - 18 < platform.y + platform.height && 
                player.y > platform.y + platform.height - 10) {
                player.y = platform.y + platform.height + 18;
                player.vy = 0;
                player.onGround = true;
                
                if (!wasOnGroundBefore && player.vy < -5) {
                    particleSystem.createLandingParticles(player.x, player.y);
                }
            }
            // Hit from below
            else if (player.vy > 0 && player.y + 18 > platform.y && 
                     player.y < platform.y + 5) {
                player.y = platform.y - 18;
                player.vy = 0;
            }
            // Side collision
            else if (player.y - 18 < platform.y + platform.height && 
                     player.y + 18 > platform.y) {
                if (player.x < platform.x) {
                    player.x = platform.x - 12;
                } else {
                    player.x = platform.x + platform.width + 12;
                }
                player.vx = 0;
            }
        }
    }
    
    // Update other systems
    checkCollectibleCollection();
    updateCamera();
    particleSystem.update();
    
    for (auto& coin : collectibles) {
        if (!coin.collected) {
            coin.update();
        }
    }
    
    renderer.updateGameTime();
    
    // Reset if fallen
    if (player.y < -100) {
        player.reset();
    }
}

void Game::render() {
    renderer.drawBackground(cameraX);
    renderer.drawPlatforms(platforms, cameraX);
    renderer.drawCollectibles(collectibles, cameraX);
    renderer.drawParticles(particleSystem.getParticles(), cameraX);
    renderer.drawPlayer(player, cameraX);
    renderer.drawUI(score, player);
}

void Game::handleKeyDown(unsigned char key) {
    keys[key] = true;
    
    if ((key == 'w' || key == 'W' || key == ' ') && player.jumpCount < player.maxJumps) {
        player.jump();
        particleSystem.createJumpParticles(player.x, player.y);
    }
    
    if (key == 27) exit(0); // ESC
}

void Game::handleKeyUp(unsigned char key) {
    keys[key] = false;
}

void Game::handleSpecialDown(int key) {
    switch(key) {
        case 1: // GLUT_KEY_LEFT
            keys['a'] = true;
            break;
        case 3: // GLUT_KEY_RIGHT
            keys['d'] = true;
            break;
        case 101: // GLUT_KEY_UP
            if (player.jumpCount < player.maxJumps) {
                player.jump();
                particleSystem.createJumpParticles(player.x, player.y);
            }
            break;
    }
}

void Game::handleSpecialUp(int key) {
    switch(key) {
        case 1: // GLUT_KEY_LEFT
            keys['a'] = false;
            break;
        case 3: // GLUT_KEY_RIGHT
            keys['d'] = false;
            break;
    }
}

void Game::processInput() {
    if (keys['a'] || keys['A']) {
        player.moveLeft();
    } else if (keys['d'] || keys['D']) {
        player.moveRight();
    } else {
        player.stopMoving();
    }
}