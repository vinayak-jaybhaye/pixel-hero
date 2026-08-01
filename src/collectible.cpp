#include "collectible.h"

Collectible::Collectible(float px, float py) 
    : x(px), y(py), rotation(0), bobOffset(0), collected(false), 
      color(1.0f, 0.9f, 0.0f) {}

void Collectible::update() {
    rotation += COIN_ROTATION_SPEED;
    bobOffset += 0.1f;
}

void initializeCollectibles(std::vector<Collectible>& collectibles) {
    collectibles.clear();
    
    collectibles.push_back(Collectible(300, 220));
    collectibles.push_back(Collectible(500, 290));
    collectibles.push_back(Collectible(250, 360));
    collectibles.push_back(Collectible(650, 420));
    collectibles.push_back(Collectible(150, 490));
    collectibles.push_back(Collectible(900, 240));
    collectibles.push_back(Collectible(1050, 360));
    collectibles.push_back(Collectible(450, 540));
    collectibles.push_back(Collectible(750, 490));
}