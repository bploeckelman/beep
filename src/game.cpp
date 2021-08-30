#include "game.h"
#include "app.h"
#include <SDL_opengl.h>

namespace BeepBoop
{

    void Game::startup()
    {
        printf("game startup\n");

        auto cfg = App::get_config();
        glViewport(0, 0, cfg.width, cfg.height);
    }

    void Game::shutdown()
    {
        printf("game shutdown\n");
    }

    void Game::update()
    {
    }

    void Game::render()
    {
    }

}
