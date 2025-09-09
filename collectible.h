#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "types.h"
#include "constants.h"
#include <vector>

class Collectible {
public:
    float x, y;
    float rotation;
    float bobOffset;
    bool collected;
    Color color;
    
    Collectible(float px, float py);
    void update();
};

void initializeCollectibles(std::vector<Collectible>& collectibles);

#endif