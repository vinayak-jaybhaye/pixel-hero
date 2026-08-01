#ifndef RENDERER_H
#define RENDERER_H

#include "player.h"
#include "platform.h"
#include "collectible.h"
#include "particle.h"
#include <vector>

class Renderer {
private:
    float gameTime;
    
public:
    Renderer();
    
    void drawBackground(float cameraX);
    void drawPlayer(const Player& player, float cameraX);
    void drawPlatforms(const std::vector<Platform>& platforms, float cameraX);
    void drawCollectibles(const std::vector<Collectible>& collectibles, float cameraX);
    void drawParticles(const std::vector<Particle>& particles, float cameraX);
    void drawUI(int score, const Player& player);
    
    void updateGameTime() { gameTime += 0.01f; }
};

#endif