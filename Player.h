#pragma once

#include "SDL_Components\ImageLoader.h"
#include "SDL_Components\Keyboard.h"
#include "SDL_Components\Mouse.h"

struct Map {
    enum MapElement : unsigned char {
        M_NONE = 0,
        M_BLACK,
        M_DARK_GRAY,
        M_GRAY,
        M_LIGHT_GRAY,
        M_WHITE,
        M_RED,
        M_GREEN,
        M_BLUE,
        M_YELLOW,
        M_PURPLE,
        M_CYAN,
        M_ORANGE,
        M_PINK,
        M_BRICK = 64,
        M_GRAVEL
    };

    const Scene& scene;

    int width, height;
    int cell_size;

    MapElement* cells;

    Map(const Scene& _scene, const std::string& file_path);
    ~Map();

    void read_from_map_file(const std::string& file_path);
    void draw(int map_size, const Vec2<int>& translation_vec = {0, 0});
};

class Player {
    const Scene& scene;
    const ImageLoader& images;
    const Map& map;

    Vec2<float> pos;
    float curr_angle;
    float FOV;
    Vec2<float> n;

    struct Ray {
        Vec2<float> hit_point;
        float in_block_pos;
        Map::MapElement element;
    };
    int ray_num;
    Ray* ray_array;

    const float SIZE = .45f;
    const float SPEED = 5.f;
    const float TURN_SPEED = 1.5f;

    inline void calculate_beginning_rays();
    void calculate_rays();

    Player() = delete;
    Player(const Player&) = delete;
public:
    Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map);
    Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map, float _x, float _y, int ray_num = 100, int FOV = 90);
    ~Player();

    void move(float dt, const Keyboard& keyboard);
    void draw(const Vec2<int>& translation_vec = {0, 0});
    void draw_walls();
};
