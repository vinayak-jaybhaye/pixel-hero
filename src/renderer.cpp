#include "renderer.h"
#include "graphics.h"
#include "constants.h"
#include <GL/glut.h>
#include <cmath>
#include <string>

Renderer::Renderer() : gameTime(0) {}

void Renderer::loadAssets() {
    TextureManager& tm = TextureManager::getInstance();
    printf("Loading sprite assets...\n");
    tm.loadSprite("player", "assets/sprites/player.png", 4);
    tm.loadSprite("coin", "assets/sprites/coin.png", 6);
    tm.loadSprite("enemy", "assets/sprites/enemy.png", 2);
    tm.loadSprite("tile_grass", "assets/sprites/tile_grass.png", 1);
    tm.loadSprite("tile_stone", "assets/sprites/tile_stone.png", 1);
    tm.loadSprite("tile_moving", "assets/sprites/tile_moving.png", 1);
    tm.loadSprite("cloud", "assets/sprites/cloud.png", 1);
    tm.loadSprite("particle", "assets/sprites/particle.png", 1);
    printf("All assets loaded.\n");
}

// ─────────────────────────────────────────
// Helper Methods
// ─────────────────────────────────────────

void Renderer::drawText(const std::string& text, float x, float y, void* font) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void Renderer::drawTextCentered(const std::string& text, float y, void* font) {
    int charWidth = 10;
    if (font == GLUT_BITMAP_HELVETICA_18)
        charWidth = 10;
    else if (font == GLUT_BITMAP_HELVETICA_12)
        charWidth = 7;
    else if (font == GLUT_BITMAP_9_BY_15)
        charWidth = 9;

    float x = (WINDOW_WIDTH - text.length() * charWidth) / 2.0f;
    drawText(text, x, y, font);
}

void Renderer::drawHeart(float x, float y, float size, bool filled) {
    Color heartColor = filled ? Color(0.9f, 0.15f, 0.2f) : Color(0.3f, 0.3f, 0.3f, 0.5f);

    float r = size * 0.3f;
    drawCircleMidpoint(x - r, y + r * 0.5f, r, heartColor, true);
    drawCircleMidpoint(x + r, y + r * 0.5f, r, heartColor, true);

    std::vector<Point> triangle = {Point(x - size * 0.6f, y), Point(x + size * 0.6f, y),
                                   Point(x, y - size * 0.7f)};
    scanLineFill(triangle, heartColor);
}

void Renderer::drawCoinIcon(float x, float y, float size) {
    TextureManager& tm = TextureManager::getInstance();
    int frame = ((int)(gameTime * 8)) % 6;
    tm.drawSprite("coin", x, y, size / 8.0f, size / 8.0f, frame);
}

// ─────────────────────────────────────────
// Background (stays procedural + cloud sprites)
// ─────────────────────────────────────────

void Renderer::drawBackground(float cameraX) {
    // Gradient sky
    for (int y = 0; y < WINDOW_HEIGHT; y += 3) {
        float t = (float)y / WINDOW_HEIGHT;
        float r = 0.35f + t * 0.3f;
        float g = 0.55f + t * 0.25f;
        float b = 0.75f + t * 0.15f;
        glColor3f(r, g, b);
        glBegin(GL_LINES);
        glVertex2f(0, y);
        glVertex2f(WINDOW_WIDTH, y);
        glEnd();
    }

    // Distant mountains (procedural — looks great at this scale)
    std::vector<Point> mountain1 = {
        Point(-200 - cameraX * 0.1f, 80),  Point(100 - cameraX * 0.1f, 280),
        Point(300 - cameraX * 0.1f, 220),  Point(500 - cameraX * 0.1f, 320),
        Point(700 - cameraX * 0.1f, 200),  Point(900 - cameraX * 0.1f, 350),
        Point(1200 - cameraX * 0.1f, 180), Point(1400 - cameraX * 0.1f, 80)};
    scanLineFill(mountain1, Color(0.3f, 0.25f, 0.45f, 0.6f));

    // Closer hills
    std::vector<Point> hills = {Point(-100 - cameraX * 0.2f, 60), Point(150 - cameraX * 0.2f, 180),
                                Point(350 - cameraX * 0.2f, 120), Point(550 - cameraX * 0.2f, 200),
                                Point(800 - cameraX * 0.2f, 140), Point(1000 - cameraX * 0.2f, 190),
                                Point(1200 - cameraX * 0.2f, 60)};
    scanLineFill(hills, Color(0.25f, 0.45f, 0.3f, 0.5f));

    // Cloud sprites (replacing procedural circles)
    TextureManager& tm = TextureManager::getInstance();
    float cloudPositions[][3] = {{150, 550, 0.15f}, {400, 520, 0.2f},   {700, 580, 0.12f},
                                 {950, 540, 0.18f}, {1200, 560, 0.15f}, {1500, 530, 0.22f}};

    for (int i = 0; i < 6; i++) {
        float parallaxX = cloudPositions[i][0] - cameraX * cloudPositions[i][2] +
                          sin(gameTime * 0.5f + i * 1.5f) * 15;
        float cloudY = cloudPositions[i][1] + cos(gameTime * 0.3f + i) * 4;

        if (parallaxX > -100 && parallaxX < WINDOW_WIDTH + 100) {
            float cloudScale = 1.5f + i * 0.2f;
            tm.drawSprite("cloud", parallaxX, cloudY, cloudScale, cloudScale, 0, false, 1.0f, 1.0f,
                          1.0f, 0.75f);
        }
    }

    // Sun (procedural — glow effect)
    float sunX = 800 - cameraX * 0.05f;
    float sunY = 620 + sin(gameTime * 0.3f) * 3;

    for (int r = 50; r > 30; r -= 4) {
        float alpha = 0.1f * (1.0f - (float)(r - 30) / 20.0f);
        drawCircleMidpoint(sunX, sunY, r, Color(1.0f, 0.95f, 0.5f, alpha), true);
    }
    drawCircleMidpoint(sunX, sunY, 28, Color(1.0f, 0.92f, 0.4f), true);

    for (int i = 0; i < 12; i++) {
        float angle = i * 0.524f + gameTime * 0.3f;
        float rayLength = 12 + sin(gameTime * 2.0f + i * 0.7f) * 4;
        drawLineDDA(sunX + cos(angle) * 32, sunY + sin(angle) * 32,
                    sunX + cos(angle) * (32 + rayLength), sunY + sin(angle) * (32 + rayLength),
                    Color(1.0f, 1.0f, 0.7f, 0.5f));
    }
}

// ─────────────────────────────────────────
// Player (sprite-based)
// ─────────────────────────────────────────

void Renderer::drawPlayer(const Player& player, float cameraX) {
    TextureManager& tm = TextureManager::getInstance();
    float screenX = player.x - cameraX;
    float drawY = player.y;

    // Determine animation frame
    int frame = 0;
    if (!player.onGround) {
        frame = 3;  // Jump frame
    } else if (fabs(player.vx) > 1.0f) {
        // Alternate between run frames
        frame = 1 + ((int)(player.animationTimer * 0.5f) % 2);
    } else {
        frame = 0;  // Idle
    }

    // Scale and flip
    float scaleX = 1.8f * player.squashScale;
    float scaleY = 1.8f * (2.0f - player.squashScale);
    bool flipX = !player.facingRight;

    // Wall slide visual: slightly tilted by adjusting position
    if (player.wallSliding) {
        frame = 3;  // Use jump frame for wall slide
        // Tint slightly blue when wall sliding
        tm.drawSprite("player", screenX, drawY, scaleX, scaleY, frame, flipX, 0.8f, 0.85f, 1.0f,
                      1.0f);
    } else {
        tm.drawSprite("player", screenX, drawY, scaleX, scaleY, frame, flipX);
    }
}

// ─────────────────────────────────────────
// Platforms (sprite-tiled)
// ─────────────────────────────────────────

void Renderer::drawPlatforms(const std::vector<Platform>& platforms, float cameraX) {
    TextureManager& tm = TextureManager::getInstance();

    for (const auto& platform : platforms) {
        float screenX = platform.x - cameraX;

        // Frustum culling
        if (screenX + platform.width < -50 || screenX > WINDOW_WIDTH + 50) continue;

        bool isGround = platform.height > 20;

        // Choose tile based on platform type
        std::string tileName;
        if (isGround) {
            tileName = "tile_grass";
        } else if (platform.isMoving) {
            tileName = "tile_moving";
        } else {
            tileName = "tile_stone";
        }

        // Draw the tile repeated across the platform
        tm.drawTiled(tileName, screenX, platform.y, platform.width, platform.height);

        // Subtle outline
        Color edgeColor(0.1f, 0.1f, 0.1f, 0.4f);
        drawLineDDA(screenX, platform.y, screenX + platform.width, platform.y, edgeColor);
        drawLineDDA(screenX + platform.width, platform.y, screenX + platform.width,
                    platform.y + platform.height, edgeColor);
        drawLineDDA(screenX + platform.width, platform.y + platform.height, screenX,
                    platform.y + platform.height, edgeColor);
        drawLineDDA(screenX, platform.y + platform.height, screenX, platform.y, edgeColor);

        // Moving platform glow
        if (platform.isMoving) {
            float glowAlpha = 0.2f + 0.15f * sin(gameTime * 3);
            drawCircleMidpoint(screenX + platform.width / 2, platform.y + platform.height + 3, 4,
                               Color(0.7f, 0.5f, 1.0f, glowAlpha), true);
        }
    }
}

// ─────────────────────────────────────────
// Collectibles (sprite-based)
// ─────────────────────────────────────────

void Renderer::drawCollectibles(const std::vector<Collectible>& collectibles, float cameraX) {
    TextureManager& tm = TextureManager::getInstance();

    for (const auto& coin : collectibles) {
        if (coin.collected) continue;

        float screenX = coin.x - cameraX;
        float drawY = coin.y + sin(coin.bobOffset) * 5;

        if (screenX > -50 && screenX < WINDOW_WIDTH + 50) {
            // Animated coin sprite
            int frame = ((int)(coin.rotation * 3)) % 6;
            float pulse = 1.0f + 0.08f * sin(coin.rotation * 2);

            // Outer glow (procedural)
            drawCircleMidpoint(screenX, drawY, 14, Color(1.0f, 0.9f, 0.3f, 0.15f), true);

            // Coin sprite
            tm.drawSprite("coin", screenX, drawY, 2.0f * pulse, 2.0f * pulse, frame);
        }
    }
}

// ─────────────────────────────────────────
// Enemies (sprite-based)
// ─────────────────────────────────────────

void Renderer::drawEnemies(const std::vector<Enemy>& enemies, float cameraX) {
    TextureManager& tm = TextureManager::getInstance();

    for (const auto& enemy : enemies) {
        if (!enemy.alive) continue;

        float screenX = enemy.x - cameraX;
        if (screenX < -50 || screenX > WINDOW_WIDTH + 50) continue;

        float bounce = fabs(sin(enemy.animationTimer * 0.3f)) * 3;
        float drawY = enemy.y + bounce;

        // Animated enemy sprite
        int frame = ((int)(enemy.animationTimer * 0.3f)) % 2;
        bool flipX = !enemy.facingRight;

        tm.drawSprite("enemy", screenX, drawY, 1.5f, 1.5f, frame, flipX);
    }
}

// ─────────────────────────────────────────
// Particles (sprite-based soft circles)
// ─────────────────────────────────────────

void Renderer::drawParticles(const std::vector<Particle>& particles, float cameraX) {
    TextureManager& tm = TextureManager::getInstance();

    for (const auto& particle : particles) {
        if (particle.isDead()) continue;

        float screenX = particle.x - cameraX;
        if (screenX > -10 && screenX < WINDOW_WIDTH + 10) {
            float size = 0.5f + particle.color.a * 0.8f;
            tm.drawSprite("particle", screenX, particle.y, size, size, 0, false, particle.color.r,
                          particle.color.g, particle.color.b, particle.color.a);
        }
    }
}

// ─────────────────────────────────────────
// HUD
// ─────────────────────────────────────────

void Renderer::drawHUD(int score, int lives, float timer, const Player& player) {
    // Semi-transparent HUD background bar
    glColor4f(0.0f, 0.0f, 0.0f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(0, WINDOW_HEIGHT - 45);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT - 45);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    // Lives (hearts)
    for (int i = 0; i < 3; i++) {
        drawHeart(25 + i * 30, WINDOW_HEIGHT - 25, 10, i < lives);
    }

    // Coin icon + score
    drawCoinIcon(130, WINDOW_HEIGHT - 22, 8);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(std::to_string(score), 145, WINDOW_HEIGHT - 27, GLUT_BITMAP_HELVETICA_18);

    // Timer
    int minutes = (int)timer / 60;
    int seconds = (int)timer % 60;
    char timerBuf[32];
    snprintf(timerBuf, sizeof(timerBuf), "%d:%02d", minutes, seconds);
    glColor3f(0.9f, 0.9f, 0.9f);
    drawText(timerBuf, WINDOW_WIDTH - 80, WINDOW_HEIGHT - 27, GLUT_BITMAP_HELVETICA_18);

    // Jump counter
    glColor3f(0.7f, 0.8f, 1.0f);
    std::string jumpInfo = "Jumps: " + std::to_string(player.maxJumps - player.jumpCount) + "/" +
                           std::to_string(player.maxJumps);
    drawText(jumpInfo, WINDOW_WIDTH / 2 - 30, WINDOW_HEIGHT - 27, GLUT_BITMAP_HELVETICA_12);
}

// ─────────────────────────────────────────
// Menu Screen
// ─────────────────────────────────────────

void Renderer::drawMenuScreen() {
    // Gradient background
    for (int y = 0; y < WINDOW_HEIGHT; y += 2) {
        float t = (float)y / WINDOW_HEIGHT;
        glColor3f(0.08f + t * 0.1f, 0.05f + t * 0.15f, 0.15f + t * 0.2f);
        glBegin(GL_LINES);
        glVertex2f(0, y);
        glVertex2f(WINDOW_WIDTH, y);
        glEnd();
    }

    // Animated stars
    for (int i = 0; i < 40; i++) {
        float sx = (i * 157 + sin(gameTime * 0.5f + i) * 20);
        sx = fmod(sx, (float)WINDOW_WIDTH);
        float sy = (i * 83) % WINDOW_HEIGHT;
        float twinkle = 0.3f + 0.7f * fabs(sin(gameTime * 2 + i * 0.8f));
        glColor4f(1.0f, 1.0f, 1.0f, twinkle);
        glPointSize(1.0f + twinkle);
        glBegin(GL_POINTS);
        glVertex2f(sx, sy);
        glEnd();
    }
    glPointSize(1.0f);

    // Title
    float titleBob = sin(gameTime * 1.5f) * 8;
    glColor3f(1.0f, 0.85f, 0.3f);
    drawTextCentered("P I X E L   H E R O", WINDOW_HEIGHT / 2 + 80 + titleBob,
                     GLUT_BITMAP_HELVETICA_18);

    // Player sprite preview on title screen
    TextureManager& tm = TextureManager::getInstance();
    int previewFrame = ((int)(gameTime * 3)) % 4;
    tm.drawSprite("player", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 130 + titleBob, 3.0f, 3.0f,
                  previewFrame);

    // Subtitle
    glColor4f(0.8f, 0.8f, 0.9f, 0.8f);
    drawTextCentered("A Classic Platformer Adventure", WINDOW_HEIGHT / 2 + 40,
                     GLUT_BITMAP_HELVETICA_12);

    // Blinking "Press ENTER" text
    float blinkAlpha = 0.5f + 0.5f * sin(gameTime * 3);
    glColor4f(1.0f, 1.0f, 1.0f, blinkAlpha);
    drawTextCentered("Press ENTER or SPACE to Start", WINDOW_HEIGHT / 2 - 30,
                     GLUT_BITMAP_HELVETICA_18);

    // Controls
    glColor4f(0.6f, 0.65f, 0.7f, 0.7f);
    drawTextCentered("A/D or Arrow Keys  -  Move", WINDOW_HEIGHT / 2 - 100,
                     GLUT_BITMAP_HELVETICA_12);
    drawTextCentered("W / Space / Up  -  Jump (Double Jump!)", WINDOW_HEIGHT / 2 - 120,
                     GLUT_BITMAP_HELVETICA_12);
    drawTextCentered("P  -  Pause    |    ESC  -  Quit", WINDOW_HEIGHT / 2 - 140,
                     GLUT_BITMAP_HELVETICA_12);

    // Decorative coins
    for (int i = 0; i < 5; i++) {
        float coinX = 200 + i * 150;
        float coinY = WINDOW_HEIGHT / 2 - 200 + sin(gameTime * 2 + i) * 10;
        int coinFrame = ((int)(gameTime * 6 + i * 2)) % 6;
        tm.drawSprite("coin", coinX, coinY, 2.0f, 2.0f, coinFrame);
    }
}

// ─────────────────────────────────────────
// Pause Overlay
// ─────────────────────────────────────────

void Renderer::drawPauseOverlay() {
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextCentered("P A U S E D", WINDOW_HEIGHT / 2 + 30, GLUT_BITMAP_HELVETICA_18);

    glColor4f(0.8f, 0.8f, 0.8f, 0.8f);
    drawTextCentered("Press P or ESC to Resume", WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_HELVETICA_12);
    drawTextCentered("Press Q to Quit to Menu", WINDOW_HEIGHT / 2 - 45, GLUT_BITMAP_HELVETICA_12);
}

// ─────────────────────────────────────────
// Game Over Screen
// ─────────────────────────────────────────

void Renderer::drawGameOverScreen(int score, float timer) {
    glColor4f(0.3f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    float shake = sin(gameTime * 20) * 2;
    glColor3f(1.0f, 0.2f, 0.2f);
    drawTextCentered("G A M E   O V E R", WINDOW_HEIGHT / 2 + 60 + shake, GLUT_BITMAP_HELVETICA_18);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextCentered("Score: " + std::to_string(score), WINDOW_HEIGHT / 2 + 10,
                     GLUT_BITMAP_HELVETICA_18);

    int minutes = (int)timer / 60;
    int seconds = (int)timer % 60;
    char timerBuf[32];
    snprintf(timerBuf, sizeof(timerBuf), "Time: %d:%02d", minutes, seconds);
    drawTextCentered(std::string(timerBuf), WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_HELVETICA_12);

    float blinkAlpha = 0.5f + 0.5f * sin(gameTime * 3);
    glColor4f(1.0f, 1.0f, 1.0f, blinkAlpha);
    drawTextCentered("Press R to Restart", WINDOW_HEIGHT / 2 - 70, GLUT_BITMAP_HELVETICA_18);

    glColor4f(0.7f, 0.7f, 0.7f, 0.6f);
    drawTextCentered("Press Q or ESC for Menu", WINDOW_HEIGHT / 2 - 100, GLUT_BITMAP_HELVETICA_12);
}

// ─────────────────────────────────────────
// Win Screen
// ─────────────────────────────────────────

void Renderer::drawWinScreen(int score, float timer) {
    glColor4f(0.2f, 0.15f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    // Celebration particles
    TextureManager& tm = TextureManager::getInstance();
    for (int i = 0; i < 30; i++) {
        float px = fmod(i * 137.0f + gameTime * 30, (float)WINDOW_WIDTH);
        float py = fmod(i * 89.0f + gameTime * (20 + i), (float)WINDOW_HEIGHT);
        float sparkle = 0.5f + 0.5f * sin(gameTime * 5 + i);
        tm.drawSprite("particle", px, py, sparkle + 0.5f, sparkle + 0.5f, 0, false, 1.0f, 0.9f,
                      0.3f, sparkle);
    }

    float bob = sin(gameTime * 2) * 5;
    glColor3f(1.0f, 0.9f, 0.2f);
    drawTextCentered("Y O U   W I N !", WINDOW_HEIGHT / 2 + 80 + bob, GLUT_BITMAP_HELVETICA_18);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextCentered("All coins collected!", WINDOW_HEIGHT / 2 + 40, GLUT_BITMAP_HELVETICA_12);
    drawTextCentered("Final Score: " + std::to_string(score), WINDOW_HEIGHT / 2,
                     GLUT_BITMAP_HELVETICA_18);

    int minutes = (int)timer / 60;
    int seconds = (int)timer % 60;
    char timerBuf[32];
    snprintf(timerBuf, sizeof(timerBuf), "Time: %d:%02d", minutes, seconds);
    drawTextCentered(std::string(timerBuf), WINDOW_HEIGHT / 2 - 30, GLUT_BITMAP_HELVETICA_12);

    float blinkAlpha = 0.5f + 0.5f * sin(gameTime * 3);
    glColor4f(1.0f, 1.0f, 1.0f, blinkAlpha);
    drawTextCentered("Press R to Play Again", WINDOW_HEIGHT / 2 - 80, GLUT_BITMAP_HELVETICA_18);

    glColor4f(0.7f, 0.7f, 0.7f, 0.6f);
    drawTextCentered("Press Q or ESC for Menu", WINDOW_HEIGHT / 2 - 110, GLUT_BITMAP_HELVETICA_12);
}

// ─────────────────────────────────────────
// Screen Flash
// ─────────────────────────────────────────

void Renderer::drawScreenFlash(float r, float g, float b, float alpha) {
    if (alpha <= 0) return;
    glColor4f(r, g, b, alpha);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
}