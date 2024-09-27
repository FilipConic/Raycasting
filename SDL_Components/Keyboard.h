#pragma once

#include <SDL2/SDL.h>

class Keyboard {
    const Uint8* state;

public:
    Keyboard();

    void update() const;
    bool operator[](SDL_Scancode code) const;
};