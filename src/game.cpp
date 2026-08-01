#include "game.h"
#include "constants.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game() : cameraX(0), cameraTargetX(0), score(0), lives(3), 
               gameTimer(0), totalCoins(0), state(GameState::MENU),
               stateTransitionTimer(0), damageFlashTimer(0),
               cameraShakeTimer(0), cameraShakeIntensity(0) {
    for (int i = 0; i < 256; i++) {
        keys[i] = false;
    }
}

void Game::init() {
    srand(time(nullptr));
    resetLevel();
}

void Game::resetLevel() {
    initializePlatforms(platforms);
    initializeCollectibles(collectibles);
    initializeEnemies(enemies);
    totalCoins = collectibles.size();
    player.reset();
    particleSystem.clear();
    cameraX = 0;
    cameraTargetX = 0;
    score = 0;
    gameTimer = 0;
    damageFlashTimer = 0;
    cameraShakeTimer = 0;
}

bool Game::checkCollision(float x, float y, float width, float height, const Platform& platform) {
    return x < platform.x + platform.width &&
           x + width > platform.x &&
           y < platform.y + platform.height &&
           y + height > platform.y;
}

void Game::checkCollectibleCollection() {
    int collectedCount = 0;
    for (auto& coin : collectibles) {
        if (coin.collected) {
            collectedCount++;
            continue;
        }
        
        float dx = player.x - coin.x;
        float dy = player.y - coin.y;
        float distance = sqrt(dx*dx + dy*dy);
        
        if (distance < 25) {
            coin.collected = true;
            score += 100;
            collectedCount++;
            particleSystem.createCollectionParticles(coin.x, coin.y);
            stateTransitionTimer = 0.3f;  // Brief flash
        }
    }
    
    // Win condition: all coins collected
    if (collectedCount >= totalCoins && totalCoins > 0) {
        state = GameState::WIN;
        stateTransitionTimer = 1.0f;
    }
}

void Game::checkEnemyCollisions() {
    for (auto& enemy : enemies) {
        if (!enemy.alive) continue;
        
        // Simple AABB collision between player and enemy
        float playerLeft = player.x - 12;
        float playerRight = player.x + 12;
        float playerBottom = player.y - 18;
        float playerTop = player.y + 18;
        
        float enemyLeft = enemy.x - enemy.width / 2;
        float enemyRight = enemy.x + enemy.width / 2;
        float enemyBottom = enemy.y - enemy.height / 2;
        float enemyTop = enemy.y + enemy.height / 2;
        
        if (playerRight > enemyLeft && playerLeft < enemyRight &&
            playerTop > enemyBottom && playerBottom < enemyTop) {
            
            // Check if player is stomping (falling onto enemy from above)
            if (player.vy < 0 && playerBottom > enemyBottom + enemy.height * 0.3f) {
                // Stomp kill!
                enemy.kill();
                score += 200;
                player.vy = 12.0f;  // Bounce up
                player.jumpCount = 0;  // Reset jumps after stomp
                particleSystem.createCollectionParticles(enemy.x, enemy.y);
                cameraShakeTimer = 0.15f;
                cameraShakeIntensity = 3.0f;
            } else {
                // Player takes damage
                playerTakeDamage();
            }
        }
    }
}

void Game::playerTakeDamage() {
    lives--;
    damageFlashTimer = 0.5f;
    cameraShakeTimer = 0.3f;
    cameraShakeIntensity = 5.0f;
    
    if (lives <= 0) {
        state = GameState::GAME_OVER;
        stateTransitionTimer = 1.0f;
    } else {
        // Knock player back
        player.vy = 10.0f;
        player.vx = player.facingRight ? -8.0f : 8.0f;
    }
}

void Game::updateCamera() {
    // Camera look-ahead based on movement direction
    float lookAhead = player.vx * 15;
    cameraTargetX = player.x - WINDOW_WIDTH / 2 + lookAhead;
    if (cameraTargetX < 0) cameraTargetX = 0;
    
    // Level right bound (don't show past the end)
    float maxCameraX = 3250 - WINDOW_WIDTH;
    if (maxCameraX > 0 && cameraTargetX > maxCameraX) {
        cameraTargetX = maxCameraX;
    }
    
    cameraX += (cameraTargetX - cameraX) * 0.08f;
}

void Game::update() {
    // Handle timers
    if (stateTransitionTimer > 0) stateTransitionTimer -= 0.016f;
    if (damageFlashTimer > 0) damageFlashTimer -= 0.016f;
    if (cameraShakeTimer > 0) cameraShakeTimer -= 0.016f;
    
    if (state != GameState::PLAYING) return;
    
    gameTimer += 0.016f;
    
    bool wasOnGroundBefore = player.onGround;
    float fallVelocity = player.vy;
    
    // Update player physics
    player.update();
    
    // Collision detection with platforms
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
                
                if (!wasOnGroundBefore && fallVelocity < -5) {
                    particleSystem.createLandingParticles(player.x, player.y);
                    cameraShakeTimer = 0.1f;
                    cameraShakeIntensity = fmin(fabs(fallVelocity) * 0.3f, 4.0f);
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
                    // Wall slide detection (pressing into wall while in air)
                    if (!player.onGround && player.vy < 0 && player.wallJumpCooldown <= 0) {
                        player.onWall = true;
                        player.wallSliding = true;
                        player.wallDirection = 1;  // Wall is on the right
                        player.jumpCount = 0;
                    }
                } else {
                    player.x = platform.x + platform.width + 12;
                    if (!player.onGround && player.vy < 0 && player.wallJumpCooldown <= 0) {
                        player.onWall = true;
                        player.wallSliding = true;
                        player.wallDirection = -1;  // Wall is on the left
                        player.jumpCount = 0;
                    }
                }
                player.vx = 0;
            }
        }
    }
    
    // Update enemies
    for (auto& enemy : enemies) {
        enemy.update();
    }
    
    // Check enemy collisions
    checkEnemyCollisions();
    
    // Update other systems
    checkCollectibleCollection();
    updateCamera();
    particleSystem.update();
    
    for (auto& coin : collectibles) {
        if (!coin.collected) {
            coin.update();
        }
    }
    
    // Dust particles when running
    if (player.onGround && fabs(player.vx) > 2.0f) {
        if (rand() % 5 == 0) {
            particleSystem.addParticle(
                player.x + (rand() % 10 - 5), player.y - 16,
                -player.vx * 0.2f, (rand() % 30) / 10.0f,
                Color(0.6f, 0.5f, 0.4f, 0.5f),
                15 + rand() % 10
            );
        }
    }
    
    renderer.updateGameTime();
    
    // Lose a life if fallen
    if (player.y < -100) {
        lives--;
        damageFlashTimer = 0.3f;
        if (lives <= 0) {
            state = GameState::GAME_OVER;
            stateTransitionTimer = 1.0f;
        } else {
            player.reset();
        }
    }
}

void Game::render() {
    // Apply camera shake
    float shakeX = 0, shakeY = 0;
    if (cameraShakeTimer > 0) {
        shakeX = (rand() % 100 - 50) / 50.0f * cameraShakeIntensity * (cameraShakeTimer / 0.3f);
        shakeY = (rand() % 100 - 50) / 50.0f * cameraShakeIntensity * (cameraShakeTimer / 0.3f);
        glTranslatef(shakeX, shakeY, 0);
    }
    
    switch (state) {
        case GameState::MENU:
            renderer.drawMenuScreen();
            break;
            
        case GameState::PLAYING:
        case GameState::PAUSED:
            renderer.drawBackground(cameraX);
            renderer.drawPlatforms(platforms, cameraX);
            renderer.drawCollectibles(collectibles, cameraX);
            renderer.drawEnemies(enemies, cameraX);
            renderer.drawParticles(particleSystem.getParticles(), cameraX);
            renderer.drawPlayer(player, cameraX);
            renderer.drawHUD(score, lives, gameTimer, player);
            
            if (state == GameState::PAUSED) {
                renderer.drawPauseOverlay();
            }
            
            // Coin collection flash (golden)
            if (stateTransitionTimer > 0 && state == GameState::PLAYING) {
                renderer.drawScreenFlash(1.0f, 1.0f, 0.8f, stateTransitionTimer * 0.2f);
            }
            
            // Damage flash (red)
            if (damageFlashTimer > 0) {
                renderer.drawScreenFlash(1.0f, 0.0f, 0.0f, damageFlashTimer * 0.4f);
            }
            break;
            
        case GameState::GAME_OVER:
            renderer.drawBackground(cameraX);
            renderer.drawPlatforms(platforms, cameraX);
            renderer.drawGameOverScreen(score, gameTimer);
            break;
            
        case GameState::WIN:
            renderer.drawBackground(cameraX);
            renderer.drawPlatforms(platforms, cameraX);
            renderer.drawCollectibles(collectibles, cameraX);
            renderer.drawPlayer(player, cameraX);
            renderer.drawWinScreen(score, gameTimer);
            break;
    }
    
    // Reset shake
    if (cameraShakeTimer > 0) {
        glTranslatef(-shakeX, -shakeY, 0);
    }
}

void Game::handleKeyDown(unsigned char key) {
    keys[key] = true;
    
    switch (state) {
        case GameState::MENU:
            if (key == 13 || key == ' ') {
                state = GameState::PLAYING;
                resetLevel();
                lives = 3;
            }
            if (key == 27) exit(0);
            break;
            
        case GameState::PLAYING:
            if (key == 'w' || key == 'W' || key == ' ') {
                if (player.wallSliding) {
                    player.wallJump();
                    particleSystem.createJumpParticles(player.x, player.y);
                } else if (player.jumpCount < player.maxJumps) {
                    player.jump();
                    particleSystem.createJumpParticles(player.x, player.y);
                }
            }
            if (key == 'p' || key == 'P' || key == 27) {
                state = GameState::PAUSED;
            }
            break;
            
        case GameState::PAUSED:
            if (key == 'p' || key == 'P' || key == 27) {
                state = GameState::PLAYING;
            }
            if (key == 'q' || key == 'Q') {
                state = GameState::MENU;
            }
            break;
            
        case GameState::GAME_OVER:
        case GameState::WIN:
            if (key == 'r' || key == 'R') {
                state = GameState::PLAYING;
                resetLevel();
                lives = 3;
            }
            if (key == 27 || key == 'q' || key == 'Q') {
                state = GameState::MENU;
            }
            break;
    }
}

void Game::handleKeyUp(unsigned char key) {
    keys[key] = false;
}

void Game::handleSpecialDown(int key) {
    if (state != GameState::PLAYING) return;
    
    switch(key) {
        case GLUT_KEY_LEFT:
            keys['a'] = true;
            break;
        case GLUT_KEY_RIGHT:
            keys['d'] = true;
            break;
        case GLUT_KEY_UP:
            if (player.wallSliding) {
                player.wallJump();
                particleSystem.createJumpParticles(player.x, player.y);
            } else if (player.jumpCount < player.maxJumps) {
                player.jump();
                particleSystem.createJumpParticles(player.x, player.y);
            }
            break;
    }
}

void Game::handleSpecialUp(int key) {
    if (state != GameState::PLAYING) return;
    
    switch(key) {
        case GLUT_KEY_LEFT:
            keys['a'] = false;
            break;
        case GLUT_KEY_RIGHT:
            keys['d'] = false;
            break;
    }
}

void Game::processInput() {
    if (state != GameState::PLAYING) return;
    
    if (keys['a'] || keys['A']) {
        player.moveLeft();
    } else if (keys['d'] || keys['D']) {
        player.moveRight();
    } else {
        player.stopMoving();
    }
}