#pragma once
#include "config.h"
#include <SDL_render.h>

namespace Beep
{
    namespace App
    {
        SDL_Renderer *render();
        bool run(const Config &config);
    }
}
