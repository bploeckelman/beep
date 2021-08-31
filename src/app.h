#pragma once
#include "config.h"
#include "game.h"

namespace BeepBoop
{
    namespace App
    {
        const Config& get_config();

        bool run(const Config &config, Game& game);
        void exit();

        void* gl_context_create();
        void gl_context_destroy(void* context);
        void gl_context_attach_to_window(void* context);

        const char* get_base_path();
        const char* get_working_dir();
        std::string file_read_as_string(const char* path);

    }
}
