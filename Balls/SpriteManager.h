//#pragma once
//#include "Sprite.h"
//#include <vector>
//
//using namespace std;
//
//class SpriteManager {
//public:
//    static void addSprites(const Sprite& sprite);
//    static void drawSprites(float cameraX, float cameraY);
//    static vector<Sprite> getSprites();
//
//    bool explorerMode;
//    Sprite mainSprite;
//    
//    // Assume these constants are defined somewhere globally or within an appropriate class
//    const float pixelWidth = 50.f;
//    const float pixelHeight = 50.f;
//
//
//private:
//    static vector<Sprite> sprites;
//};
//

#pragma once
#include "Sprite.h"
#include <vector>

using namespace std;

class SpriteManager {
public:
    SpriteManager(); // Default constructor
    static void addSprites(const Sprite& sprite); // Note: Should this be static?
    void drawSprites(float cameraX, float cameraY, bool isExplorerMode); // Changed to non-static
    static vector<Sprite>& getSprites();

    // These are no longer static
    bool explorerMode;
    Sprite mainSprite;
    float pixelWidth;
    float pixelHeight;

private:
    static vector<Sprite> sprites;
};

