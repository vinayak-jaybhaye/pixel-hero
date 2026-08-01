#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.h"
#include <cstdio>

TextureManager* TextureManager::instance = nullptr;

TextureManager& TextureManager::getInstance() {
    if (!instance) {
        instance = new TextureManager();
    }
    return *instance;
}

bool TextureManager::loadSprite(const std::string& name, const std::string& path, int frameCount) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);  // OpenGL expects bottom-up
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);  // Force RGBA

    if (!data) {
        fprintf(stderr, "Failed to load sprite: %s (%s)\n", path.c_str(), stbi_failure_reason());
        return false;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Nearest-neighbor filtering for pixel art (no blurring)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    Sprite sprite;
    sprite.textureID = textureID;
    sprite.width = width;
    sprite.height = height;
    sprite.frameWidth = width / frameCount;
    sprite.frameHeight = height;
    sprite.frameCount = frameCount;

    sprites[name] = sprite;
    printf("  Loaded sprite: %s (%dx%d, %d frames)\n", name.c_str(), width, height, frameCount);
    return true;
}

void TextureManager::drawSprite(const std::string& name, float x, float y, float scaleX,
                                float scaleY, int frame, bool flipX, float r, float g, float b,
                                float a) {
    auto it = sprites.find(name);
    if (it == sprites.end()) return;

    const Sprite& sprite = it->second;

    // Clamp frame
    if (frame >= sprite.frameCount) frame = frame % sprite.frameCount;

    // Calculate texture coordinates for this frame
    float frameU = (float)sprite.frameWidth / sprite.width;
    float u0 = frame * frameU;
    float u1 = u0 + frameU;

    if (flipX) {
        float tmp = u0;
        u0 = u1;
        u1 = tmp;
    }

    // Calculate quad size
    float halfW = (sprite.frameWidth * scaleX) / 2.0f;
    float halfH = (sprite.frameHeight * scaleY) / 2.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sprite.textureID);
    glColor4f(r, g, b, a);

    glBegin(GL_QUADS);
    glTexCoord2f(u0, 0.0f);
    glVertex2f(x - halfW, y - halfH);  // Bottom-left
    glTexCoord2f(u1, 0.0f);
    glVertex2f(x + halfW, y - halfH);  // Bottom-right
    glTexCoord2f(u1, 1.0f);
    glVertex2f(x + halfW, y + halfH);  // Top-right
    glTexCoord2f(u0, 1.0f);
    glVertex2f(x - halfW, y + halfH);  // Top-left
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void TextureManager::drawTiled(const std::string& name, float x, float y, float width,
                               float height) {
    auto it = sprites.find(name);
    if (it == sprites.end()) return;

    const Sprite& sprite = it->second;

    float tilesX = width / sprite.width;
    float tilesY = height / sprite.height;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sprite.textureID);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(x, y);
    glTexCoord2f(tilesX, 0.0f);
    glVertex2f(x + width, y);
    glTexCoord2f(tilesX, tilesY);
    glVertex2f(x + width, y + height);
    glTexCoord2f(0.0f, tilesY);
    glVertex2f(x, y + height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

const Sprite* TextureManager::getSprite(const std::string& name) const {
    auto it = sprites.find(name);
    if (it != sprites.end()) return &(it->second);
    return nullptr;
}

void TextureManager::cleanup() {
    for (auto& pair : sprites) {
        glDeleteTextures(1, &pair.second.textureID);
    }
    sprites.clear();
}
