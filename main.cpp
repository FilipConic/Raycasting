#include <iostream>
#include "Player.h"
#include "SDL_Components\Surface.h"
#include <numbers>

using namespace std;

int main(int argc, char** argv) {
    argc = (int)argc; argv = (char**)argv;

    Scene s("my sdl game", 1600, 900);
    Keyboard k;
    Mouse m;

    ImageLoader il(s, {
    ".\\images\\glitch_texture.png",
    ".\\images\\bricks.png",
    ".\\images\\gravel.png",
    ".\\images\\cobble_path.png"
    });
    
    Vec2<int> translation = {50, 50};
    Map map(s, "map.txt");
    Player p(s, il, map, 9.5f, 6.5f, 200, 90);

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
        p.draw_floor_and_ceiling();
        p.draw_walls();

        if (draw_map) {
            map.draw(250, translation);
            p.draw(translation);
        }
        
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT: game_running = false; break;
            case SDL_KEYDOWN: if (k[SDL_SCANCODE_M]) draw_map = !draw_map; break;
            }
        }

        if (k[SDL_SCANCODE_ESCAPE])
            game_running = false;

        s.clear(DARK_GRAY);
    }

    return 0;
}