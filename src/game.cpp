#include "game.h"
#include <SDL_image.h>

namespace Beep
{
    void Game::startup()
    {
        printf("game startup\n");

        const char *filename = "test.png";
        SDL_Surface *surface = IMG_Load(filename);
        if (surface == nullptr) {
            fprintf(stderr, "failed to load surface for image '%s'\n", filename);
            SDL_Quit();
        } else {
            printf("Loaded image '%s'\n", filename);
        }

        SDL_FreeSurface(surface);
    }

    void Game::shutdown()
    {
        printf("game shutdown\n");
    }

    bool enough_already_update = false;
    bool enough_already_render = false;

    void Game::update()
    {
        if (enough_already_update) {
            return;
        }
        enough_already_update = true;
        printf("game update\n");
    }

    void Game::render()
    {
        if (enough_already_render) {
            return;
        }
        enough_already_render = true;
        printf("game render\n");
    }

}
