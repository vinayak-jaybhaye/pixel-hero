#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "platform.h"
#include "collectible.h"
#include "particle.h"
#include "renderer.h"
#include "enemy.h"
#include <vector>

// Game states
enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, WIN };

class Game {
   private:
    Player player;
    std::vector<Platform> platforms;
    std::vector<Collectible> collectibles;
    std::vector<Enemy> enemies;
    ParticleSystem particleSystem;
    Renderer renderer;

    float cameraX;
    float cameraTargetX;
    int score;
    int lives;
    float gameTimer;
    int totalCoins;
    bool keys[256];
    GameState state;
    float stateTransitionTimer;
    float damageFlashTimer;
    float cameraShakeTimer;
    float cameraShakeIntensity;

    bool checkCollision(float x, float y, float width, float height, const Platform& platform);
    void checkCollectibleCollection();
    void checkEnemyCollisions();
    void updateCamera();
    void resetLevel();
    void playerTakeDamage();

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
    GameState getState() const { return state; }
};

#endif