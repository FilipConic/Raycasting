#include "Player.h"

#include <cassert>
#include <numbers>
#include <iostream>
#include <cmath>
#include <cwchar>

using namespace std;

#define assertm(condition, msg) assert(((void)msg, condition));

const float DOUBLE_PI = numbers::pi_v<float> * 2.f;
const float HALF_PI = numbers::pi_v<float> / 2.f;

//////////////////////////////
// Player class definitions //
//////////////////////////////

Player::Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map) : scene(_scene), images(_image_loader), map(_map), pos(0, 0), ceiling_floor_texture(nullptr) {}
Player::Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map, float _x, float _y, int _ray_num, int _FOV) : scene(_scene), images(_image_loader), map(_map), pos(_x, _y), curr_angle(0), FOV(_FOV * numbers::pi_v<float> / 180.f), ray_num(_ray_num) {
    ceiling_floor_texture = SDL_CreateTexture(scene, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, scene.get_width(), scene.get_height());
    SDL_SetTextureBlendMode(ceiling_floor_texture, SDL_BLENDMODE_BLEND);
    void* pixels;
    int pitch;
    SDL_LockTexture(ceiling_floor_texture, NULL, &pixels, &pitch);
    memset(pixels, 0, pitch * scene.get_height());
    SDL_UnlockTexture(ceiling_floor_texture);
}
Player::~Player() { SDL_DestroyTexture(ceiling_floor_texture); }

void Player::move(float dt, const Keyboard& keyboard, Mouse& mouse) {
    Vec2<float> movement_vec{0.f, 0.f};

    float speed = SPEED * dt;
    if (keyboard[SDL_SCANCODE_LCTRL]) speed *= 1.5f;
    if (keyboard[SDL_SCANCODE_W])   movement_vec +=  n;
    if (keyboard[SDL_SCANCODE_S])   movement_vec -=  n;
    if (keyboard[SDL_SCANCODE_D]) { movement_vec += (n * 1).rotate90(); }
    if (keyboard[SDL_SCANCODE_A]) { movement_vec -= (n * 1).rotate90(); }

    int scene_width = scene.get_width() / 2;
    int diff = scene_width - mouse.pos.x;
    auto interpolation = [&](float t) -> float {
        float y = 1 / (1 + exp(3 - 3 * t));
        return y * mouse.pos.x + (1 - y) * scene_width;
    };
    if (abs(diff) >= 15) {
        float mouse_movement = interpolation(1e-6f * abs(diff));
        mouse.set_pos(mouse_movement, scene_width);
        curr_angle += (mouse_movement - scene_width) / SIZE * 9e-3f;
    }

    if      (curr_angle > DOUBLE_PI) curr_angle -= DOUBLE_PI;
    else if (curr_angle < 0) curr_angle += DOUBLE_PI;
    
    /////////////// cut of movement ///////////////
    if (abs(movement_vec.x) > 1e-2 && abs(movement_vec.y) > 1e-2) movement_vec.normalize() *= speed;
    Vec2<float> new_pos(pos + movement_vec);
    if (new_pos.x > map.width)  new_pos.x = map.width;
    else if (new_pos.x < 0)     new_pos.x = 0;
    if (new_pos.y > map.height) new_pos.y = map.height;
    else if (new_pos.y < 0)     new_pos.y = 0;
    Vec2<int> new_pos_int(new_pos);
    Vec2<int> top_left    {(int)(pos.x - SIZE), (int)(pos.y - SIZE)};
    if (top_left.x < 0) top_left.x = 0;
    if (top_left.y < 0) top_left.y = 0;
    Vec2<int> bottom_right{(int)(pos.x + SIZE), (int)(pos.y + SIZE)};
    if (bottom_right.x > map.width) bottom_right.x = map.width;
    if (bottom_right.y > map.height) bottom_right.y = map.height;

    auto magn = [](float x, float y) -> float { return x * x + y * y; };
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
    scene.draw_line(translation_vec + map.cell_size * pos, translation_vec + map.cell_size * (pos + n), BLUE);
}
void Player::draw_walls() {
    auto visibility_fall_off = [](float x) -> float { return 1.f - 1.f / (1.f + exp(2.5f*(7.f - x))); };
    auto brightness_fall_off = [](float x) -> float { return -x / 20.f + 1.f; };

    float dx, dy;
    Vec2<float> prev, curr, hit_x, hit_y;
    bool curr_is_x, off_the_map;
    char x_side, y_side;
    Map::MapElement hit_color;

    auto calculate_one_ray = [&](Ray& ray) -> void {
        dy = abs(ray.hit_point.x) > 1e-5 ? ray.hit_point.y / ray.hit_point.x : 1e6;
        dx = abs(ray.hit_point.y) > 1e-5 ? ray.hit_point.x / ray.hit_point.y : 1e6;

        x_side = ray.hit_point.x >= 0 ? 1 : 0;
        y_side = ray.hit_point.y >= 0 ? 1 : 0;

        if (x_side) hit_x.x =  ceil(pos.x);
        else        hit_x.x = floor(pos.x);
        hit_x.y = pos.y + (hit_x.x - pos.x) * dy;

        if (y_side) hit_y.y =  ceil(pos.y);
        else        hit_y.y = floor(pos.y);               
        hit_y.x = pos.x + (hit_y.y - pos.y) * dx;

        off_the_map = false;
        curr_is_x = (hit_x - pos).magnitude_squared() < (hit_y - pos).magnitude_squared();        
        
        if (curr_is_x) {
            if (abs(map.width - hit_x.x) < 1e-3 || abs(hit_x.x) < 1e-3) {
                hit_color = Map::M_NONE;
                off_the_map = true;
            }
        } else {
            if (abs(map.height - hit_y.y) < 1e-3 || abs(hit_y.y) < 1e-3) {
                hit_color = Map::M_NONE;
                off_the_map = true;
            }
        }
        
        if (!off_the_map)  {
            hit_color = curr_is_x ?
            map.cells[(int)(hit_x.x + x_side - 1) + (int)(hit_x.y)              * map.width] :
            map.cells[(int)(hit_y.x)              + (int)(hit_y.y + y_side - 1) * map.width];
            while (!hit_color) {
                if (curr_is_x) {
                    if (x_side) {
                        hit_x.x += 1;
                        hit_x.y += dy;
                    } else {
                        hit_x.x -= 1;
                        hit_x.y -= dy;
                    }
                } else {
                    if (y_side) {
                        hit_y.y += 1;
                        hit_y.x += dx;
                    } else {
                        hit_y.y -= 1;
                        hit_y.x -= dx;
                    }                    
                }
                curr_is_x = (hit_x - pos).magnitude_squared() < (hit_y - pos).magnitude_squared();
                if (curr_is_x) {
                    hit_color = map.cells[(int)(hit_x.x + x_side - 1) + (int)(hit_x.y) * map.width];
                    if (map.width - hit_x.x < 1e-3 || hit_x.x < 1e-3) {
                        hit_color = Map::M_NONE;
                        break;
                    }
                } else {
                    hit_color = map.cells[(int)(hit_y.x) + (int)(hit_y.y + y_side - 1) * map.width];
                    if (map.height - hit_y.y < 1e-3 || hit_y.y < 1e-3) {
                        hit_color = Map::M_NONE;
                        break;
                    }
                }
            }
        } else {
            hit_color = Map::M_NONE;
        }
        ray.in_block_pos = curr_is_x ? hit_x.y - floor(hit_x.y) : hit_y.x - floor(hit_y.x); 
        ray.hit_point = curr_is_x ? hit_x - pos : hit_y - pos;
        ray.element = hit_color;
    };

    SDL_Rect rect, image_rect;
    int rect_width = scene.get_width() / ray_num;
    int s_height = scene.get_height();
    float dist, height;

    Vec2<float> ray_hit;
    Color color;
    Image img;
    unsigned char alpha;

    Vec2<float> between_rays = (angle_to_vec(curr_angle + HALF_PI) * (2.f * sin(FOV / 2.f) * SIZE / (ray_num - 1)));
    Ray curr_ray;
    Vec2<float> begin_vec;

    begin_vec = n * SIZE;
    begin_vec.rotate(-FOV / 2.f);

    for (int i = 0; i < ray_num; ++i) {
        curr_ray.hit_point = begin_vec + between_rays * i;

        calculate_one_ray(curr_ray);

        if (!curr_ray.element) continue;

        dist = n * curr_ray.hit_point;
        if (abs(dist) > 1e-5)
            height = s_height / dist;

        rect = {
            i * rect_width,
            (int)((s_height - height) / 2.f),
            rect_width,
            (int)(height)
        };
        
        if ((curr_ray.element & WALL_MASK) & 0xC0) {
            switch (curr_ray.element) {
            case Map::M_BRICK:  img = images.get_image(1); break;
            case Map::M_GRAVEL: img = images.get_image(2); break;
            case Map::M_COBBLE: img = images.get_image(3); break;
            default:            img = images.get_image(0); break;
            }
            image_rect = {
                (int)(curr_ray.in_block_pos * img.width),
                0,
                1,
                img.height
            };
            alpha = (unsigned char)(visibility_fall_off(dist) * 255);
            SDL_SetTextureAlphaMod(img.texture, alpha);
            SDL_RenderCopy(scene, img.texture, &image_rect, &rect);
            SDL_SetTextureAlphaMod(img.texture, 255);

            color = BLACK;
            color.alpha((1 - brightness_fall_off(dist)) * visibility_fall_off(dist));
            scene.draw_rect(rect, color);
        } else {
            color = map_element_to_color(curr_ray.element);
            color *= brightness_fall_off(dist);
            color.alpha(visibility_fall_off(dist));
            scene.draw_rect(rect, color);
        }
    }
}
void Player::draw_floor_and_ceiling() {
    auto visibility_fall_off = [](float x) -> float { return 1.f - 1.f / (1.f + exp(2.5f*(9.f - x))); };
    auto brightness_fall_off = [](float x) -> float { return -x / 20.f + 1.f; };

    Image img = images.get_image(3);
    Uint32* img_pixels = (Uint32*)img.surface->pixels;

    const int width = scene.get_width();
    const int height = scene.get_height();

    int img_x, img_y;
    float curr_len = 1;

    Vec2<float> right = angle_to_vec(curr_angle + FOV / 2.f);
    Vec2<float> left  = angle_to_vec(curr_angle - FOV / 2.f);
    Vec2<float> vec, curr_left, curr_right;

    int pitch;
    void* void_pixels;
    SDL_LockTexture(ceiling_floor_texture, NULL, &void_pixels, &pitch);
    Uint32* pixels = (Uint32*)void_pixels;
    memset(void_pixels, 0, pitch * height);

    const float angle_inc = HALF_PI / height;
    const int end = height * width / 2;
    const int set_size = width / ray_num;
    float a = HALF_PI / 2.f;

    Uint32 color;

    bool repeat = false;

    int j_floor = (height - 1) * width;
    for (int j_ceil = 0; j_ceil <= end;) {
        if (repeat) {
            a += angle_inc;
            memcpy(&(pixels[j_ceil ]), &(pixels[j_ceil  - width]), pitch);
            memcpy(&(pixels[j_floor]), &(pixels[j_floor + width]), pitch);
        } else {
            curr_len   = tan(a += angle_inc);
            curr_left  = left  * curr_len + pos;
            curr_right = right * curr_len + pos;

            for (int i = 0; i < width; i += set_size) {
                vec = lerp(curr_right, curr_left, (float)i / (width - 1.f));
                if (vec.x < 0 || vec.x >= map.width || vec.y < 0 || vec.y >= map.height) continue;

                img_x = img.width  * (vec.x - (int)(vec.x));
                img_y = img.height * (vec.y - (int)(vec.y));

                color = img_pixels[img_x + img_y * img.width];
                set_rgba_alpha(color, visibility_fall_off(curr_len));
                multiply_value_rgba(color, brightness_fall_off(curr_len));
                for (int k = 0; k < set_size; ++k) {
                    pixels[i + k + j_floor] = color;
                    pixels[i + k + j_ceil]  = color;
                }
            }
        }
        j_ceil += width;
        j_floor -= width;
        repeat = !repeat;
    }

    SDL_UnlockTexture(ceiling_floor_texture);
    SDL_RenderCopy(scene, ceiling_floor_texture, NULL, NULL);
}

void Player::update() {
    n = angle_to_vec(curr_angle);
    draw_floor_and_ceiling();
    draw_walls();
}

Uint32 color_to_rgba(const Color& color)  {
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Uint32 result = (color.a << 24) + (color.b << 16) + (color.g << 8) + color.r;
    #else
    Uint32 result = (color.r << 24) + (color.g << 16) + (color.b << 8) + color.a;
    #endif
    return result;
}
void set_rgba_alpha(Uint32& color, float val) {
    if (val > 1) val = 1;
    else if (val < 0) val = 0;
    color = (Uint32)((color >> 24) * val) << 24 | (color & 0xffffff);
}
void multiply_value_rgba(Uint32& color, float val) {
    if (val > 1) val = 1;
    else if (val < 0) val = 0;
    color = (color & 0xff000000) | ((unsigned char)(((color >> 16) & 0xff) * val) << 16) | ((unsigned char)(((color >> 8) & 0xff) * val) << 8) | (unsigned char)((color& 0xff) * val);
}
