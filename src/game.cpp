#include "game.h"
#include "app.h"
#include <SDL_image.h>

namespace BeepBoop
{
    namespace
    {
        typedef struct Texture
        {
            const char *filename;
            SDL_Texture *tex;
            SDL_Rect rect;
        } Texture;

        Texture texture = { "test.png" };
    }

    void Game::startup()
    {
        printf("game startup\n");

        texture.tex = IMG_LoadTexture(App::render(), texture.filename);
        if (texture.tex == nullptr) {
            fprintf(stderr, "failed to load texture for image '%s'\n", texture.filename);
            SDL_Quit();
        } else {
            SDL_QueryTexture(texture.tex, nullptr, nullptr, &texture.rect.w, &texture.rect.h);
            printf("Loaded image '%s'\n", texture.filename);
        }
    }

    void Game::shutdown()
    {
        printf("game shutdown\n");

        if (texture.tex != nullptr) {
            SDL_DestroyTexture(texture.tex);
            printf("Unloaded image '%s'\n", texture.filename);
        }
    }

    void Game::update()
    {
    }

    void Game::render()
    {
        SDL_RenderCopy(App::render(), texture.tex, nullptr, &texture.rect);
    }

}
