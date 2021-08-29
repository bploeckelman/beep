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

using namespace BeepBoop;

namespace
{
    Config app_config;
    bool app_is_running = false;
    bool app_is_exiting = false;

    std::map<int, bool> keyboard;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
}

SDL_Renderer *App::render() {
    return renderer;
}

bool App::run(const Config& config)
{
    printf("run\n");

    app_config = config;
    app_is_running = true;
    app_is_exiting = false;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to initialize sdl: %s\n", SDL_GetError());
        return false;
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("beep",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            app_config.width,
            app_config.height,
            window_flags);
    if (window == nullptr) {
        fprintf(stderr, "failed to create sdl window: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        fprintf(stderr, "failed to create opengl context: %s\n", SDL_GetError());
        return false;
    }
    SDL_GL_MakeCurrent(window, context);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        fprintf(stderr, "failed to create sdl rendererer: %s\n", SDL_GetError());
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    app_config.on_startup();

    while (app_is_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:    app_is_running = false;
                case SDL_KEYDOWN: keyboard[event.key.keysym.sym] = false; break;
                case SDL_KEYUP:   keyboard[event.key.keysym.sym] = true;  break;
            }
        }

        if (keyboard[SDLK_ESCAPE] || keyboard[SDLK_q]) {
            app_is_running = false;
        }

        app_config.on_update();

        // TODO: replace SDL_Renderer calls with big boy opengl stuff so we can draw imgui on top
        {
            SDL_SetRenderDrawColor(renderer, 0, 191, 255, 1);
            SDL_RenderClear(renderer);

            app_config.on_render();

            SDL_RenderPresent(renderer);
        }

        // TODO: new hotness imgui stuff, put app_config.on_render() between glClear and ImGui::Render
//        {
//            ImGui_ImplOpenGL3_NewFrame();
//            ImGui_ImplSDL2_NewFrame(window);
//            ImGui::NewFrame();
//
//            // populate imgui frame or something?
//
//            // TODO: this stuff is probably gonna trash the above stuff, have to go all opengl all the time
//            glClearColor(0, 191.f / 255.f, 1, 1);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//            ImGui::Render();
//            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//            SDL_GL_SwapWindow(window);
//        }
    }

    app_is_exiting = true;
    app_config.on_shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("end");
    return true;
}
