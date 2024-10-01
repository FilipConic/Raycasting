#pragma once

#include "Vec2.h"
#include <SDL2\SDL.h>

struct Mouse {
    Vec2<int> pos;
    bool left, middle, right;

    Mouse();
    void update();
    void set_pos(int x, int y);
};