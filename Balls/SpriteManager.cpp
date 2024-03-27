#include "SpriteManager.h"
#include "Sprite.h"

using namespace std;


vector<Sprite> SpriteManager::sprites;

SpriteManager::SpriteManager() : explorerMode(false), pixelWidth(50.f), pixelHeight(50.f) {
    // Initialize mainSprite if needed, depending on how Sprite is defined
}

void SpriteManager::addSprites(const Sprite& sprite) {
    sprites.push_back(sprite);
}

void SpriteManager::drawSprites(float cameraX, float cameraY, bool isExplorerMode) {
    if (isExplorerMode) {
        // Get the first sprite's position as the main sprite
        mainSprite = sprites.front(); // Assuming mainSprite is the sprite you control

        const float peripheryTileSize = 5.0f; // Tile size in pixels
        const int peripheryWidthTiles = 33; // Number of horizontal tiles
        const int peripheryHeightTiles = 19; // Number of vertical tiles
        const float peripheryWidth = peripheryWidthTiles * peripheryTileSize;
        const float peripheryHeight = peripheryHeightTiles * peripheryTileSize;

        // Assuming `mainSprite` has members `x` and `y` for position
        // Center camera on sprite
        cameraX = mainSprite.getX() - peripheryWidth / 2.0f;
        cameraY = mainSprite.getY() - peripheryHeight / 2.0f;

        // Set up camera view
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(cameraX, cameraX + peripheryWidth, cameraY, cameraY + peripheryHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        /// Since we're already centered on the sprite, it should be drawn at the camera's origin
        mainSprite.draw(0, 0);
    }
    else {
        // Standard drawing code for non-explorer mode
        for (auto& sprite : sprites) {
            sprite.draw(cameraX, cameraY);
        }
    }
}

vector<Sprite>& SpriteManager::getSprites() {
    return sprites;
}