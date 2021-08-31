#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include <cstdio>
#include <map>
#include <string>
#include <filesystem>

#include "app.h"
#include "graphics.h"
#include "game.h"

using namespace BeepBoop;
namespace fs = std::filesystem;

namespace
{
    Config app_config;
    std::string app_base_path;
    std::string app_cwd_path;
    bool app_is_exiting;

    std::map<int, bool> keyboard;

    SDL_Window* window = nullptr;

    // --------------------------------------------------

    void init_sdl();
    void create_window();
    void imgui_startup();
    void imgui_shutdown();

    void startup()
    {
        app_is_exiting = false;

        init_sdl();
        create_window();

        Graphics::startup();
        imgui_startup();
    }
}

// ----------------------------------------------------------------------------

const Config& App::get_config()
{
    return app_config;
}

bool App::run(const Config& config, Game& game)
{
    printf("run\n");

    app_config = config;

    startup();
    app_config.on_startup();

    while (!app_is_exiting)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
                case SDL_QUIT:    app_is_exiting = true;
                case SDL_KEYDOWN: keyboard[event.key.keysym.sym] = false; break;
                case SDL_KEYUP:   keyboard[event.key.keysym.sym] = true;  break;
            }
        }

        if (keyboard[SDLK_ESCAPE] || keyboard[SDLK_q]) {
            app_is_exiting = true;
        }

        app_config.on_update();
        Graphics::update();

        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();
            {
                // populate imgui frame
                ImGui::Begin("beep");
                ImGui::SliderAngle("angle", &game.state.angle);
                ImGui::End();
            }

            glClearColor(0, 191.f / 255.f, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            app_config.on_render();
            Graphics::render();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);
        }
    }

    app_config.on_shutdown();

    imgui_shutdown();
    Graphics::shutdown();

    SDL_DestroyWindow(window);

    App::exit();
    return true;
}

void App::exit()
{
    printf("exit");
    SDL_Quit();
}

// ----------------------------------------------------------------------------

void* App::gl_context_create()
{
    void* context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        fprintf(stderr, "failed to create opengl context: %s\n", SDL_GetError());
        SDL_Quit();
    }
    return context;
}

void App::gl_context_destroy(void* context)
{
    SDL_GL_DeleteContext(context);
}

void App::gl_context_attach_to_window(void *context)
{
    SDL_GL_MakeCurrent(window, context);
}

// ----------------------------------------------------------------------------

const char* App::get_base_path()
{
    if (app_base_path.empty())
    {
        app_base_path = SDL_GetBasePath();
        printf("base path: %s\n", app_base_path.c_str());
    }
    return app_base_path.c_str();
}

const char* App::get_working_dir()
{
    if (app_cwd_path.empty())
    {
        auto path = fs::current_path();
        app_cwd_path = path.u8string();
        printf("working path: %s\n", app_cwd_path.c_str());
    }
    return app_cwd_path.c_str();
}

// ----------------------------------------------------------------------------

std::string App::file_read_as_string(const char *path)
{
    printf("reading '%s'... ", path);

    auto sdl_mode = "rt";
    auto context = SDL_RWFromFile(path, sdl_mode);
    if (context == nullptr)
    {
        fprintf(stderr, "failed to open file '%s' for reading\n", path);
        App::exit();
    }

    std::string data;
    size_t length = SDL_RWsize(context) + 1;
    char* buffer = new char[length];
    {
        size_t read = SDL_RWread(context, buffer, sizeof(char), length);
        if (read == 0)
        {
            fprintf(stderr, "failed to read from file '%s': %s\n", path, SDL_GetError());
            App::exit();
        }

        SDL_RWclose(context);
        printf("complete (%zu bytes)\n", read);

        buffer[length - 1] = '\0';
        data = std::string(buffer);
    }
    delete[] buffer;
    return data;
}

// ----------------------------------------------------------------------------

namespace
{
    void init_sdl()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "failed to initialize sdl: %s\n", SDL_GetError());
            App::exit();
        }

        {
            App::get_base_path();
            App::get_working_dir();
        }
    }

    void create_window()
    {
        auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        window = SDL_CreateWindow("beep",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                app_config.width,
                app_config.height,
                window_flags);
        if (window == nullptr) {
            fprintf(stderr, "failed to create sdl window: %s\n", SDL_GetError());
            App::exit();
        }
    }

    void imgui_startup()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForOpenGL(window, Graphics::get_gl_context());
        ImGui_ImplOpenGL3_Init();
    }

    void imgui_shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

}
