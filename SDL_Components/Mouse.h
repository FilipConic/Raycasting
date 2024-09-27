#pragma once

#include "Vec2.h"
#include <SDL2/SDL.h>

struct Mouse {
    Vec2<int> pos;
    bool left, middle, right;

    Mouse();
    void update();
};