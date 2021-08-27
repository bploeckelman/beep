#include "app.h"
#include <SDL.h>
#include <map>

using namespace Beep;

namespace
{
    Config app_config;
    bool app_is_running = false;
    bool app_is_exiting = false;

    std::map<int, bool> keyboard;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
}

SDL_Renderer *App::render() {
    return renderer;
}

bool App::run(const Config& config)
{
    printf("run\n");

    app_config = config;
    app_is_running = true;
    app_is_exiting = false;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to initialize sdl: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("beep",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
            app_config.width,
            app_config.height,
            SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        fprintf(stderr, "failed to create sdl window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        fprintf(stderr, "failed to create sdl rendererer: %s\n", SDL_GetError());
        return false;
    }

    app_config.on_startup();

    while (app_is_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:    app_is_running = false;
                case SDL_KEYDOWN: keyboard[event.key.keysym.sym] = false; break;
                case SDL_KEYUP:   keyboard[event.key.keysym.sym] = true;  break;
            }
        }

        if (keyboard[SDLK_ESCAPE] || keyboard[SDLK_q]) {
            app_is_running = false;
        }

        app_config.on_update();

        SDL_SetRenderDrawColor(renderer, 0, 191, 255, 1);
        SDL_RenderClear(renderer);
        app_config.on_render();
        SDL_RenderPresent(renderer);
    }

    app_is_exiting = true;
    app_config.on_shutdown();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("end");
    return true;
}
