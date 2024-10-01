#pragma once

#include "SDL_Components\ImageLoader.h"

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
        M_GRAVEL,
        M_COBBLE
    };

    const Scene& scene;

    int width, height;
    int cell_size;

    unsigned* cells;

    Map(const Scene& _scene, const std::string& file_path);
    ~Map();

    void read_from_map_file(const std::string& map_file_path);
    void draw(int map_size, const Vec2<int>& translation_vec = {0, 0});
};

const Color& map_element_to_color(Map::MapElement m_clr);

const unsigned WALL_MASK = 0xff;
const unsigned FLOOR_MASK = WALL_MASK << 8;
const unsigned CEILING_MASK = WALL_MASK << 16;

