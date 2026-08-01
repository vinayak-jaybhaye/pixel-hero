#include "enemy.h"
#include <cmath>

Enemy::Enemy(float px, float py, float left, float right, EnemyType t)
    : x(px),
      y(py),
      vx(1.5f),
      width(20),
      height(20),
      patrolLeft(left),
      patrolRight(right),
      color(0.9f, 0.2f, 0.2f),
      alive(true),
      facingRight(true),
      animationTimer(0),
      type(t) {
    if (t == EnemyType::STATIC) {
        vx = 0;
        color = Color(0.6f, 0.6f, 0.6f);
    }
}

void Enemy::update() {
    if (!alive) return;

    animationTimer += 0.1f;

    if (type == EnemyType::PATROL) {
        x += vx;

        // Reverse at patrol bounds
        if (x >= patrolRight) {
            x = patrolRight;
            vx = -fabs(vx);
            facingRight = false;
        } else if (x <= patrolLeft) {
            x = patrolLeft;
            vx = fabs(vx);
            facingRight = true;
        }
    }
}

void Enemy::kill() {
    alive = false;
}

void initializeEnemies(std::vector<Enemy>& enemies) {
    enemies.clear();

    // Enemies on ground platforms
    enemies.push_back(Enemy(200, 93, 50, 350));    // On first ground platform
    enemies.push_back(Enemy(550, 93, 500, 700));   // On second ground platform
    enemies.push_back(Enemy(950, 93, 850, 1150));  // On third ground platform

    // Enemies on floating platforms
    enemies.push_back(Enemy(280, 216, 260, 350));  // On first floating platform
    enemies.push_back(Enemy(220, 356, 210, 280));  // On the higher floating platform

    // Enemies further in the level
    enemies.push_back(Enemy(1400, 93, 1300, 1550));
    enemies.push_back(Enemy(1800, 218, 1750, 1950));
    enemies.push_back(Enemy(2200, 93, 2100, 2400));
}
