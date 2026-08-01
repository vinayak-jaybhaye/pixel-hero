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
    
    // Section 1: Starting area — easy pickups
    collectibles.push_back(Collectible(300, 220));
    collectibles.push_back(Collectible(500, 290));
    collectibles.push_back(Collectible(250, 360));
    collectibles.push_back(Collectible(150, 490));
    collectibles.push_back(Collectible(450, 540));
    
    // Section 2: Gap challenge — reward exploration
    collectibles.push_back(Collectible(900, 240));
    collectibles.push_back(Collectible(1050, 360));
    collectibles.push_back(Collectible(650, 420));
    collectibles.push_back(Collectible(750, 490));
    collectibles.push_back(Collectible(1300, 190));
    collectibles.push_back(Collectible(1400, 260));
    
    // Section 3: Vertical climb — coins along the climb path
    collectibles.push_back(Collectible(1550, 220));
    collectibles.push_back(Collectible(1400, 320));
    collectibles.push_back(Collectible(1600, 400));
    collectibles.push_back(Collectible(1450, 480));
    collectibles.push_back(Collectible(1650, 560));
    collectibles.push_back(Collectible(1850, 240));
    
    // Section 4: Danger zone — risky coins
    collectibles.push_back(Collectible(2150, 220));
    collectibles.push_back(Collectible(2270, 290));
    collectibles.push_back(Collectible(2390, 220));
    collectibles.push_back(Collectible(2500, 340));
    collectibles.push_back(Collectible(2350, 440));
    
    // Section 5: Final stretch
    collectibles.push_back(Collectible(2800, 220));
    collectibles.push_back(Collectible(2950, 320));
    collectibles.push_back(Collectible(3100, 220));
    collectibles.push_back(Collectible(2900, 440));
    collectibles.push_back(Collectible(3050, 520));
}