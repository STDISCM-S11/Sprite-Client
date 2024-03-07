#pragma once
#include "Sprite.h"
#include <vector>

using namespace std;

class SpriteManager {
public:
    static void addSprites(const Sprite& sprite);
    static void drawSprites();
    static vector<Sprite> getSprites();

private:
    static vector<Sprite> sprites;
};