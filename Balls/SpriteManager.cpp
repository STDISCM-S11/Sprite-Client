#include "SpriteManager.h"
#include "Sprite.h"

using namespace std;

vector<Sprite> SpriteManager::sprites;
Sprite* mainSprite = nullptr; // Initialize mainSprite pointer

SpriteManager::SpriteManager() : explorerMode(false), pixelWidth(50.f), pixelHeight(50.f) {
    // Initialize mainSprite if needed, depending on how Sprite is defined
}

void SpriteManager::addSprites(const Sprite& sprite) {
    sprites.push_back(sprite);
}

void SpriteManager::drawSprites(float cameraX, float cameraY, bool isExplorerMode) {
    if (isExplorerMode) {
        // Set up camera view centered on the main sprite
        if (mainSprite) {
            const float peripheryTileSize = 5.0f; // Tile size in pixels
            const int peripheryWidthTiles = 33; // Number of horizontal tiles
            const int peripheryHeightTiles = 19; // Number of vertical tiles
            const float peripheryWidth = peripheryWidthTiles * peripheryTileSize;
            const float peripheryHeight = peripheryHeightTiles * peripheryTileSize;

            cameraX = mainSprite->getX() - peripheryWidth / 2.0f;
            cameraY = mainSprite->getY() - peripheryHeight / 2.0f;

            // Set up camera view
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(cameraX, cameraX + peripheryWidth, cameraY, cameraY + peripheryHeight);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Draw the main sprite at the center of the camera
            mainSprite->draw(0, 0);

            // Draw other sprites relative to the main sprite
            for (auto& sprite : sprites) {
                if (&sprite != mainSprite) {
                    float relX = sprite.getX() - mainSprite->getX();
                    float relY = sprite.getY() - mainSprite->getY();

                    if (relX >= -peripheryWidth / 2.0f &&
                        relX <= peripheryWidth / 2.0f &&
                        relY >= -peripheryHeight / 2.0f &&
                        relY <= peripheryHeight / 2.0f) {

                        float drawX = relX + peripheryWidth / 2.0f;
                        float drawY = relY + peripheryHeight / 2.0f;

                        // Draw the sprite
                        sprite.draw(drawX, drawY);
                    }
                }
            }
        }
    }
    else {
        // Standard drawing code for non-explorer mode
        for (auto& sprite : sprites) {
            sprite.draw(cameraX, cameraY);
        }
    }
}

void SpriteManager::setMainSprite(Sprite* sprite) {
    mainSprite = sprite;
}

Sprite* SpriteManager::getMainSprite() {
    return mainSprite;
}

vector<Sprite>& SpriteManager::getSprites() {
    return sprites;
}
