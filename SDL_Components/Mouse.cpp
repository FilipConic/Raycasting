#include "Mouse.h"

Mouse::Mouse() : pos(0, 0), left(false), middle(false), right(false), left_held(false), middle_held(false), right_held(false) { SDL_ShowCursor(SDL_DISABLE); }

void Mouse::update() {
    Uint32 bitmask = SDL_GetMouseState(&pos.x, &pos.y);
    if (left && !left_held) left_held = true;
    else if (!left) left_held = false;
    if (middle && !middle_held) middle_held = true;
    else if (!middle) middle_held = false;
    if (right && !right_held) right_held = true;
    else if (!right) right_held = false;
    left   = bitmask & SDL_BUTTON(1);
    middle = bitmask & SDL_BUTTON(2);
    right  = bitmask & SDL_BUTTON(3);
}
void Mouse::set_pos(int x, int y) {
    SDL_WarpMouseInWindow(NULL, x, y);
}