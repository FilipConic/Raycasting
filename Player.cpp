#include "Player.h"

#include <cassert>
#include <numbers>
#include <iostream>
#include <cmath>
#include <cwchar>

using namespace std;

#define assertm(condition, msg) assert(((void)msg, condition));

const float PI = numbers::pi_v<float>;
const float DOUBLE_PI = PI * 2.f;
const float HALF_PI = PI / 2.f;

//////////////////////////////
// Player class definitions //
//////////////////////////////

Player::Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map) : scene(_scene), images(_image_loader), map(_map), pos(0, 0), ceiling_floor_texture(nullptr) {}
Player::Player(const Scene& _scene, const ImageLoader& _image_loader, const Map& _map, float _x, float _y, int pixelation, int _FOV) : scene(_scene), images(_image_loader), map(_map), pos(_x, _y), curr_angle(0), FOV(_FOV * PI / 180.f), num_pixels(pixelation), ray_num(scene.get_width() / pixelation) {
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
    if (movement_vec.magnitude_squared() > 0) movement_vec.normalize() *= speed;

    Vec2<float> new_pos    = pos + movement_vec;    
    Vec2<int> new_pos_int  = new_pos;
    Vec2<int> top_left     = map_vec(pos, [](float x)->float{return floor(x - 0.5);}).max({0, 0});
    Vec2<int> bottom_right = map_vec(pos, [](float x)->float{return floor(x + 0.5);}).min({map.width - 1.f, map.height - 1.f});

    auto magn = [](float x, float y) -> float { return x * x + y * y; };
    float size_squared = SIZE * SIZE - 0.05f;
    if (movement_vec.x > 0) {
        if (map.cells[bottom_right.x + new_pos_int.y * map.width] & WALL_MASK) {
            movement_vec.x = 0.f;
        } else if (map.cells[bottom_right.x + top_left.y * map.width] & WALL_MASK) {
            if (magn(pos.x - bottom_right.x, pos.y - new_pos_int.y) < size_squared) movement_vec.x = 0.f;
        } else if (map.cells[bottom_right.x + bottom_right.y * map.width] & WALL_MASK) {
            if (magn(pos.x - bottom_right.x, pos.y - bottom_right.y) < size_squared) movement_vec.x = 0.f;
        }
    } else if (movement_vec.x <= 0) {
        if (map.cells[top_left.x + new_pos_int.y * map.width] & WALL_MASK) {
            movement_vec.x = 0.f;
        } else if (map.cells[top_left.x + top_left.y * map.width] & WALL_MASK) {
            if (magn(pos.x - new_pos_int.x, pos.y - new_pos_int.y) < size_squared) movement_vec.x = 0.f; 
        } else if (map.cells[top_left.x + bottom_right.y * map.width] & WALL_MASK) {
            if (magn(pos.x - new_pos_int.x, pos.y - bottom_right.y) < size_squared) movement_vec.x = 0.f;
        }
    }
    if (movement_vec.y > 0) {
        if (map.cells[new_pos_int.x + bottom_right.y * map.width] & WALL_MASK) {
            movement_vec.y = 0.f;
        } else if (map.cells[top_left.x + bottom_right.y * map.width] & WALL_MASK) {
            if (magn(pos.x - new_pos_int.x, pos.y - bottom_right.y) < size_squared) movement_vec.y = 0.f;
        } else if (map.cells[bottom_right.x + bottom_right.y * map.width] & WALL_MASK) {
            if (magn(pos.x - bottom_right.x, pos.y - bottom_right.y) < size_squared) movement_vec.y = 0.f;
        }
    } else if (movement_vec.y <= 0) {
        if (map.cells[new_pos_int.x + top_left.y * map.width] & WALL_MASK) {
            movement_vec.y = 0.f;
        } else if (map.cells[top_left.x + top_left.y * map.width] & WALL_MASK) {
            if (magn(pos.x - new_pos_int.x, pos.y - new_pos_int.y) < size_squared) movement_vec.y = 0.f;
        } else if (map.cells[bottom_right.x + top_left.y * map.width] & WALL_MASK) {
            if (magn(pos.x - bottom_right.x, pos.y - new_pos_int.y) < size_squared) movement_vec.y = 0.f;
        }
    }
    ///////////////////////////////////////////////////////////////////

    pos += movement_vec.min({speed, speed}).max({-speed, -speed});
    pos = pos.max({SIZE, SIZE}).min({map.width - SIZE, map.height - SIZE});

    if (mouse.left && !mouse.left_held) {
        int ix = (int)pos.x;
        int iy = (int)pos.y;
        if ((15.f * HALF_PI / 4.f < curr_angle && curr_angle <= DOUBLE_PI) || (0 < curr_angle && curr_angle <= HALF_PI / 4.f)) {
            if (pos.x < map.width - 2) {
                if (!(map.cells[ix + 1 + iy * map.width] & WALL_MASK) && (map.cells[ix + 2 + iy * map.width] & WALL_MASK)) {
                    map.cells[ix + 1 + iy * map.width] ^= map.cells[ix + 2 + iy * map.width];
                    map.cells[ix + 2 + iy * map.width] ^= map.cells[ix + 1 + iy * map.width];
                    map.cells[ix + 1 + iy * map.width] ^= map.cells[ix + 2 + iy * map.width];
                }
            }
        } else if (3.f * HALF_PI / 4.f < curr_angle && curr_angle <= 5.f * HALF_PI / 4.f) {
            if (pos.y < map.height - 2) {
                if (!(map.cells[ix + (iy + 1) * map.width] & WALL_MASK) && (map.cells[ix + (iy + 2) * map.width] & WALL_MASK)) {
                    map.cells[ix + (iy + 1) * map.width] ^= map.cells[ix + (iy + 2) * map.width];
                    map.cells[ix + (iy + 2) * map.width] ^= map.cells[ix + (iy + 1) * map.width];
                    map.cells[ix + (iy + 1) * map.width] ^= map.cells[ix + (iy + 2) * map.width];
                }
            }
        } else if (7.f * HALF_PI / 4.f < curr_angle && curr_angle <= 9.f * HALF_PI / 4.f) {
            if (pos.x > 2) {
                if (!(map.cells[ix - 1 + iy * map.width] & WALL_MASK) && (map.cells[ix - 2 + iy * map.width] & WALL_MASK)) {
                    map.cells[ix - 1 + iy * map.width] ^= map.cells[ix - 2 + iy * map.width];
                    map.cells[ix - 2 + iy * map.width] ^= map.cells[ix - 1 + iy * map.width];
                    map.cells[ix - 1 + iy * map.width] ^= map.cells[ix - 2 + iy * map.width];
                }
            }
        } else if (11.f * HALF_PI / 4.f < curr_angle && curr_angle <= 13.f * HALF_PI / 4.f) {
            if (pos.y > 2) {
                if (!(map.cells[ix + (iy - 1) * map.width] & WALL_MASK) && (map.cells[ix + (iy - 2) * map.width] & WALL_MASK)) {
                    map.cells[ix + (iy - 1) * map.width] ^= map.cells[ix + (iy - 2) * map.width];
                    map.cells[ix + (iy - 2) * map.width] ^= map.cells[ix + (iy - 1) * map.width];
                    map.cells[ix + (iy - 1) * map.width] ^= map.cells[ix + (iy - 2) * map.width];
                }
            }
        }
    }
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
    unsigned hit_color;

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
            while (!(hit_color & WALL_MASK)) {
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
        ray.hit_side = curr_is_x ? (x_side ? 0 : 1) : (y_side ? 2 : 3);
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
            switch (curr_ray.element & WALL_MASK) {
            case Map::M_BRICK:  img = images.get_image(1); break;
            case Map::M_GRAVEL: img = images.get_image(2); break;
            case Map::M_COBBLE: img = images.get_image(3); break;
            default:            img = images.get_image(0); break;
            }
            image_rect = {
                curr_ray.hit_side == 1 || curr_ray.hit_side == 2 ? (int)(curr_ray.in_block_pos * img.width) : (int)((1 - curr_ray.in_block_pos) * img.width),
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
            color = map_element_to_color((Map::MapElement)(curr_ray.element & WALL_MASK));
            color *= brightness_fall_off(dist);
            color.alpha(visibility_fall_off(dist));
            scene.draw_rect(rect, color);
        }
    }
}
void Player::draw_floor_and_ceiling() {
    auto visibility_fall_off = [](float x) -> float { return 1.f - 1.f / (1.f + exp(2.5f*(10.f - x))); };
    auto brightness_fall_off = [](float x) -> float { return -x / 20.f + 1.f; };

    const int width = scene.get_width();
    const int height = scene.get_height();

    int img_x, img_y;
    float curr_len = 1;

    Vec2<float> left  = angle_to_vec(curr_angle - FOV / 2.f);
    Vec2<float> vec, curr_left, curr_right;

    int pitch;
    void* void_pixels;
    SDL_LockTexture(ceiling_floor_texture, NULL, &void_pixels, &pitch);
    Uint32* pixels = (Uint32*)void_pixels;
    memset(void_pixels, 0, pitch * height);

    const float fix_val = 2.f * cos(FOV / 2.f) - .03f;
    const int end = height * width / 2;

    Uint32 floor_color, ceil_color;

    bool repeat = false;

    Image img;
    Uint32* img_pixels;
    auto get_image_from_element = [&](unsigned char el) -> void {
        switch (el) {
        case Map::M_BRICK:  img = images.get_image(1); break;
        case Map::M_GRAVEL: img = images.get_image(2); break;
        case Map::M_COBBLE: img = images.get_image(3); break;
        default:            img = images.get_image(0); break;
        }
        img_pixels = (Uint32*)img.surface->pixels;
    };

    unsigned map_floor, map_ceil;
    int j_floor = (height - 1) * width;
    for (int j_ceil = 0; j_ceil < end;) {
        if (repeat) {
            memcpy(&(pixels[j_ceil ]), &(pixels[j_ceil  - width]), pitch);
            memcpy(&(pixels[j_floor]), &(pixels[j_floor + width]), pitch);
        } else {
            curr_len   = fix_val / (1.f - (float)j_ceil / (end));
            curr_left  = left * curr_len;
            (curr_right = curr_left).rotate(FOV);

            for (int i = 0; i < width; i += num_pixels) {
                vec = lerp(curr_right, curr_left, (float)i / (width - 1.f)) + pos;
                if (vec.x < 0 || vec.x >= map.width || vec.y < 0 || vec.y >= map.height) continue;
                
                map_floor = map.cells[(int)(vec.x) + (int)(vec.y) * map.width];
                map_ceil = (map_floor & CEILING_MASK) >> 16;
                (map_floor &= FLOOR_MASK) >>= 8;
                
                if (!map_floor) {
                    floor_color = 0;
                } else if (map_floor & 0xC0) {
                    get_image_from_element(map_floor);
                    img_x = img.width  * (vec.x - floor(vec.x));
                    img_y = img.height * (vec.y - floor(vec.y));

                    floor_color = img_pixels[img_x + img_y * img.surface->w];
                } else {
                    floor_color = color_to_abgr(map_element_to_color((Map::MapElement)map_floor));
                }

                if (!map_ceil) {
                    ceil_color = 0;
                } else if (map_ceil & 0xC0) {
                    get_image_from_element(map_ceil);
                    img_x = img.width  * (vec.x - floor(vec.x));
                    img_y = img.height * (vec.y - floor(vec.y));

                    ceil_color = img_pixels[img_x + img_y * img.width];
                } else {
                    ceil_color = color_to_abgr(map_element_to_color((Map::MapElement)map_ceil));
                }

                if (map_floor) {
                    set_rgba_alpha(floor_color, visibility_fall_off(curr_len));
                    multiply_value_rgba(floor_color, brightness_fall_off(curr_len));
                }
                if (map_ceil) {
                    set_rgba_alpha(ceil_color, visibility_fall_off(curr_len));
                    multiply_value_rgba(ceil_color, brightness_fall_off(curr_len));
                }

                for (int k = 0; k < num_pixels; ++k) {
                    pixels[i + k + j_floor] = floor_color;
                    pixels[i + k + j_ceil]  = ceil_color;
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

void Player::update(float dt, const Keyboard& keyboard, Mouse& mouse) {
    n = angle_to_vec(curr_angle);
    draw_floor_and_ceiling();
    draw_walls();
    move(dt, keyboard, mouse);
}

Uint32 color_to_abgr(const Color& color)  {
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
