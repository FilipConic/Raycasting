#pragma once

#include "ImageLoader.h"

class Surface {
    const Scene& scene;
    SDL_Texture* texture;
    Uint32* array;

    int width, height;
    int size;

    Surface() = delete;
    Surface(const Surface&) = delete;
public:
    Surface(const Scene& _scene);
    Surface(const Scene& _scene, int _width, int _height);
    ~Surface();

    bool put_pixel(int pos_x, int pos_y, const Color& color = BLACK);
    bool put_pixel(const Vec2<int>& pos, const Color& color = BLACK);
    bool draw_rect(const SDL_Rect& rect, const Color& color = BLACK);
    bool draw_circle(int pos_x, int pos_y, int r, const Color& color = BLACK);
    bool draw_circle(const Vec2<int>& pos, int r, const Color& color = BLACK);
    bool put_image(const Image& img, const SDL_Rect& dest_rect);

    void render(const SDL_Rect& dest_rect) const;
};

Uint32 color_to_rgba(const Color& color);
