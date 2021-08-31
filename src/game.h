#pragma once
#include "common.h"

namespace BeepBoop
{
    struct Game
    {
        struct State
        {
            float angle;
        };
        State state;

        void startup();
        void shutdown();
        void update();
        void render();
    };
}
