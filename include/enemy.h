#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"
#include <vector>

enum class EnemyType {
    PATROL,     // Walks back and forth on a platform
    STATIC      // Stays in place (like a spike hazard)
};

class Enemy {
public:
    float x, y;
    float vx;
    float width, height;
    float patrolLeft, patrolRight;
    Color color;
    bool alive;
    bool facingRight;
    float animationTimer;
    EnemyType type;
    
    Enemy(float px, float py, float left, float right, EnemyType t = EnemyType::PATROL);
    void update();
    void kill();
};

void initializeEnemies(std::vector<Enemy>& enemies);

#endif
