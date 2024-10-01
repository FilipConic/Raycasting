#pragma once

#include "SDL_Components\Keyboard.h"
#include "SDL_Components\Mouse.h"
#include "Map.h"

class Player {
    const Scene& scene;
    const ImageLoader& images;
    const Map& map;

    Vec2<float> pos;
    float curr_angle;
    float FOV;
    Vec2<float> n;

    int ray_num;
    struct Ray {
        Vec2<float> hit_point;
        float in_block_pos;
        unsigned element;
    };

    SDL_Texture* ceiling_floor_texture;

    const float SIZE = .45f;
    const float SPEED = 5.f;
    const float TURN_SPEED = 1.5f;

    Player() = delete;
    Player(const Player&) = delete;
public:
    Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map);
    Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map, float _x, float _y, int ray_num = 100, int FOV = 90);
    ~Player();

    void move(float dt, const Keyboard& keyboard, Mouse& mouse);
    void draw(const Vec2<int>& translation_vec = {0, 0});
    void draw_walls();
    void draw_floor_and_ceiling();
    void update(float dt, const Keyboard& keyboard, Mouse& mouse);
};

Uint32 color_to_abgr(const Color& color);
void set_rgba_alpha(Uint32& color, float val);
void multiply_value_rgba(Uint32& color, float val);
