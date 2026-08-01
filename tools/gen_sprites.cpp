// Generates pixel art sprites as PNG files for the game
// Build: g++ -o gen_sprites tools/gen_sprites.cpp -I vendor -lm
// Run: ./gen_sprites

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <cstring>
#include <cstdio>
#include <cmath>

// Write a single pixel (RGBA)
void setPixel(unsigned char* img, int width, int x, int y, 
              unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
    if (x < 0 || y < 0) return;
    int idx = (y * width + x) * 4;
    img[idx] = r; img[idx+1] = g; img[idx+2] = b; img[idx+3] = a;
}

void fillRect(unsigned char* img, int width, int x, int y, int w, int h,
              unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
    for (int dy = 0; dy < h; dy++)
        for (int dx = 0; dx < w; dx++)
            setPixel(img, width, x + dx, y + dy, r, g, b, a);
}

// ─────────────────────────────────────────
// Player Sprite Sheet: 4 frames, each 32x32, total 128x32
// Frame 0: Idle
// Frame 1: Run1
// Frame 2: Run2
// Frame 3: Jump
// ─────────────────────────────────────────
void generatePlayerSprites() {
    const int FW = 32, FH = 32, FRAMES = 4;
    const int W = FW * FRAMES, H = FH;
    unsigned char* img = new unsigned char[W * H * 4]();  // All transparent
    
    for (int f = 0; f < FRAMES; f++) {
        int ox = f * FW;  // Frame X offset
        
        // ── Boots ──
        fillRect(img, W, ox+10, 26, 5, 4, 60, 40, 30);  // Left boot
        fillRect(img, W, ox+17, 26, 5, 4, 60, 40, 30);  // Right boot
        
        if (f == 1) {  // Run frame 1: legs apart
            fillRect(img, W, ox+8, 26, 5, 4, 60, 40, 30);
            fillRect(img, W, ox+19, 26, 5, 4, 60, 40, 30);
        } else if (f == 2) {  // Run frame 2: legs together
            fillRect(img, W, ox+12, 26, 8, 4, 60, 40, 30);
        } else if (f == 3) {  // Jump: legs tucked
            fillRect(img, W, ox+10, 24, 5, 4, 60, 40, 30);
            fillRect(img, W, ox+17, 24, 5, 4, 60, 40, 30);
        }
        
        // ── Legs ──
        fillRect(img, W, ox+11, 22, 4, 4, 70, 90, 150);  // Left leg (blue pants)
        fillRect(img, W, ox+17, 22, 4, 4, 70, 90, 150);  // Right leg
        
        if (f == 1) {
            fillRect(img, W, ox+9, 22, 4, 4, 70, 90, 150);
            fillRect(img, W, ox+19, 22, 4, 4, 70, 90, 150);
        } else if (f == 3) {
            fillRect(img, W, ox+11, 20, 4, 4, 70, 90, 150);
            fillRect(img, W, ox+17, 20, 4, 4, 70, 90, 150);
        }
        
        // ── Body (shirt) ──
        fillRect(img, W, ox+10, 12, 12, 10, 200, 70, 70);  // Red shirt
        // Shirt highlight
        fillRect(img, W, ox+11, 13, 3, 3, 230, 100, 100);
        
        // ── Arms ──
        if (f == 0) {  // Idle: arms down
            fillRect(img, W, ox+7, 14, 3, 7, 230, 180, 150);   // Left arm (skin)
            fillRect(img, W, ox+22, 14, 3, 7, 230, 180, 150);  // Right arm
        } else if (f == 1) {  // Run1: arms swinging
            fillRect(img, W, ox+7, 12, 3, 6, 230, 180, 150);
            fillRect(img, W, ox+22, 16, 3, 6, 230, 180, 150);
        } else if (f == 2) {  // Run2: arms other way
            fillRect(img, W, ox+7, 16, 3, 6, 230, 180, 150);
            fillRect(img, W, ox+22, 12, 3, 6, 230, 180, 150);
        } else {  // Jump: arms up
            fillRect(img, W, ox+6, 8, 3, 6, 230, 180, 150);
            fillRect(img, W, ox+23, 8, 3, 6, 230, 180, 150);
        }
        
        // ── Head ──
        fillRect(img, W, ox+11, 4, 10, 8, 240, 190, 150);  // Face (skin tone)
        
        // ── Hair ──
        fillRect(img, W, ox+10, 2, 12, 4, 80, 50, 30);   // Hair top
        fillRect(img, W, ox+10, 4, 2, 3, 80, 50, 30);    // Hair side left
        fillRect(img, W, ox+20, 4, 2, 3, 80, 50, 30);    // Hair side right
        
        // ── Eyes ──
        setPixel(img, W, ox+14, 7, 255, 255, 255);  // Left eye white
        setPixel(img, W, ox+15, 7, 30, 30, 50);     // Left pupil
        setPixel(img, W, ox+18, 7, 255, 255, 255);  // Right eye white
        setPixel(img, W, ox+19, 7, 30, 30, 50);     // Right pupil
        
        // ── Mouth ──
        if (f == 3) {  // Open mouth when jumping
            setPixel(img, W, ox+16, 10, 150, 50, 50);
            setPixel(img, W, ox+17, 10, 150, 50, 50);
        } else {
            setPixel(img, W, ox+16, 10, 180, 100, 80);  // Smile
            setPixel(img, W, ox+17, 10, 180, 100, 80);
        }
    }
    
    stbi_write_png("assets/sprites/player.png", W, H, 4, img, W * 4);
    printf("  Generated: assets/sprites/player.png (%dx%d, %d frames)\n", W, H, FRAMES);
    delete[] img;
}

// ─────────────────────────────────────────
// Coin Sprite Sheet: 6 frames, each 16x16, total 96x16
// ─────────────────────────────────────────
void generateCoinSprites() {
    const int FW = 16, FH = 16, FRAMES = 6;
    const int W = FW * FRAMES, H = FH;
    unsigned char* img = new unsigned char[W * H * 4]();
    
    for (int f = 0; f < FRAMES; f++) {
        int ox = f * FW;
        // Simulate rotation by varying width
        float phase = f * 3.14159f / FRAMES;
        int halfW = (int)(6 * fabs(cos(phase)));
        if (halfW < 1) halfW = 1;
        
        int cx = ox + 8, cy = 8;
        
        // Coin body (ellipse via width variation)
        for (int dy = -6; dy <= 6; dy++) {
            int rowW = (int)(halfW * sqrt(1.0 - (dy * dy) / 36.0));
            if (rowW < 1) rowW = 1;
            for (int dx = -rowW; dx <= rowW; dx++) {
                // Gradient from gold to light yellow
                float t = (float)(dx + rowW) / (2 * rowW + 1);
                unsigned char r = 255;
                unsigned char g = (unsigned char)(200 + t * 55);
                unsigned char b = (unsigned char)(30 + t * 100);
                setPixel(img, W, cx + dx, cy + dy, r, g, b);
            }
        }
        
        // $ symbol in center (when wide enough)
        if (halfW >= 3) {
            setPixel(img, W, cx, cy - 3, 180, 140, 20);
            setPixel(img, W, cx, cy - 2, 180, 140, 20);
            setPixel(img, W, cx - 1, cy - 2, 180, 140, 20);
            setPixel(img, W, cx - 1, cy - 1, 180, 140, 20);
            setPixel(img, W, cx, cy, 180, 140, 20);
            setPixel(img, W, cx + 1, cy + 1, 180, 140, 20);
            setPixel(img, W, cx + 1, cy + 2, 180, 140, 20);
            setPixel(img, W, cx - 1, cy + 2, 180, 140, 20);
            setPixel(img, W, cx, cy + 3, 180, 140, 20);
        }
        
        // Shine highlight
        if (halfW >= 2) {
            setPixel(img, W, cx - halfW + 1, cy - 3, 255, 255, 220);
            setPixel(img, W, cx - halfW + 1, cy - 2, 255, 255, 220);
        }
    }
    
    stbi_write_png("assets/sprites/coin.png", W, H, 4, img, W * 4);
    printf("  Generated: assets/sprites/coin.png (%dx%d, %d frames)\n", W, H, FRAMES);
    delete[] img;
}

// ─────────────────────────────────────────
// Enemy Sprite Sheet: 2 frames, each 24x24, total 48x24
// ─────────────────────────────────────────
void generateEnemySprites() {
    const int FW = 24, FH = 24, FRAMES = 2;
    const int W = FW * FRAMES, H = FH;
    unsigned char* img = new unsigned char[W * H * 4]();
    
    for (int f = 0; f < FRAMES; f++) {
        int ox = f * FW;
        
        // ── Body (round-ish blob) ──
        for (int dy = -8; dy <= 8; dy++) {
            int rowW = (int)(10 * sqrt(1.0 - (dy * dy) / 64.0));
            for (int dx = -rowW; dx <= rowW; dx++) {
                float t = (float)(dy + 8) / 16.0f;
                unsigned char r = (unsigned char)(180 + t * 40);
                unsigned char g = (unsigned char)(40 + t * 20);
                unsigned char b = (unsigned char)(40 + t * 20);
                setPixel(img, W, ox + 12 + dx, 12 + dy, r, g, b);
            }
        }
        
        // ── Eyes ──
        // Left eye
        fillRect(img, W, ox+7, 8, 4, 4, 255, 255, 255);
        setPixel(img, W, ox+8, 9, 20, 20, 20);
        setPixel(img, W, ox+9, 9, 20, 20, 20);
        
        // Right eye
        fillRect(img, W, ox+13, 8, 4, 4, 255, 255, 255);
        setPixel(img, W, ox+14, 9, 20, 20, 20);
        setPixel(img, W, ox+15, 9, 20, 20, 20);
        
        // ── Angry eyebrows ──
        setPixel(img, W, ox+7, 7, 80, 20, 20);
        setPixel(img, W, ox+8, 6, 80, 20, 20);
        setPixel(img, W, ox+9, 7, 80, 20, 20);
        setPixel(img, W, ox+14, 7, 80, 20, 20);
        setPixel(img, W, ox+15, 6, 80, 20, 20);
        setPixel(img, W, ox+16, 7, 80, 20, 20);
        
        // ── Mouth (angry) ──
        for (int i = 0; i < 6; i++) {
            setPixel(img, W, ox + 9 + i, 15, 100, 20, 20);
        }
        setPixel(img, W, ox+9, 14, 100, 20, 20);   // Fang
        setPixel(img, W, ox+14, 14, 100, 20, 20);   // Fang
        
        // ── Feet ──
        if (f == 0) {
            fillRect(img, W, ox+6, 20, 4, 3, 120, 30, 30);
            fillRect(img, W, ox+14, 20, 4, 3, 120, 30, 30);
        } else {
            fillRect(img, W, ox+4, 20, 4, 3, 120, 30, 30);
            fillRect(img, W, ox+16, 20, 4, 3, 120, 30, 30);
        }
        
        // ── Small horns ──
        setPixel(img, W, ox+7, 3, 140, 50, 50);
        setPixel(img, W, ox+8, 2, 140, 50, 50);
        setPixel(img, W, ox+16, 3, 140, 50, 50);
        setPixel(img, W, ox+15, 2, 140, 50, 50);
    }
    
    stbi_write_png("assets/sprites/enemy.png", W, H, 4, img, W * 4);
    printf("  Generated: assets/sprites/enemy.png (%dx%d, %d frames)\n", W, H, FRAMES);
    delete[] img;
}

// ─────────────────────────────────────────
// Platform Tiles: grass_top (32x32), dirt (32x32), stone (32x32)
// ─────────────────────────────────────────
void generatePlatformTiles() {
    const int S = 32;
    
    // ── Grass Top Tile ──
    {
        unsigned char* img = new unsigned char[S * S * 4]();
        // Dirt body
        for (int y = 8; y < S; y++) {
            for (int x = 0; x < S; x++) {
                int noise = ((x * 7 + y * 13) % 20) - 10;
                unsigned char r = 140 + noise;
                unsigned char g = 95 + noise;
                unsigned char b = 55 + noise / 2;
                setPixel(img, S, x, y, r, g, b);
            }
        }
        // Green grass top
        for (int y = 4; y < 10; y++) {
            for (int x = 0; x < S; x++) {
                int noise = ((x * 11 + y * 7) % 15) - 7;
                unsigned char r = 60 + noise;
                unsigned char g = 160 + noise * 2;
                unsigned char b = 50 + noise;
                setPixel(img, S, x, y, r, g, b);
            }
        }
        // Grass blades on top edge
        for (int x = 0; x < S; x += 3) {
            int h = 2 + (x * 7) % 3;
            for (int dy = 0; dy < h; dy++) {
                setPixel(img, S, x, 4 - dy, 50, 140 + (dy * 10), 40);
            }
        }
        // Dark bottom edge
        for (int x = 0; x < S; x++) {
            setPixel(img, S, x, S - 1, 80, 55, 30);
        }
        
        stbi_write_png("assets/sprites/tile_grass.png", S, S, 4, img, S * 4);
        printf("  Generated: assets/sprites/tile_grass.png (%dx%d)\n", S, S);
        delete[] img;
    }
    
    // ── Stone Tile ──
    {
        unsigned char* img = new unsigned char[S * S * 4]();
        for (int y = 0; y < S; y++) {
            for (int x = 0; x < S; x++) {
                int noise = ((x * 13 + y * 17) % 30) - 15;
                unsigned char r = 130 + noise;
                unsigned char g = 120 + noise;
                unsigned char b = 110 + noise;
                setPixel(img, S, x, y, r, g, b);
            }
        }
        // Brick lines
        for (int x = 0; x < S; x++) {
            setPixel(img, S, x, S / 2, 90, 80, 75);
            setPixel(img, S, x, 0, 100, 90, 85);
            setPixel(img, S, x, S - 1, 90, 80, 75);
        }
        for (int y = 0; y < S / 2; y++) {
            setPixel(img, S, S / 2, y, 90, 80, 75);
        }
        for (int y = S / 2; y < S; y++) {
            setPixel(img, S, S / 4, y, 90, 80, 75);
            setPixel(img, S, 3 * S / 4, y, 90, 80, 75);
        }
        // Highlight on top edges
        for (int x = 1; x < S - 1; x++) {
            setPixel(img, S, x, 1, 160, 150, 140);
        }
        
        stbi_write_png("assets/sprites/tile_stone.png", S, S, 4, img, S * 4);
        printf("  Generated: assets/sprites/tile_stone.png (%dx%d)\n", S, S);
        delete[] img;
    }
    
    // ── Moving Platform Tile (purple tint) ──
    {
        unsigned char* img = new unsigned char[S * S * 4]();
        for (int y = 0; y < S; y++) {
            for (int x = 0; x < S; x++) {
                int noise = ((x * 11 + y * 19) % 25) - 12;
                unsigned char r = 120 + noise;
                unsigned char g = 90 + noise;
                unsigned char b = 160 + noise;
                setPixel(img, S, x, y, r, g, b);
            }
        }
        // Glowing edge
        for (int x = 0; x < S; x++) {
            setPixel(img, S, x, 0, 180, 150, 220);
            setPixel(img, S, x, S - 1, 80, 60, 110);
        }
        for (int y = 0; y < S; y++) {
            setPixel(img, S, 0, y, 100, 75, 140);
            setPixel(img, S, S - 1, y, 100, 75, 140);
        }
        // Center glow dot
        for (int dy = -2; dy <= 2; dy++)
            for (int dx = -2; dx <= 2; dx++)
                if (dx*dx + dy*dy <= 4)
                    setPixel(img, S, S/2 + dx, S/2 + dy, 200, 180, 240, 150);
        
        stbi_write_png("assets/sprites/tile_moving.png", S, S, 4, img, S * 4);
        printf("  Generated: assets/sprites/tile_moving.png (%dx%d)\n", S, S);
        delete[] img;
    }
}

// ─────────────────────────────────────────
// Background elements
// ─────────────────────────────────────────
void generateBackgroundSprites() {
    // ── Cloud (64x32) ──
    {
        const int W = 64, H = 32;
        unsigned char* img = new unsigned char[W * H * 4]();
        // Three overlapping circles
        int circles[][3] = {{20, 16, 12}, {35, 14, 14}, {50, 16, 10}};
        for (int c = 0; c < 3; c++) {
            int cx = circles[c][0], cy = circles[c][1], cr = circles[c][2];
            for (int y = 0; y < H; y++) {
                for (int x = 0; x < W; x++) {
                    float dist = sqrt((x-cx)*(x-cx) + (y-cy)*(y-cy));
                    if (dist < cr) {
                        float t = dist / cr;
                        unsigned char a = (unsigned char)(220 * (1 - t * t));
                        // Only write if more opaque
                        int idx = (y * W + x) * 4;
                        if (a > img[idx + 3]) {
                            img[idx] = 255; img[idx+1] = 255; img[idx+2] = 255; img[idx+3] = a;
                        }
                    }
                }
            }
        }
        stbi_write_png("assets/sprites/cloud.png", W, H, 4, img, W * 4);
        printf("  Generated: assets/sprites/cloud.png (%dx%d)\n", W, H);
        delete[] img;
    }
}

// ─────────────────────────────────────────
// Particle sprite (8x8 soft circle)
// ─────────────────────────────────────────
void generateParticleSprite() {
    const int S = 8;
    unsigned char* img = new unsigned char[S * S * 4]();
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            float dist = sqrt((x - 3.5f) * (x - 3.5f) + (y - 3.5f) * (y - 3.5f));
            if (dist < 4.0f) {
                float alpha = (1.0f - dist / 4.0f);
                alpha = alpha * alpha;  // Smooth falloff
                setPixel(img, S, x, y, 255, 255, 255, (unsigned char)(alpha * 255));
            }
        }
    }
    stbi_write_png("assets/sprites/particle.png", S, S, 4, img, S * 4);
    printf("  Generated: assets/sprites/particle.png (%dx%d)\n", S, S);
    delete[] img;
}

int main() {
    printf("Generating sprite assets...\n");
    generatePlayerSprites();
    generateCoinSprites();
    generateEnemySprites();
    generatePlatformTiles();
    generateBackgroundSprites();
    generateParticleSprite();
    printf("Done! All sprites generated in assets/sprites/\n");
    return 0;
}
