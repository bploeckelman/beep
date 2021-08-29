#pragma once
#include "config.h"
#include <SDL_render.h>

namespace BeepBoop
{
    namespace App
    {
        SDL_Renderer *render();
        bool run(const Config &config);
    }
}
