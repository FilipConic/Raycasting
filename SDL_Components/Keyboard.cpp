#include "Keyboard.h"

Keyboard::Keyboard() { state = SDL_GetKeyboardState(NULL); }
void Keyboard::update() const { SDL_PumpEvents(); }
bool Keyboard::operator[](SDL_Scancode code) const { return state[code]; }