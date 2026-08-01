#include "renderer.h"
#include "graphics.h"
#include "constants.h"
#include <GL/glut.h>
#include <cmath>
#include <string>

Renderer::Renderer() : gameTime(0) {}

void Renderer::drawBackground(float cameraX) {
    // Draw animated clouds
    float cloudPositions[][3] = {
        {150, 550, 0.3f}, {400, 520, 0.4f}, {700, 580, 0.25f}, 
        {950, 540, 0.35f}, {1200, 560, 0.3f}
    };
    
    for (int i = 0; i < 5; i++) {
        float parallaxX = cloudPositions[i][0] - cameraX * cloudPositions[i][2] + sin(gameTime + i) * 10;
        float cloudY = cloudPositions[i][1] + cos(gameTime * 0.5f + i) * 3;
        
        if (parallaxX > -100 && parallaxX < WINDOW_WIDTH + 100) {
            drawCircleMidpoint(parallaxX, cloudY, 20, Color(1.0f, 1.0f, 1.0f, 0.8f), true);
            drawCircleMidpoint(parallaxX + 25, cloudY, 18, Color(1.0f, 1.0f, 1.0f, 0.8f), true);
            drawCircleMidpoint(parallaxX - 20, cloudY, 15, Color(1.0f, 1.0f, 1.0f, 0.8f), true);
        }
    }
    
    // Draw sun
    float sunX = 800 - cameraX * 0.1f;
    float sunY = 600 + sin(gameTime) * 5;
    drawCircleBresenham(sunX, sunY, 30, Color(1.0f, 0.9f, 0.3f), true);
    
    // Sun rays
    for (int i = 0; i < 8; i++) {
        float angle = i * 0.785f + gameTime;
        float rayLength = 15 + sin(gameTime * 2 + i) * 3;
        drawLineDDA(
            sunX + cos(angle) * 35, sunY + sin(angle) * 35,
            sunX + cos(angle) * (35 + rayLength), sunY + sin(angle) * (35 + rayLength),
            Color(1.0f, 1.0f, 0.7f, 0.6f)
        );
    }
    
    // Draw mountains
    std::vector<Point> mountain1 = {
        Point(-100 - cameraX * 0.2f, 100),
        Point(200 - cameraX * 0.2f, 300),
        Point(400 - cameraX * 0.2f, 200),
        Point(600 - cameraX * 0.2f, 100)
    };
    scanLineFill(mountain1, Color(0.4f, 0.3f, 0.6f, 0.7f));
}

void Renderer::drawPlayer(const Player& player, float cameraX) {
    float screenX = player.x - cameraX;
    float drawY = player.y;
    
    // Apply squash and stretch
    float scaleX = player.facingRight ? player.squashScale : -player.squashScale;
    float scaleY = 2.0f - player.squashScale;
    
    // Draw player body
    std::vector<Point> playerVertices = {
        Point(screenX - 12 * abs(scaleX), drawY - 18 * scaleY),
        Point(screenX + 12 * abs(scaleX), drawY - 18 * scaleY),
        Point(screenX + 12 * abs(scaleX), drawY + 18 * scaleY),
        Point(screenX - 12 * abs(scaleX), drawY + 18 * scaleY)
    };
    
    // Draw outline
    drawLineBresenham(screenX - 12 * abs(scaleX), drawY - 18 * scaleY, 
                     screenX + 12 * abs(scaleX), drawY - 18 * scaleY, Color(0.2f, 0.1f, 0.1f), 2);

    scanLineFill(playerVertices, player.color, Color(1.0f, 0.6f, 0.6f), true);
    
    
    // Draw eyes
    float eyeOffset = sin(player.animationTimer) * 0.5f;
    int eyeY = drawY + 8 * scaleY + eyeOffset;
    
    drawCircleMidpoint(screenX - 6, eyeY, 3, Color(1.0f, 1.0f, 1.0f), true);
    drawCircleMidpoint(screenX + 6, eyeY, 3, Color(1.0f, 1.0f, 1.0f), true);
    
    // Pupils
    int pupilOffsetX = player.vx > 0 ? 1 : (player.vx < 0 ? -1 : 0);
    drawCircleMidpoint(screenX - 6 + pupilOffsetX, eyeY, 1, Color(0.0f, 0.0f, 0.0f), true);
    drawCircleMidpoint(screenX + 6 + pupilOffsetX, eyeY, 1, Color(0.0f, 0.0f, 0.0f), true);
}

void Renderer::drawPlatforms(const std::vector<Platform>& platforms, float cameraX) {
    for (const auto& platform : platforms) {
        float screenX = platform.x - cameraX;
        
        float x1 = screenX, y1 = platform.y;
        float x2 = screenX + platform.width, y2 = platform.y + platform.height;
        
        if (cohenSutherlandClip(x1, y1, x2, y2, -50, 0, WINDOW_WIDTH + 50, WINDOW_HEIGHT)) {
            std::vector<Point> platformVertices = {
                Point(screenX, platform.y),
                Point(screenX + platform.width, platform.y),
                Point(screenX + platform.width, platform.y + platform.height),
                Point(screenX, platform.y + platform.height)
            };
            
            Color lightColor(
                std::min(1.0f, platform.color.r + 0.3f),
                std::min(1.0f, platform.color.g + 0.3f),
                std::min(1.0f, platform.color.b + 0.3f)
            );

            // Platform outline
            Color edgeColor(0.1f, 0.1f, 0.1f);
            drawLineDDA(screenX, platform.y, screenX + platform.width, platform.y, edgeColor);                        // top
            drawLineDDA(screenX + platform.width, platform.y, screenX + platform.width, platform.y + platform.height, edgeColor); // right
            drawLineDDA(screenX + platform.width, platform.y + platform.height, screenX, platform.y + platform.height, edgeColor); // bottom
            drawLineDDA(screenX, platform.y + platform.height, screenX, platform.y, edgeColor);                       // left
            
            // Then fill the platform
            scanLineFill(platformVertices, platform.color, lightColor, true);
        }
    }
}


void Renderer::drawCollectibles(const std::vector<Collectible>& collectibles, float cameraX) {
    for (const auto& coin : collectibles) {
        if (coin.collected) continue;
        
        float screenX = coin.x - cameraX;
        float drawY = coin.y + sin(coin.bobOffset) * 5;
        
        if (screenX > -50 && screenX < WINDOW_WIDTH + 50) {
            float scale = 1.0f + 0.1f * sin(coin.rotation * 2);
            int radius = 12 * scale;
            
            drawCircleMidpoint(screenX, drawY, radius, coin.color, true);
            drawCircleMidpoint(screenX, drawY, radius - 2, Color(1.0f, 1.0f, 0.8f), true);
            
            // Sparkle effect
            if ((int)(coin.rotation * 10) % 20 < 10) {
                drawLineDDA(screenX - 8, drawY, screenX + 8, drawY, Color(1.0f, 1.0f, 1.0f, 0.8f));
                drawLineDDA(screenX, drawY - 8, screenX, drawY + 8, Color(1.0f, 1.0f, 1.0f, 0.8f));
            }
        }
    }
}

void Renderer::drawParticles(const std::vector<Particle>& particles, float cameraX) {
    for (const auto& particle : particles) {
        if (particle.isDead()) continue;
        
        float screenX = particle.x - cameraX;
        if (screenX > -10 && screenX < WINDOW_WIDTH + 10) {
            glColor4f(particle.color.r, particle.color.g, particle.color.b, particle.color.a);
            glPointSize(3.0f);
            glBegin(GL_POINTS);
            glVertex2f(screenX, particle.y);
            glEnd();
            glPointSize(1.0f);
        }
    }
}

void Renderer::drawUI(int score, const Player& player) {
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glRasterPos2f(20, WINDOW_HEIGHT - 25);
    std::string scoreText = "Score: " + std::to_string(score);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    glRasterPos2f(10, WINDOW_HEIGHT - 50);
    std::string jumpInfo = "Jumps: " + std::to_string(player.maxJumps - player.jumpCount) + "/" + std::to_string(player.maxJumps);
    for (char c : jumpInfo) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}