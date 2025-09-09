#include "particle.h"
#include <cmath>
#include <cstdlib>

Particle::Particle(float px, float py, float pvx, float pvy, Color c, float l)
    : x(px), y(py), vx(pvx), vy(pvy), color(c), life(l), maxLife(l) {}

void Particle::update() {
    x += vx;
    y += vy;
    vy -= 0.2f; // gravity on particles
    vx *= 0.99f; // air resistance
    life--;
    
    // Fade out
    color.a = life / maxLife;
}

bool Particle::isDead() const { 
    return life <= 0; 
}

void ParticleSystem::addParticle(float x, float y, float vx, float vy, Color color, float life) {
    particles.push_back(Particle(x, y, vx, vy, color, life));
}

void ParticleSystem::createJumpParticles(float x, float y) {
    for (int i = 0; i < 8; i++) {
        float angle = i * 0.785f;
        float speed = 2.0f + (rand() % 100) / 50.0f;
        addParticle(
            x + (rand() % 20 - 10), 
            y - 15,
            cos(angle) * speed, 
            sin(angle) * speed,
            Color(0.8f, 0.8f, 1.0f, 0.8f),
            30 + rand() % 20
        );
    }
}

void ParticleSystem::createLandingParticles(float x, float y) {
    for (int i = 0; i < 12; i++) {
        float vx = (rand() % 200 - 100) / 20.0f;
        float vy = (rand() % 100) / 10.0f;
        addParticle(
            x + (rand() % 40 - 20), 
            y - 15,
            vx, vy,
            Color(0.6f, 0.4f, 0.2f, 0.9f),
            40 + rand() % 30
        );
    }
}

void ParticleSystem::createCollectionParticles(float x, float y) {
    for (int i = 0; i < 15; i++) {
        float angle = i * 0.419f;
        float speed = 3.0f + (rand() % 100) / 50.0f;
        addParticle(
            x, y,
            cos(angle) * speed, sin(angle) * speed,
            Color(1.0f, 1.0f, 0.0f, 1.0f),
            50 + rand() % 30
        );
    }
}

void ParticleSystem::update() {
    for (auto it = particles.begin(); it != particles.end();) {
        it->update();
        if (it->isDead()) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}