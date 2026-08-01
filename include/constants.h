#ifndef CONSTANTS_H
#define CONSTANTS_H

// Window dimensions
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 700;

// physics constants
const float GRAVITY = 0.6f;
const float JUMP_VELOCITY = 16.0f;
const float PLAYER_SPEED = 4.5f;
const float FRICTION = 0.85f;
const float AIR_RESISTANCE = 0.98f;
const float GROUND_FRICTION = 0.8f;
const float ACCELERATION = 0.8f;
const float MAX_FALL_SPEED = -20.0f;

// Animation constants
const float COIN_ROTATION_SPEED = 0.08f;
const float CLOUD_DRIFT_SPEED = 0.02f;
const float PARTICLE_LIFE = 60.0f;

// Clipping constants
const int INSIDE        = 0;
const int LEFT          = 1;
const int RIGHT         = 2;
const int BOTTOM        = 4;
const int TOP           = 8;

const int BOTTOM_LEFT   = BOTTOM | LEFT;   // 5
const int BOTTOM_RIGHT  = BOTTOM | RIGHT;  // 6
const int TOP_LEFT      = TOP | LEFT;      // 9
const int TOP_RIGHT     = TOP | RIGHT;     // 10


#endif