#include <iostream>
#include "Player.h"
#include <numbers>

using namespace std;

template<Number T>
ostream& operator<<(ostream& out, const Vec2<T>& vec) {
    out << vec.x << ", " << vec.y;
    return out;
}

int main(int argc, char** argv) {
    argc = (int)argc; argv = (char**)argv;

    Scene s("my sdl game", 1600, 900);
    Keyboard k;
    Mouse m;

    ImageLoader il(s, {".\\images\\glitch_texture.png", ".\\images\\bricks.png", ".\\images\\gravel.png"});
    
    Vec2<int> translation = {50, 50};
    Map map(s, "map.txt");
    Player p(s, il, map, 9.5f, 6.5f, 1600);

    SDL_Event e;
    Uint64 last_ticks, time_passed = SDL_GetTicks64();
    float dt;
    
    bool game_running = true, draw_map = true;
    while (game_running) {
        last_ticks = SDL_GetTicks64();
        dt = (last_ticks - time_passed) / 1e3f;
        time_passed = last_ticks;
       
        k.update();
        m.update();

        p.move(dt, k);
        p.draw_walls();

        if (draw_map) {
            map.draw(250, translation);
            p.draw(translation);
        }
        
        s.clear(DARK_GRAY);
        
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT: game_running = false; break;
            case SDL_KEYDOWN: if (k[SDL_SCANCODE_M]) draw_map = !draw_map; break;
            }
        }

        if (k[SDL_SCANCODE_ESCAPE])
            game_running = false;
    }

    return 0;
}