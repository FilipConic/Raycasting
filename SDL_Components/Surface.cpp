#include "Surface.h"

#include <iostream>
#include <cstring>

using namespace std;

Surface::Surface(const Scene& _scene) : scene(_scene), array(nullptr), width(10), height(10), size(width * height) {
    array = new Uint32[size];
    memset(array, 0, size * sizeof(Uint32));
}
Surface::Surface(const Scene& _scene, int _width, int _height) : scene(_scene), array(nullptr), width(_width), height(_height), size(width * height) {
    array = new Uint32[size];
    memset(array, 0, size * sizeof(Uint32));

    #if SDL_BYTEORDER == SDL_LIL_ENDIAN
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, sizeof(Uint32), 0xff, 0xff00, 0xff0000, 0xff000000);
    #else
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, sizeof(Uint32), 0, 0, 0, 0);
    #endif
    texture = SDL_CreateTextureFromSurface(scene, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(surface);
}
Surface::~Surface() {
    delete[] array;
    SDL_DestroyTexture(texture);
}

bool Surface::put_pixel(int pos_x, int pos_y, const Color& color) {
    if (pos_x < 0 || pos_x >= width || pos_y < 0 || pos_y >= height ) return false;
    array[pos_x + pos_y * width] = color_to_rgba(color);
    return true;
}
bool Surface::put_pixel(const Vec2<int>& pos, const Color& color) {
    if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height ) return false;
    array[pos.x + pos.y * width] = color_to_rgba(color);
    return true;
}

bool Surface::put_image(const Image& img, const SDL_Rect& dest_rect) {
    Uint32* pixels = (Uint32*)img.surface->pixels;

    for (int j = 0; j < img.height; ++j) {
        for (int i = 0; i < img.width; ++i) {
            array[i + j * width] = pixels[i + j * img.width];
        }
    }

    return true;
}


void Surface::render(const SDL_Rect& dest_rect) const {
    SDL_UpdateTexture(texture, NULL, array, width * sizeof(Uint32));
    SDL_Rect src_rect = {0, 0, width, height};
    SDL_RenderCopy(scene, texture, &src_rect, &dest_rect);
}

Uint32 color_to_rgba(const Color& color)  {
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Uint32 result = (color.a << 24) + (color.r << 16) + (color.g << 8) + color.b;
    #else
    Uint32 result = (color.r << 24) + (color.g << 16) + (color.b << 8) + color.a;
    #endif
    return result;
}