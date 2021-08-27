#include "app.h"

using namespace Beep;

namespace
{
    Config app_config;
    bool app_is_running = false;
    bool app_is_exiting = false;

}

bool App::run(const Config& config)
{
    printf("run");

    app_config = config;
    app_is_running = true;
    app_is_exiting = false;

    while (app_is_running)
    {
        char ch = getchar();
        if (ch == 'q') {
            app_is_running = false;
        }
        app_config.on_update();
        app_config.on_render();
    }

    app_is_exiting = true;

    return true;
}
