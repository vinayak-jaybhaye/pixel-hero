#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glut.h>
#include <string>
#include <map>

struct Sprite {
    GLuint textureID;
    int width, height;
    int frameWidth, frameHeight;
    int frameCount;
};

class TextureManager {
   private:
    std::map<std::string, Sprite> sprites;
    static TextureManager* instance;

   public:
    static TextureManager& getInstance();

    // Load a sprite sheet. frameCount=1 for single images.
    bool loadSprite(const std::string& name, const std::string& path, int frameCount = 1);

    // Draw a sprite frame at position, with optional scaling and flip
    void drawSprite(const std::string& name, float x, float y, float scaleX = 1.0f,
                    float scaleY = 1.0f, int frame = 0, bool flipX = false, float r = 1.0f,
                    float g = 1.0f, float b = 1.0f, float a = 1.0f);

    // Draw a tiled sprite across a rectangular area
    void drawTiled(const std::string& name, float x, float y, float width, float height);

    const Sprite* getSprite(const std::string& name) const;

    void cleanup();
};

#endif
