#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "platform.h"
#include "collectible.h"
#include "particle.h"
#include "renderer.h"
#include <vector>

class Game {
private:
    Player player;
    std::vector<Platform> platforms;
    std::vector<Collectible> collectibles;
    ParticleSystem particleSystem;
    Renderer renderer;
    
    float cameraX;
    float cameraTargetX;
    int score;
    bool keys[256];
    
    bool checkCollision(float x, float y, float width, float height, const Platform& platform);
    void checkCollectibleCollection();
    void updateCamera();
    
public:
    Game();
    
    void init();
    void update();
    void render();
    void handleKeyDown(unsigned char key);
    void handleKeyUp(unsigned char key);
    void handleSpecialDown(int key);
    void handleSpecialUp(int key);
    void processInput();
    
    Player& getPlayer() { return player; }
    int getScore() const { return score; }
};

#endif