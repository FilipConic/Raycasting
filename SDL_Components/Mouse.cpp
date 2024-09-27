#include "Mouse.h"

Mouse::Mouse() : pos(0, 0), left(false), middle(false), right(false) {}

void Mouse::update() {
    Uint32 bitmask = SDL_GetMouseState(&pos.x, &pos.y);
    left   = bitmask & SDL_BUTTON(1);
    middle = bitmask & SDL_BUTTON(2);
    right  = bitmask & SDL_BUTTON(3);
}