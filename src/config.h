#pragma once
#include "common.h"

namespace BeepBoop
{
    struct Config
    {
        const char* name;
        int width;
        int height;

        LifecycleFunction on_startup;
        LifecycleFunction on_shutdown;
        LifecycleFunction on_update;
        LifecycleFunction on_render;
    };
}
