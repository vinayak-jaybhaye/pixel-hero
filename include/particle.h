#ifndef PARTICLE_H
#define PARTICLE_H

#include "types.h"
#include <vector>

struct Particle {
    float x, y, vx, vy;
    Color color;
    float life, maxLife;
    
    Particle(float px, float py, float pvx, float pvy, Color c, float l);
    void update();
    bool isDead() const;
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    
public:
    void addParticle(float x, float y, float vx, float vy, Color color, float life);
    void createJumpParticles(float x, float y);
    void createLandingParticles(float x, float y);
    void createCollectionParticles(float x, float y);
    void update();
    void clear() { particles.clear(); }
    const std::vector<Particle>& getParticles() const { return particles; }
};

#endif