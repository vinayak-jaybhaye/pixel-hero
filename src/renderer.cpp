#include "renderer.h"
#include "graphics.h"
#include "constants.h"
#include <GL/glut.h>
#include <cmath>
#include <string>

Renderer::Renderer() : gameTime(0) {}

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
    // Approximate character width for centering
    int charWidth = 10;
    if (font == GLUT_BITMAP_HELVETICA_18) charWidth = 10;
    else if (font == GLUT_BITMAP_HELVETICA_12) charWidth = 7;
    else if (font == GLUT_BITMAP_9_BY_15) charWidth = 9;
    
    float x = (WINDOW_WIDTH - text.length() * charWidth) / 2.0f;
    drawText(text, x, y, font);
}

void Renderer::drawHeart(float x, float y, float size, bool filled) {
    Color heartColor = filled ? Color(0.9f, 0.15f, 0.2f) : Color(0.3f, 0.3f, 0.3f, 0.5f);
    
    // Draw heart using two circles and a triangle
    float r = size * 0.3f;
    drawCircleMidpoint(x - r, y + r * 0.5f, r, heartColor, true);
    drawCircleMidpoint(x + r, y + r * 0.5f, r, heartColor, true);
    
    // Bottom triangle of the heart
    std::vector<Point> triangle = {
        Point(x - size * 0.6f, y),
        Point(x + size * 0.6f, y),
        Point(x, y - size * 0.7f)
    };
    scanLineFill(triangle, heartColor);
}

void Renderer::drawCoinIcon(float x, float y, float size) {
    float pulse = 1.0f + 0.1f * sin(gameTime * 4);
    int r = size * pulse;
    drawCircleMidpoint(x, y, r, Color(1.0f, 0.85f, 0.0f), true);
    drawCircleMidpoint(x, y, r - 2, Color(1.0f, 1.0f, 0.6f), true);
}

// ─────────────────────────────────────────
// Background
// ─────────────────────────────────────────

void Renderer::drawBackground(float cameraX) {
    // Gradient sky using scanline
    for (int y = 0; y < WINDOW_HEIGHT; y += 3) {
        float t = (float)y / WINDOW_HEIGHT;
        // Deep blue at bottom to warm sky at top
        float r = 0.35f + t * 0.3f;
        float g = 0.55f + t * 0.25f;
        float b = 0.75f + t * 0.15f;
        glColor3f(r, g, b);
        glBegin(GL_LINES);
        glVertex2f(0, y);
        glVertex2f(WINDOW_WIDTH, y);
        glEnd();
    }
    
    // Draw distant mountains (parallax layer 1 — slowest)
    std::vector<Point> mountain1 = {
        Point(-200 - cameraX * 0.1f, 80),
        Point(100 - cameraX * 0.1f, 280),
        Point(300 - cameraX * 0.1f, 220),
        Point(500 - cameraX * 0.1f, 320),
        Point(700 - cameraX * 0.1f, 200),
        Point(900 - cameraX * 0.1f, 350),
        Point(1200 - cameraX * 0.1f, 180),
        Point(1400 - cameraX * 0.1f, 80)
    };
    scanLineFill(mountain1, Color(0.3f, 0.25f, 0.45f, 0.6f));
    
    // Closer hills (parallax layer 2)
    std::vector<Point> hills = {
        Point(-100 - cameraX * 0.2f, 60),
        Point(150 - cameraX * 0.2f, 180),
        Point(350 - cameraX * 0.2f, 120),
        Point(550 - cameraX * 0.2f, 200),
        Point(800 - cameraX * 0.2f, 140),
        Point(1000 - cameraX * 0.2f, 190),
        Point(1200 - cameraX * 0.2f, 60)
    };
    scanLineFill(hills, Color(0.25f, 0.45f, 0.3f, 0.5f));
    
    // Draw animated clouds
    float cloudPositions[][3] = {
        {150, 550, 0.15f}, {400, 520, 0.2f}, {700, 580, 0.12f}, 
        {950, 540, 0.18f}, {1200, 560, 0.15f}, {1500, 530, 0.22f}
    };
    
    for (int i = 0; i < 6; i++) {
        float parallaxX = cloudPositions[i][0] - cameraX * cloudPositions[i][2] + sin(gameTime * 0.5f + i * 1.5f) * 15;
        float cloudY = cloudPositions[i][1] + cos(gameTime * 0.3f + i) * 4;
        
        if (parallaxX > -100 && parallaxX < WINDOW_WIDTH + 100) {
            Color cloudColor(1.0f, 1.0f, 1.0f, 0.7f);
            drawCircleMidpoint(parallaxX, cloudY, 22, cloudColor, true);
            drawCircleMidpoint(parallaxX + 28, cloudY + 2, 18, cloudColor, true);
            drawCircleMidpoint(parallaxX - 22, cloudY - 1, 16, cloudColor, true);
            drawCircleMidpoint(parallaxX + 10, cloudY + 8, 15, cloudColor, true);
        }
    }
    
    // Draw sun with animated glow
    float sunX = 800 - cameraX * 0.05f;
    float sunY = 620 + sin(gameTime * 0.3f) * 3;
    
    // Sun glow
    for (int r = 50; r > 30; r -= 4) {
        float alpha = 0.1f * (1.0f - (float)(r - 30) / 20.0f);
        drawCircleMidpoint(sunX, sunY, r, Color(1.0f, 0.95f, 0.5f, alpha), true);
    }
    drawCircleMidpoint(sunX, sunY, 28, Color(1.0f, 0.92f, 0.4f), true);
    
    // Animated sun rays
    for (int i = 0; i < 12; i++) {
        float angle = i * 0.524f + gameTime * 0.3f;
        float rayLength = 12 + sin(gameTime * 2.0f + i * 0.7f) * 4;
        drawLineDDA(
            sunX + cos(angle) * 32, sunY + sin(angle) * 32,
            sunX + cos(angle) * (32 + rayLength), sunY + sin(angle) * (32 + rayLength),
            Color(1.0f, 1.0f, 0.7f, 0.5f)
        );
    }
}

// ─────────────────────────────────────────
// Player Character
// ─────────────────────────────────────────

void Renderer::drawPlayer(const Player& player, float cameraX) {
    float screenX = player.x - cameraX;
    float drawY = player.y;
    
    // Apply squash and stretch
    float scaleX = player.facingRight ? player.squashScale : -player.squashScale;
    float scaleY = 2.0f - player.squashScale;
    float absScaleX = fabs(scaleX);
    
    // ── Body ──
    float bodyW = 12 * absScaleX;
    float bodyH = 14 * scaleY;
    std::vector<Point> bodyVerts = {
        Point(screenX - bodyW, drawY - bodyH),
        Point(screenX + bodyW, drawY - bodyH),
        Point(screenX + bodyW, drawY + bodyH),
        Point(screenX - bodyW, drawY + bodyH)
    };
    scanLineFill(bodyVerts, player.color, Color(1.0f, 0.6f, 0.6f), true);
    
    // Body outline
    Color outline(0.15f, 0.1f, 0.1f);
    drawLineBresenham(screenX - bodyW, drawY - bodyH, screenX + bodyW, drawY - bodyH, outline, 2);
    drawLineBresenham(screenX + bodyW, drawY - bodyH, screenX + bodyW, drawY + bodyH, outline, 2);
    drawLineBresenham(screenX + bodyW, drawY + bodyH, screenX - bodyW, drawY + bodyH, outline, 2);
    drawLineBresenham(screenX - bodyW, drawY + bodyH, screenX - bodyW, drawY - bodyH, outline, 2);
    
    // ── Head ──
    float headY = drawY + bodyH + 7 * scaleY;
    int headRadius = 8 * absScaleX;
    drawCircleMidpoint(screenX, headY, headRadius, Color(0.95f, 0.75f, 0.6f), true);
    drawCircleMidpoint(screenX, headY, headRadius, outline, false);
    
    // ── Eyes ──
    float eyeOffset = sin(player.animationTimer * 0.5f) * 0.5f;
    int eyeY = headY + 2 + eyeOffset;
    int eyeSpacing = 4;
    
    // White of eyes
    drawCircleMidpoint(screenX - eyeSpacing, eyeY, 3, Color(1.0f, 1.0f, 1.0f), true);
    drawCircleMidpoint(screenX + eyeSpacing, eyeY, 3, Color(1.0f, 1.0f, 1.0f), true);
    
    // Pupils (follow movement direction)
    int pupilOffsetX = player.vx > 0.5f ? 1 : (player.vx < -0.5f ? -1 : 0);
    drawCircleMidpoint(screenX - eyeSpacing + pupilOffsetX, eyeY, 1, Color(0.1f, 0.1f, 0.2f), true);
    drawCircleMidpoint(screenX + eyeSpacing + pupilOffsetX, eyeY, 1, Color(0.1f, 0.1f, 0.2f), true);
    
    // ── Legs ──
    float legPhase = player.animationTimer * 0.4f;
    float legSwing = (fabs(player.vx) > 0.5f && player.onGround) ? sin(legPhase) * 5 : 0;
    float legY = drawY - bodyH;
    
    // Left leg
    drawLineBresenham(screenX - 5, legY, screenX - 5 - legSwing, legY - 10 * scaleY, outline, 3);
    // Right leg
    drawLineBresenham(screenX + 5, legY, screenX + 5 + legSwing, legY - 10 * scaleY, outline, 3);
    
    // ── Arms ──
    float armY = drawY + 5 * scaleY;
    float armSwing = (fabs(player.vx) > 0.5f && player.onGround) ? sin(legPhase + 3.14f) * 4 : 0;
    
    if (player.wallSliding) {
        // Wall slide pose — arms reaching toward wall, legs tucked
        int wallSide = player.wallDirection;
        drawLineBresenham(screenX, armY, screenX + wallSide * 15, armY + 5, outline, 2);
        drawLineBresenham(screenX, armY - 3, screenX + wallSide * 12, armY + 2, outline, 2);
        // Wall slide dust particles (visual only)
        for (int i = 0; i < 3; i++) {
            float dustY = drawY - bodyH + i * 8;
            float dustAlpha = 0.3f - i * 0.1f;
            drawCircleMidpoint(screenX + wallSide * (bodyW + 3), dustY, 2, 
                             Color(0.7f, 0.6f, 0.5f, dustAlpha), true);
        }
    } else if (!player.onGround) {
        // Arms up when jumping
        drawLineBresenham(screenX - bodyW, armY, screenX - bodyW - 8, armY + 8, outline, 2);
        drawLineBresenham(screenX + bodyW, armY, screenX + bodyW + 8, armY + 8, outline, 2);
    } else {
        // Arms swing when running
        drawLineBresenham(screenX - bodyW, armY, screenX - bodyW - 6 - armSwing, armY - 6, outline, 2);
        drawLineBresenham(screenX + bodyW, armY, screenX + bodyW + 6 + armSwing, armY - 6, outline, 2);
    }
}

// ─────────────────────────────────────────
// Platforms
// ─────────────────────────────────────────

void Renderer::drawPlatforms(const std::vector<Platform>& platforms, float cameraX) {
    for (const auto& platform : platforms) {
        float screenX = platform.x - cameraX;
        
        float x1 = screenX, y1 = platform.y;
        float x2 = screenX + platform.width, y2 = platform.y + platform.height;
        
        if (cohenSutherlandClip(x1, y1, x2, y2, -50, 0, WINDOW_WIDTH + 50, WINDOW_HEIGHT)) {
            // Determine if this is a ground platform (taller) or floating
            bool isGround = platform.height > 20;
            
            // Platform body
            std::vector<Point> platformVertices = {
                Point(screenX, platform.y),
                Point(screenX + platform.width, platform.y),
                Point(screenX + platform.width, platform.y + platform.height),
                Point(screenX, platform.y + platform.height)
            };
            
            if (isGround) {
                // Ground: brown body with green grass top
                Color dirtColor(0.45f, 0.3f, 0.15f);
                Color dirtLight(0.55f, 0.38f, 0.2f);
                scanLineFill(platformVertices, dirtColor, dirtLight, true);
                
                // Green grass strip on top
                float grassH = 5;
                std::vector<Point> grassVerts = {
                    Point(screenX, platform.y + platform.height - grassH),
                    Point(screenX + platform.width, platform.y + platform.height - grassH),
                    Point(screenX + platform.width, platform.y + platform.height),
                    Point(screenX, platform.y + platform.height)
                };
                scanLineFill(grassVerts, Color(0.3f, 0.75f, 0.25f), Color(0.4f, 0.85f, 0.35f), true);
                
                // Small grass blades
                for (float gx = screenX + 5; gx < screenX + platform.width - 5; gx += 8) {
                    float bladeH = 4 + sin(gx * 0.3f + gameTime) * 2;
                    drawLineDDA(gx, platform.y + platform.height, gx + 2, platform.y + platform.height + bladeH,
                               Color(0.25f, 0.7f, 0.2f, 0.8f));
                }
            } else {
                // Floating platforms: stone/brick look
                Color stoneColor = platform.isMoving ? 
                    Color(0.5f, 0.4f, 0.65f) :   // Purple tint for moving
                    Color(0.55f, 0.45f, 0.35f);    // Brown/stone for static
                Color stoneLight(
                    fmin(1.0f, stoneColor.r + 0.15f),
                    fmin(1.0f, stoneColor.g + 0.15f),
                    fmin(1.0f, stoneColor.b + 0.15f)
                );
                
                scanLineFill(platformVertices, stoneColor, stoneLight, true);
                
                // Brick line pattern
                Color brickLine(stoneColor.r * 0.7f, stoneColor.g * 0.7f, stoneColor.b * 0.7f, 0.5f);
                float midY = platform.y + platform.height / 2;
                drawLineDDA(screenX, midY, screenX + platform.width, midY, brickLine);
                
                for (float bx = screenX + 15; bx < screenX + platform.width; bx += 25) {
                    drawLineDDA(bx, platform.y, bx, midY, brickLine);
                }
                for (float bx = screenX + 5; bx < screenX + platform.width; bx += 25) {
                    drawLineDDA(bx, midY, bx, platform.y + platform.height, brickLine);
                }
            }
            
            // Platform outline
            Color edgeColor(0.1f, 0.1f, 0.1f, 0.6f);
            drawLineDDA(screenX, platform.y, screenX + platform.width, platform.y, edgeColor);
            drawLineDDA(screenX + platform.width, platform.y, screenX + platform.width, platform.y + platform.height, edgeColor);
            drawLineDDA(screenX + platform.width, platform.y + platform.height, screenX, platform.y + platform.height, edgeColor);
            drawLineDDA(screenX, platform.y + platform.height, screenX, platform.y, edgeColor);
            
            // Moving platform glow indicator
            if (platform.isMoving) {
                float glowAlpha = 0.15f + 0.1f * sin(gameTime * 3);
                drawCircleMidpoint(screenX + platform.width / 2, platform.y + platform.height / 2, 
                                  3, Color(0.8f, 0.6f, 1.0f, glowAlpha), true);
            }
        }
    }
}

// ─────────────────────────────────────────
// Collectibles
// ─────────────────────────────────────────

void Renderer::drawCollectibles(const std::vector<Collectible>& collectibles, float cameraX) {
    for (const auto& coin : collectibles) {
        if (coin.collected) continue;
        
        float screenX = coin.x - cameraX;
        float drawY = coin.y + sin(coin.bobOffset) * 5;
        
        if (screenX > -50 && screenX < WINDOW_WIDTH + 50) {
            float scale = 1.0f + 0.08f * sin(coin.rotation * 2);
            int radius = 10 * scale;
            
            // Outer glow
            drawCircleMidpoint(screenX, drawY, radius + 3, Color(1.0f, 0.9f, 0.3f, 0.2f), true);
            
            // Coin body
            drawCircleMidpoint(screenX, drawY, radius, Color(1.0f, 0.85f, 0.0f), true);
            drawCircleMidpoint(screenX, drawY, radius - 2, Color(1.0f, 1.0f, 0.6f), true);
            
            // Inner detail
            drawCircleMidpoint(screenX, drawY, radius - 4, Color(1.0f, 0.9f, 0.2f), false);
            
            // Sparkle cross effect
            if ((int)(coin.rotation * 10) % 20 < 10) {
                float sparkleAlpha = 0.6f + 0.3f * sin(coin.rotation * 5);
                drawLineDDA(screenX - 6, drawY, screenX + 6, drawY, Color(1.0f, 1.0f, 1.0f, sparkleAlpha));
                drawLineDDA(screenX, drawY - 6, screenX, drawY + 6, Color(1.0f, 1.0f, 1.0f, sparkleAlpha));
            }
        }
    }
}

// ─────────────────────────────────────────
// Particles
// ─────────────────────────────────────────

void Renderer::drawParticles(const std::vector<Particle>& particles, float cameraX) {
    for (const auto& particle : particles) {
        if (particle.isDead()) continue;
        
        float screenX = particle.x - cameraX;
        if (screenX > -10 && screenX < WINDOW_WIDTH + 10) {
            glColor4f(particle.color.r, particle.color.g, particle.color.b, particle.color.a);
            float size = 2.0f + particle.color.a * 2.0f;
            glPointSize(size);
            glBegin(GL_POINTS);
            glVertex2f(screenX, particle.y);
            glEnd();
            glPointSize(1.0f);
        }
    }
}

// ─────────────────────────────────────────
// Enemies
// ─────────────────────────────────────────

void Renderer::drawEnemies(const std::vector<Enemy>& enemies, float cameraX) {
    for (const auto& enemy : enemies) {
        if (!enemy.alive) continue;
        
        float screenX = enemy.x - cameraX;
        if (screenX < -50 || screenX > WINDOW_WIDTH + 50) continue;
        
        float w = enemy.width / 2;
        float h = enemy.height / 2;
        float bounce = fabs(sin(enemy.animationTimer * 0.3f)) * 3;
        float drawY = enemy.y + bounce;
        
        // Enemy body
        std::vector<Point> bodyVerts = {
            Point(screenX - w, drawY - h),
            Point(screenX + w, drawY - h),
            Point(screenX + w, drawY + h),
            Point(screenX - w, drawY + h)
        };
        scanLineFill(bodyVerts, enemy.color, Color(1.0f, 0.4f, 0.4f), true);
        
        // Body outline
        Color outline(0.2f, 0.05f, 0.05f);
        drawLineBresenham(screenX - w, drawY - h, screenX + w, drawY - h, outline, 2);
        drawLineBresenham(screenX + w, drawY - h, screenX + w, drawY + h, outline, 2);
        drawLineBresenham(screenX + w, drawY + h, screenX - w, drawY + h, outline, 2);
        drawLineBresenham(screenX - w, drawY + h, screenX - w, drawY - h, outline, 2);
        
        // Angry eyes
        float eyeY = drawY + 4;
        drawCircleMidpoint(screenX - 5, eyeY, 3, Color(1.0f, 1.0f, 1.0f), true);
        drawCircleMidpoint(screenX + 5, eyeY, 3, Color(1.0f, 1.0f, 1.0f), true);
        
        // Pupils (look in movement direction)
        int pupilDir = enemy.facingRight ? 1 : -1;
        drawCircleMidpoint(screenX - 5 + pupilDir, eyeY, 1, Color(0.1f, 0.0f, 0.0f), true);
        drawCircleMidpoint(screenX + 5 + pupilDir, eyeY, 1, Color(0.1f, 0.0f, 0.0f), true);
        
        // Angry eyebrows
        if (enemy.facingRight) {
            drawLineBresenham(screenX - 8, eyeY + 5, screenX - 3, eyeY + 3, outline, 2);
            drawLineBresenham(screenX + 3, eyeY + 5, screenX + 8, eyeY + 3, outline, 2);
        } else {
            drawLineBresenham(screenX - 8, eyeY + 3, screenX - 3, eyeY + 5, outline, 2);
            drawLineBresenham(screenX + 3, eyeY + 3, screenX + 8, eyeY + 5, outline, 2);
        }
        
        // Small feet
        float legPhase = enemy.animationTimer * 0.4f;
        float legSwing = sin(legPhase) * 3;
        drawLineBresenham(screenX - 5, drawY - h, screenX - 5 - legSwing, drawY - h - 5, outline, 2);
        drawLineBresenham(screenX + 5, drawY - h, screenX + 5 + legSwing, drawY - h - 5, outline, 2);
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
    std::string jumpInfo = "Jumps: " + std::to_string(player.maxJumps - player.jumpCount) + "/" + std::to_string(player.maxJumps);
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
    drawTextCentered("P I X E L   H E R O", WINDOW_HEIGHT / 2 + 80 + titleBob, GLUT_BITMAP_HELVETICA_18);
    
    // Subtitle
    glColor4f(0.8f, 0.8f, 0.9f, 0.8f);
    drawTextCentered("A Classic Platformer Adventure", WINDOW_HEIGHT / 2 + 40, GLUT_BITMAP_HELVETICA_12);
    
    // Blinking "Press ENTER" text
    float blinkAlpha = 0.5f + 0.5f * sin(gameTime * 3);
    glColor4f(1.0f, 1.0f, 1.0f, blinkAlpha);
    drawTextCentered("Press ENTER or SPACE to Start", WINDOW_HEIGHT / 2 - 30, GLUT_BITMAP_HELVETICA_18);
    
    // Controls
    glColor4f(0.6f, 0.65f, 0.7f, 0.7f);
    drawTextCentered("A/D or Arrow Keys  -  Move", WINDOW_HEIGHT / 2 - 100, GLUT_BITMAP_HELVETICA_12);
    drawTextCentered("W / Space / Up  -  Jump (Double Jump!)", WINDOW_HEIGHT / 2 - 120, GLUT_BITMAP_HELVETICA_12);
    drawTextCentered("P  -  Pause    |    ESC  -  Quit", WINDOW_HEIGHT / 2 - 140, GLUT_BITMAP_HELVETICA_12);
    
    // Decorative coins on title screen
    for (int i = 0; i < 5; i++) {
        float coinX = 200 + i * 150;
        float coinY = WINDOW_HEIGHT / 2 - 200 + sin(gameTime * 2 + i) * 10;
        drawCoinIcon(coinX, coinY, 6);
    }
}

// ─────────────────────────────────────────
// Pause Overlay
// ─────────────────────────────────────────

void Renderer::drawPauseOverlay() {
    // Dark overlay
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    
    // Pause text
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
    // Red-tinted overlay
    glColor4f(0.3f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    
    // Game Over text
    float shake = sin(gameTime * 20) * 2;
    glColor3f(1.0f, 0.2f, 0.2f);
    drawTextCentered("G A M E   O V E R", WINDOW_HEIGHT / 2 + 60 + shake, GLUT_BITMAP_HELVETICA_18);
    
    // Stats
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextCentered("Score: " + std::to_string(score), WINDOW_HEIGHT / 2 + 10, GLUT_BITMAP_HELVETICA_18);
    
    int minutes = (int)timer / 60;
    int seconds = (int)timer % 60;
    char timerBuf[32];
    snprintf(timerBuf, sizeof(timerBuf), "Time: %d:%02d", minutes, seconds);
    drawTextCentered(std::string(timerBuf), WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_HELVETICA_12);
    
    // Restart prompt
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
    // Golden overlay
    glColor4f(0.2f, 0.15f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    
    // Celebration particles (small golden dots)
    for (int i = 0; i < 30; i++) {
        float px = fmod(i * 137.0f + gameTime * 30, (float)WINDOW_WIDTH);
        float py = fmod(i * 89.0f + gameTime * (20 + i), (float)WINDOW_HEIGHT);
        float sparkle = 0.5f + 0.5f * sin(gameTime * 5 + i);
        glColor4f(1.0f, 0.9f, 0.3f, sparkle);
        glPointSize(2.0f + sparkle);
        glBegin(GL_POINTS);
        glVertex2f(px, py);
        glEnd();
    }
    glPointSize(1.0f);
    
    // Win text
    float bob = sin(gameTime * 2) * 5;
    glColor3f(1.0f, 0.9f, 0.2f);
    drawTextCentered("Y O U   W I N !", WINDOW_HEIGHT / 2 + 80 + bob, GLUT_BITMAP_HELVETICA_18);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTextCentered("All coins collected!", WINDOW_HEIGHT / 2 + 40, GLUT_BITMAP_HELVETICA_12);
    
    // Stats
    drawTextCentered("Final Score: " + std::to_string(score), WINDOW_HEIGHT / 2, GLUT_BITMAP_HELVETICA_18);
    
    int minutes = (int)timer / 60;
    int seconds = (int)timer % 60;
    char timerBuf[32];
    snprintf(timerBuf, sizeof(timerBuf), "Time: %d:%02d", minutes, seconds);
    drawTextCentered(std::string(timerBuf), WINDOW_HEIGHT / 2 - 30, GLUT_BITMAP_HELVETICA_12);
    
    // Restart prompt
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