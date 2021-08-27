#include <iostream>
#include "app.h"
#include "game.h"
#include "config.h"

using namespace Beep;

namespace
{
    Game game;

    void startup()
    {
        game.startup();
    }
    void shutdown()
    {
        game.shutdown();
    }
    void update()
    {
        game.update();
    }
    void render()
    {
        game.render();
    }
}

int main()
{
    printf("main\n");

    Config config;
    config.name = "Beep";
    config.width = 1280;
    config.height = 720;

    config.on_startup = startup;
    config.on_shutdown = shutdown;
    config.on_update = update;
    config.on_render = render;

    App::run(config);
    return 0;
}
