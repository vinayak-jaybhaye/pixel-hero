#ifndef RENDERER_H
#define RENDERER_H

#include "player.h"
#include "platform.h"
#include "collectible.h"
#include "particle.h"
#include "enemy.h"
#include "texture.h"
#include <vector>
#include <string>

class Renderer {
   private:
    float gameTime;

    // Helper methods
    void drawText(const std::string& text, float x, float y, void* font);
    void drawTextCentered(const std::string& text, float y, void* font);
    void drawHeart(float x, float y, float size, bool filled);
    void drawCoinIcon(float x, float y, float size);

   public:
    Renderer();

    // Load all sprite assets
    void loadAssets();

    // Game world rendering
    void drawBackground(float cameraX);
    void drawPlayer(const Player& player, float cameraX);
    void drawPlatforms(const std::vector<Platform>& platforms, float cameraX);
    void drawCollectibles(const std::vector<Collectible>& collectibles, float cameraX);
    void drawParticles(const std::vector<Particle>& particles, float cameraX);
    void drawEnemies(const std::vector<Enemy>& enemies, float cameraX);

    // UI rendering
    void drawHUD(int score, int lives, float timer, const Player& player);
    void drawMenuScreen();
    void drawPauseOverlay();
    void drawGameOverScreen(int score, float timer);
    void drawWinScreen(int score, float timer);
    void drawScreenFlash(float r, float g, float b, float alpha);

    void updateGameTime() { gameTime += 0.016f; }
};

#endif