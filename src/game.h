#pragma once
#include "common.h"

namespace BeepBoop
{
    struct Game
    {
        void startup();
        void shutdown();
        void update();
        void render();
    };
}
