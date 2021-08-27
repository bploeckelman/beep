#include "game.h"


namespace Beep
{

    void Game::startup()
    {
        printf("game startup\n");
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
