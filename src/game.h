#pragma once
#include "common.h"

namespace Beep
{
    struct Game
    {
        void startup();
        void shutdown();
        void update();
        void render();
    };
}
