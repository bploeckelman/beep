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
    bool app_is_exiting = false;

    std::map<int, bool> keyboard;

    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;

    // --------------------------------------------------

    PFNGLGENBUFFERSPROC              glGenBuffers;
    PFNGLBINDBUFFERPROC              glBindBuffer;
    PFNGLBUFFERDATAPROC              glBufferData;
    PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
    PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
    PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
    PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
    PFNGLCREATESHADERPROC            glCreateShader;
    PFNGLSHADERSOURCEPROC            glShaderSource;
    PFNGLCOMPILESHADERPROC           glCompileShader;
    PFNGLDELETESHADERPROC            glDeleteShader;
    PFNGLGETSHADERIVPROC             glGetShaderiv;
    PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
    PFNGLCREATEPROGRAMPROC           glCreateProgram;
    PFNGLDELETEPROGRAMPROC           glDeleteProgram;
    PFNGLATTACHSHADERPROC            glAttachShader;
    PFNGLLINKPROGRAMPROC             glLinkProgram;
    PFNGLUSEPROGRAMPROC              glUseProgram;
    PFNGLGETPROGRAMIVPROC            glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
    PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

    ui32 shaderProgram;
    ui32 vao;
    ui32 vbo;
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
    };

    // --------------------------------------------------

    void opengl_get_extensions();
    void opengl_check_shader_compilation(ui32 shader);
    void opengl_check_program_linkage(ui32 program);

    void opengl_startup()
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        context = SDL_GL_CreateContext(window);
        if (context == nullptr) {
            fprintf(stderr, "failed to create opengl context: %s\n", SDL_GetError());
            SDL_Quit();
        }
        SDL_GL_MakeCurrent(window, context);

        opengl_get_extensions();

        // compile and link shaders
        {
            const char *vertexShaderSource = ""
                    "#version 330 core\n"
                    "layout (location = 0) in vec3 aPos;\n"
                    "void main()\n"
                    "{\n"
                    "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                    "}\0";
            ui32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
            glCompileShader(vertexShader);
            opengl_check_shader_compilation(vertexShader);

            const char *fragmentShaderSource = ""
                    "#version 330 core\n"
                    "out vec4 FragColor;\n"
                    "void main()\n"
                    "{\n"
                    "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                    "}\0";
            ui32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
            glCompileShader(fragmentShader);
            opengl_check_shader_compilation(fragmentShader);

            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);
            opengl_check_program_linkage(shaderProgram);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }

        // build vbo and vao
        {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
            glEnableVertexAttribArray(0);
        }
    }

    void opengl_shutdown()
    {
        glDeleteProgram(shaderProgram);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    // --------------------------------------------------

    void imgui_startup()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init();
    }

    void imgui_shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}

const Config &App::config()
{
    return app_config;
}

bool App::run(const Config& config)
{
    printf("run\n");

    app_config = config;
    app_is_exiting = false;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "failed to initialize sdl: %s\n", SDL_GetError());
        return false;
    }

    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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

    opengl_startup();
    imgui_startup();

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

        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            // TODO: populate imgui frame

            glClearColor(0, 191.f / 255.f, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            app_config.on_render();
            {
                glUseProgram(shaderProgram);
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);
        }
    }

    app_config.on_shutdown();

    imgui_shutdown();
    opengl_shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("end");
    return true;
}

/**
 * OpenGL utility functions
 * - extension address lookup (TODO: should probably just use GLEW)
 * - shader compilation and linkage checks
 */
namespace
{
    void *opengl_get_extension(const char *name)
    {
        void *proc = SDL_GL_GetProcAddress(name);
        if (proc == nullptr)
        {
            fprintf(stderr, "failed to find address for %s: %s\n", name, SDL_GetError());
            SDL_Quit();
        }
        return proc;
    }

    void opengl_get_extensions()
    {
        glGenBuffers              = (PFNGLGENBUFFERSPROC)              opengl_get_extension("glGenBuffers");
        glBindBuffer              = (PFNGLBINDBUFFERPROC)              opengl_get_extension("glBindBuffer");
        glBufferData              = (PFNGLBUFFERDATAPROC)              opengl_get_extension("glBufferData");
        glDeleteBuffers           = (PFNGLDELETEBUFFERSPROC)           opengl_get_extension("glDeleteBuffers");
        glGenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC)         opengl_get_extension("glGenVertexArrays");
        glBindVertexArray         = (PFNGLBINDVERTEXARRAYPROC)         opengl_get_extension("glBindVertexArray");
        glDeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC)      opengl_get_extension("glDeleteVertexArrays");
        glCreateShader            = (PFNGLCREATESHADERPROC)            opengl_get_extension("glCreateShader");
        glShaderSource            = (PFNGLSHADERSOURCEPROC)            opengl_get_extension("glShaderSource");
        glCompileShader           = (PFNGLCOMPILESHADERPROC)           opengl_get_extension("glCompileShader");
        glDeleteShader            = (PFNGLDELETESHADERPROC)            opengl_get_extension("glDeleteShader");
        glGetShaderiv             = (PFNGLGETSHADERIVPROC)             opengl_get_extension("glGetShaderiv");
        glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)        opengl_get_extension("glGetShaderInfoLog");
        glCreateProgram           = (PFNGLCREATEPROGRAMPROC)           opengl_get_extension("glCreateProgram");
        glDeleteProgram           = (PFNGLDELETEPROGRAMPROC)           opengl_get_extension("glDeleteProgram");
        glAttachShader            = (PFNGLATTACHSHADERPROC)            opengl_get_extension("glAttachShader");
        glLinkProgram             = (PFNGLLINKPROGRAMPROC)             opengl_get_extension("glLinkProgram");
        glUseProgram              = (PFNGLUSEPROGRAMPROC)              opengl_get_extension("glUseProgram");
        glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)            opengl_get_extension("glGetProgramiv");
        glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)       opengl_get_extension("glGetProgramInfoLog");
        glVertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC)     opengl_get_extension("glVertexAttribPointer");
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) opengl_get_extension("glEnableVertexAttribArray");
    }

    void opengl_check_shader_compilation(ui32 shader)
    {
        i32 success;
        char log[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, log);
            fprintf(stderr, "failed to compile shader: %s\n", log);
        }
    }

    void opengl_check_program_linkage(ui32 program)
    {
        i32 success;
        char log[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, nullptr, log);
            fprintf(stderr, "failed to link shader program: %s\n", log);
        }
    }

}

