#include "Scene.h"

#include <cassert>

using namespace std;

#define assertm(condition, msg) assert(((void)msg, condition))

Scene::Scene(const string& title, int _width, int _height) : width(_width), height(_height) {
    assertm(!SDL_Init(SDL_INIT_EVERYTHING), "System was unable to initialize SDL!\n");
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    assertm(window != NULL, "System was unable to create a window!\n");
    renderer = SDL_CreateRenderer(window, -1, 0);
    assertm(renderer != NULL, "System was unable to create a renderer!\n");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}
Scene::~Scene() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Scene::clear(const Color& color) const {
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}
void Scene::set_color(const Color& color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

int Scene::get_width() const { return width; }
int Scene::get_height() const { return height; }
Scene::operator SDL_Renderer*() const { return renderer; }

void Scene::draw_rect_border(const SDL_Rect& rect, const Color& color) const {
    set_color(color);
    SDL_RenderDrawRect(renderer, &rect);
}
void Scene::draw_rect(const SDL_Rect& rect, const Color& color) const {
    set_color(color);
    SDL_RenderFillRect(renderer, &rect);
}
void Scene::draw_line(const Vec2<int>& vec1, const Vec2<int>& vec2, const Color& color) const {
    set_color(color);
    SDL_RenderDrawLine(renderer, (int)vec1.x, (int)vec1.y, (int)vec2.x, (int)vec2.y);
}
void Scene::draw_circle(const Vec2<int>& pos, const int r, const Color& color) const {
    set_color(color);
    
    int r_squared = r * r, p = 0, i;
    for (int j = 0; j < r; ++j) {
        p = j * j;
        for (i = 0; i <= j; ++i) {
            if (i * i + p - r_squared < 0) {
                SDL_RenderDrawPoint(renderer, pos.x + i, pos.y + j);
                SDL_RenderDrawPoint(renderer, pos.x - i, pos.y + j);
                SDL_RenderDrawPoint(renderer, pos.x + i, pos.y - j);
                SDL_RenderDrawPoint(renderer, pos.x - i, pos.y - j);
            
                SDL_RenderDrawPoint(renderer, pos.x + j, pos.y + i);
                SDL_RenderDrawPoint(renderer, pos.x - j, pos.y + i);
                SDL_RenderDrawPoint(renderer, pos.x + j, pos.y - i);
                SDL_RenderDrawPoint(renderer, pos.x - j, pos.y - i);
            }
        }
    }
}


