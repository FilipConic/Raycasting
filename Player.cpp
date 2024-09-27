#include "Player.h"

#include <cctype>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <numbers>
#include <iostream>
#include <cmath>

using namespace std;

#define assertm(condition, msg) assert(((void)msg, condition));

///////////////////////////////////////////////////////////////
// Defininitions for the Map struct used by the Player class //
///////////////////////////////////////////////////////////////

Map::Map(const Scene& _scene, const string& file_path) : scene(_scene), width(0), height(0), cell_size(0), cells(nullptr) { read_from_map_file(file_path); }
Map::~Map() { delete[] cells; }

void Map::read_from_map_file(const string& file_path) {
    ifstream file(file_path, ios::in);
    assertm(file.is_open(), "Unable to open map file!\n");

    stringstream stream;
    string line, word;

    auto word_to_map_color = [&word]() -> MapElement {
        transform(word.begin(), word.end(), word.begin(), [](unsigned char c){ return std::tolower(c); });
        if      (word == "black")      return M_BLACK;
        else if (word == "dark_gray")  return M_DARK_GRAY;
        else if (word == "gray")       return M_GRAY;
        else if (word == "light_gray") return M_LIGHT_GRAY;
        else if (word == "white")      return M_WHITE;
        else if (word == "red")        return M_RED;
        else if (word == "green")      return M_GREEN;
        else if (word == "blue")       return M_BLUE;
        else if (word == "yellow")     return M_YELLOW;
        else if (word == "purple")     return M_PURPLE;
        else if (word == "cyan")       return M_CYAN;
        else if (word == "orange")     return M_ORANGE;
        else if (word == "pink")       return M_PINK;
        else if (word == "brick")      return M_BRICK;
        else if (word == "gravel")     return M_GRAVEL;
        else                           return M_NONE;
    };

    int w;
    while (getline(file, line)) {
        w = 0;
        stream.clear();
        stream << line;
        while (!stream.eof()) {
            stream >> word;
            ++w;
        }
        width = (w >= width ? w : width);
        ++height;
    }
    cells = new MapElement[width * height];
    memset(cells, 0, sizeof(MapElement) * width * height);

    file.close();
    file.open(file_path, ios::in);
    assertm(file.is_open(), "Unable to open map file!\n");

    int i, j = 0;
    while (getline(file, line)) {
        i = 0; 
        stream.clear();
        stream << line;
        while (!stream.eof()) {
            stream >> word;
            cells[i++ + j * width] = word_to_map_color();
        }
        ++j;
    }

    file.close();
}
const Color& map_element_to_color(Map::MapElement m_clr) {
    switch (m_clr) {
    case Map::M_BLACK:      return BLACK;
    case Map::M_DARK_GRAY:  return DARK_GRAY;
    case Map::M_GRAY:       return GRAY;
    case Map::M_LIGHT_GRAY: return LIGHT_GRAY;
    case Map::M_WHITE:      return WHITE;
    case Map::M_RED:        return RED;
    case Map::M_GREEN:      return GREEN;
    case Map::M_BLUE:       return BLUE;
    case Map::M_YELLOW:     return YELLOW;
    case Map::M_PURPLE:     return PURPLE;
    case Map::M_CYAN:       return CYAN;
    case Map::M_ORANGE:     return ORANGE;
    case Map::M_PINK:       return PINK;
    case Map::M_BRICK:      return RED;
    case Map::M_GRAVEL:     return GRAY;
    default:                return NO_COLOR;
    }
} 
void Map::draw(int map_size, const Vec2<int>& translation_vec) {
    cell_size = map_size / (height > width ? height : width);
    SDL_Rect rect{translation_vec.x, translation_vec.y, width * cell_size,  height * cell_size};

    scene.draw_rect(rect, WHITE);
    rect.w = cell_size;
    rect.h = cell_size;
    MapElement curr_cell;
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            curr_cell = cells[i + j * width];
            if (curr_cell)
                scene.draw_rect(rect, map_element_to_color(curr_cell));
            scene.draw_rect_border(rect);
            rect.x += cell_size;
        }
        rect.x = translation_vec.x;
        rect.y += cell_size;
    }
        
}

//////////////////////////////
// Player class definitions //
//////////////////////////////

Player::Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map) : scene(_scene), images(_image_loader), map(_map), pos(0, 0) {}
Player::Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map, float _x, float _y, int _ray_num, int _FOV) : scene(_scene), images(_image_loader), map(_map), pos(_x, _y), curr_angle(0), FOV(_FOV * numbers::pi_v<float> / 180.f), ray_num(_ray_num), ray_array(new Ray[ray_num]) { calculate_beginning_rays(); }
Player::~Player() { delete[] ray_array; }

const float HALF_PI = numbers::pi_v<float> / 2.f;
void Player::calculate_beginning_rays() {
    n = angle_to_vec(curr_angle);
    Vec2<float> between_rays = (angle_to_vec(curr_angle + HALF_PI) * (2.f * sin(FOV / 2.f) * SIZE / ray_num));
    //cout << between_rays.x << ", " << between_rays.y << endl;
    (ray_array[0].hit_point = n * SIZE).rotate(-FOV / 2.f);
    for (int i = 1; i < ray_num; ++i)
        ray_array[i].hit_point = between_rays + ray_array[i-1].hit_point;
}
void Player::calculate_rays() {
    float dx, dy;
    Vec2<float> prev, curr, hit_x, hit_y;
    bool curr_is_x, off_the_map;
    Map::MapElement hit_color;

    auto calculate_one_ray = [&](Ray& ray) -> void {
        dy = abs(ray.hit_point.x) > 1e-5 ? ray.hit_point.y / ray.hit_point.x : 1e6;
        dx = abs(ray.hit_point.y) > 1e-5 ? ray.hit_point.x / ray.hit_point.y : 1e6;

        curr = pos;
        curr_is_x = true;
        off_the_map = false;
        hit_color = Map::M_NONE;

        auto set_curr = [&]() -> void {
            if ((hit_x - pos).magnitude_squared() < (hit_y - pos).magnitude_squared()) {
                curr = hit_x;
                curr_is_x = true;
            } else {
                curr = hit_y;
                curr_is_x = false;
            }

            if (curr_is_x) {
                if (abs(curr.x - map.width) < 1e-3 || abs(curr.x) < 1e-3) off_the_map = true;
            } else {
                if (abs(curr.y - map.height) < 1e-3 || abs(curr.y) < 1e-3) off_the_map = true;
            }
        };

        if (ray.hit_point.x >= 0 && ray.hit_point.y >= 0) {
            do {
                hit_x.x = floor(curr.x) + 1;
                hit_x.y = curr.y + (hit_x.x - curr.x) * dy;

                hit_y.y = floor(curr.y) + 1;
                hit_y.x = curr.x + (hit_y.y - curr.y) * dx;
                
                set_curr();
                if (off_the_map) { hit_color = Map::M_NONE; break; }

                hit_color = map.cells[(int)(curr.x) + (int)(curr.y) * map.width];
            } while (!hit_color);
        } else if (ray.hit_point.x < 0 && ray.hit_point.y >= 0) {
           do {
                hit_x.x = ceil(curr.x) - 1;
                hit_x.y = curr.y + (hit_x.x - curr.x) * dy;

                hit_y.y = floor(curr.y) + 1;
                hit_y.x = curr.x + (hit_y.y - curr.y) * dx;

                set_curr();
                if (off_the_map) { hit_color = Map::M_NONE; break; }

                if (curr_is_x) hit_color = map.cells[(int)(curr.x - 1) + (int)(curr.y) * map.width];
                else           hit_color = map.cells[(int)(curr.x)     + (int)(curr.y) * map.width];  
            } while (!hit_color);
        } else if (ray.hit_point.x >= 0 && ray.hit_point.y < 0) {
            do {
                hit_x.x = floor(curr.x) + 1;
                hit_x.y = curr.y + (hit_x.x - curr.x) * dy;

                hit_y.y = ceil(curr.y) - 1;
                hit_y.x = curr.x + (hit_y.y - curr.y) * dx;
            
                set_curr();
                if (off_the_map) { hit_color = Map::M_NONE; break; }


                if (curr_is_x) hit_color = map.cells[(int)(curr.x) + (int)(curr.y)     * map.width];
                else           hit_color = map.cells[(int)(curr.x) + (int)(curr.y - 1) * map.width];
            } while (!hit_color);
        } else {
            do {
                hit_x.x = ceil(curr.x) - 1;
                hit_x.y = curr.y + (hit_x.x - curr.x) * dy;

                hit_y.y = ceil(curr.y) - 1;
                hit_y.x = curr.x + (hit_y.y - curr.y) * dx;

                set_curr();
                if (off_the_map) { hit_color = Map::M_NONE; break; }

                if (curr_is_x) hit_color = map.cells[(int)(curr.x - 1) + (int)(curr.y)     * map.width];
                else           hit_color = map.cells[(int)(curr.x)     + (int)(curr.y - 1) * map.width];
            } while (!hit_color);
        }
        if (curr_is_x) { ray.in_block_pos = curr.y - floor(curr.y); }
        else           { ray.in_block_pos = curr.x - floor(curr.x); }
        ray.hit_point = curr - pos;
        ray.element = hit_color;
    };
    for (int i = 0; i < ray_num; ++i)
        calculate_one_ray(ray_array[i]);
}
void Player::move(float dt, const Keyboard& keyboard) {
    Vec2<float> movement_vec{0.f, 0.f};

    float speed = SPEED * dt;
    if (keyboard[SDL_SCANCODE_LCTRL])
        speed *= 1.75f;
    if (keyboard[SDL_SCANCODE_W])
        movement_vec += n * speed;
    if (keyboard[SDL_SCANCODE_S])
        movement_vec -= n * speed;
    if (keyboard[SDL_SCANCODE_D])
        curr_angle += TURN_SPEED * dt;
    if (keyboard[SDL_SCANCODE_A])
        curr_angle -= TURN_SPEED * dt;
    
    /////////////// cut of movement ///////////////
    Vec2<float> new_pos(pos + movement_vec);
    new_pos.x = max(min(new_pos.x, (float)map.width), 0.f);
    new_pos.y = max(min(new_pos.y, (float)map.height), 0.f);
    Vec2<int> new_pos_int(new_pos);
    Vec2<int> top_left    {(int)(pos.x - SIZE), (int)(pos.y - SIZE)};
    top_left.x = max(top_left.x, 0);
    top_left.y = max(top_left.y, 0);
    Vec2<int> bottom_right{(int)(pos.x + SIZE), (int)(pos.y + SIZE)};
    bottom_right.x = min(bottom_right.x, map.width);
    bottom_right.y = min(bottom_right.y, map.height);

    auto magn = [](float x, float y) -> float {
        return x * x + y * y;
    };
    float size_squared = SIZE * SIZE - 0.05f;
    if (movement_vec.x > 0) {
        if (bottom_right.x >= map.width) {
            movement_vec.x = 0.f;
        } else if (map.cells[bottom_right.x + new_pos_int.y * map.width]) {
            movement_vec.x = 0.f;
        } else if (map.cells[bottom_right.x + top_left.y * map.width]) {
            if (magn(pos.x - bottom_right.x, pos.y - new_pos_int.y) < size_squared) movement_vec.x = 0.f;
        } else if (map.cells[bottom_right.x + bottom_right.y * map.width]) {
            if (magn(pos.x - bottom_right.x, pos.y - bottom_right.y) < size_squared) movement_vec.x = 0.f;
        }
    } else if (movement_vec.x < 0) {
        if (top_left.x < 0) {
            movement_vec.x = 0.f;
        } else if (map.cells[top_left.x + new_pos_int.y * map.width]) {
            movement_vec.x = 0.f;
        } else if (map.cells[top_left.x + top_left.y * map.width]) {
            if (magn(pos.x - new_pos_int.x, pos.y - new_pos_int.y) < size_squared) movement_vec.x = 0.f; 
        } else if (map.cells[top_left.x + bottom_right.y * map.width]) {
            if (magn(pos.x - new_pos_int.x, pos.y - bottom_right.y) < size_squared) movement_vec.x = 0.f;
        }
    }
    if (movement_vec.y > 0) {
        if (bottom_right.y >= map.height) {
            movement_vec.y = 0.f;
        } else if (map.cells[new_pos_int.x + bottom_right.y * map.width]) {
            movement_vec.y = 0.f;
        } else if (map.cells[top_left.x + bottom_right.y * map.width]) {
            if (magn(pos.x - new_pos_int.x, pos.y - bottom_right.y) < size_squared) movement_vec.y = 0.f;
        } else if (map.cells[bottom_right.x + bottom_right.y * map.width]) {
            if (magn(pos.x - bottom_right.x, pos.y - bottom_right.y) < size_squared) movement_vec.y = 0.f;
        }
    } else if (movement_vec.y < 0) {
        if (top_left.y < 0) {
            movement_vec.y = 0.f;
        } else if (map.cells[new_pos_int.x + top_left.y * map.width]) {
            movement_vec.y = 0.f;
        } else if (map.cells[top_left.x + top_left.y * map.width]) {
            if (magn(pos.x - new_pos_int.x, pos.y - new_pos_int.y) < size_squared) movement_vec.y = 0.f;
        } else if (map.cells[bottom_right.x + top_left.y * map.width]) {
            if (magn(pos.x - bottom_right.x, pos.y - new_pos_int.y) < size_squared) movement_vec.y = 0.f;
        }
    }
    ///////////////////////////////////////////////////////////////////

    movement_vec.x = max(min(movement_vec.x, speed), -speed);
    movement_vec.y = max(min(movement_vec.y, speed), -speed);

    pos += movement_vec;
}
void Player::draw(const Vec2<int>& translation_vec) {
    scene.draw_circle(translation_vec + map.cell_size * pos, map.cell_size * SIZE, GREEN);
    for (int i = 0; i < ray_num; ++i) {
        scene.draw_line(translation_vec + pos * map.cell_size, translation_vec + (pos + ray_array[i].hit_point) * map.cell_size);
    }
    scene.draw_line(translation_vec + map.cell_size * pos, translation_vec + map.cell_size * (pos + n), BLUE);
}
void Player::draw_walls() {
    auto visibility_fall_off = [](float x) -> float { return 1.f - 1.f / (1.f + exp(2.5f*(7.f - x))); };
    auto brightness_fall_off = [](float x) -> float { return -x / 20.f + 1.f; };

    calculate_beginning_rays();
    calculate_rays();

    SDL_Rect rect, image_rect;
    int rect_width = scene.get_width() / ray_num;
    int s_height = scene.get_height();
    float dist, height;
    
    Vec2<float> ray_hit;
    Color color;
    Image img;
    unsigned char alpha;

    for (int i = 0; i < ray_num; ++i) {
        Ray& ray = ray_array[i];
        if (!ray.element) continue;

        dist = n * ray.hit_point;
        if (abs(dist) > 1e-5)
            height = s_height / dist;

        rect = {
            i * rect_width,
            (int)(s_height / 2.f - height / 2.f),
            rect_width,
            (int)(height)
        };
        if (ray.element & 0xC0) {
            alpha = (unsigned char)(visibility_fall_off(dist) * 255);
            switch (ray.element) {
            case Map::M_BRICK:  img = images.get_image(1); break;
            case Map::M_GRAVEL: img = images.get_image(2); break;
            default:            img = images.get_image(0); break;
            }
            image_rect = {
                (int)(ray.in_block_pos * img.width),
                0,
                1,
                img.height
            };
            SDL_SetTextureAlphaMod(img.texture, alpha);
            SDL_RenderCopy(scene, img.texture, &image_rect, &rect);
            SDL_SetTextureAlphaMod(img.texture, 255);
        } else {
            if (abs(ray.in_block_pos - 0.5f) > 0.48f) color = BLACK;
            else color = map_element_to_color(ray.element);
            color *= brightness_fall_off(dist);
            color.a = (unsigned char)((float)color.a * visibility_fall_off(dist));
            scene.draw_rect(rect, color);
        }
    }
}
