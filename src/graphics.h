#pragma once
#include "common.h"


namespace BeepBoop
{

    namespace Graphics
    {

        void startup();
        void shutdown();
        void update();
        void render();

        void* get_gl_context();

    }

}



