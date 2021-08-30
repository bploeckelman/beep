#pragma once
#include "config.h"

namespace BeepBoop
{
    namespace App
    {
        const Config &get_config();

        bool run(const Config &config);
        void exit();

        void* gl_context_create();
        void gl_context_destroy(void* context);
        void gl_context_attach_to_window(void* context);
    }
}
