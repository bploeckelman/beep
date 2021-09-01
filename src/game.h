#pragma once
#include "common.h"
#include "graphics.h"

namespace BeepBoop
{
    struct Game
    {
        struct State
        {
            float angle;
            Camera camera;
        };
        static State state;

        void startup();
        void shutdown();
        void update();
        void render();
    };
}
