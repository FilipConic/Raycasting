#pragma once

#include "Color.h"
#include "Vec2.h"
#include <SDL2/SDL.h>
#include <string>

class Scene {
    int width;
    int height;
    SDL_Renderer* renderer;
    SDL_Window* window;

public:
    Scene(const std::string& title, int _width, int _height);
    ~Scene();

    void clear(const Color& color = WHITE) const;
    inline void set_color(const Color& color = WHITE) const;

    int get_width() const;
    int get_height() const;
    operator SDL_Renderer*() const;

    void draw_rect_border(const SDL_Rect& rect, const Color& color = BLACK) const;
    void draw_rect(const SDL_Rect& rect, const Color& color = BLACK) const;
    void draw_line(const Vec2<int>& vec1, const Vec2<int>& vec2, const Color& = BLACK) const;
    void draw_circle(const Vec2<int>& pos, const int r, const Color& color = BLACK) const;
};