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

#include "app.h"
#include "graphics.h"

using namespace BeepBoop;

namespace
{
    Config app_config;
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

const Config &App::get_config()
{
    return app_config;
}

bool App::run(const Config& config)
{
    printf("run\n");

    app_config = config;

    startup();
    app_config.on_startup();

    while (!app_is_exiting)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
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

            // TODO: populate imgui frame

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

namespace
{
    void init_sdl()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "failed to initialize sdl: %s\n", SDL_GetError());
            App::exit();
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

