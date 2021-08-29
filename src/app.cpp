#include <GL/gl3w.h>
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


// TODO: move this elsewhere
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace BeepBoop;

namespace
{
    Config app_config;
    bool app_is_exiting = false;

    std::map<int, bool> keyboard;

    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;

    // --------------------------------------------------

    float vertices[] = {
            // positions          // colors           // texture coords
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   // bottom right
             0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   // top left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f    // bottom left
    };
    ui32 indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    ui32 vao, vbo, ebo;
    ui32 shaderProgram;
    ui32 texture;

    // --------------------------------------------------

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

        if (gl3wInit())
        {
            fprintf(stderr, "failed to initialize gl3w\n");
            SDL_Quit();
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // compile and link shaders
        {
            const char *vertexShaderSource = ""
                    "#version 330 core\n"
                    "layout (location = 0) in vec3 aPos;\n"
                    "layout (location = 1) in vec3 aColor;\n"
                    "layout (location = 2) in vec2 aTexCoord;\n"
                    "out vec3 OurColor;"
                    "out vec2 TexCoord;"
                    "void main()\n"
                    "{\n"
                    "  gl_Position = vec4(aPos, 1.0);\n"
                    "  OurColor = aColor;"
                    "  TexCoord = aTexCoord;"
                    "}\0";
            ui32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
            glCompileShader(vertexShader);
            opengl_check_shader_compilation(vertexShader);

            const char *fragmentShaderSource = ""
                    "#version 330 core\n"
                    "out vec4 FragColor;\n"
                    "in vec3 OurColor;\n"
                    "in vec2 TexCoord;\n"
                    "uniform sampler2D texture1;\n"
                    "void main()\n"
                    "{\n"
                    "  FragColor = texture(texture1, TexCoord);\n"// * vec4(OurColor, 1.0);\n"
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
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
            glEnableVertexAttribArray(0);
            // color attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            // texcoord attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }

        // load textures
        {
            const char *filename = "test.png";
            printf("Loading texture %s... ", filename);
            {
                int width, height, numChannels;
                unsigned char *data = stbi_load(filename, &width, &height, &numChannels, 0);
                if (data == nullptr)
                {
                    fprintf(stderr, "failed to load texture data for '%s'\n", filename);
                    SDL_Quit();
                }

                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                // set the texture wrapping parameters
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                // set texture filtering parameters
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);
            }
            printf("success!\n");
        }
    }

    void opengl_shutdown()
    {
        glDeleteTextures(1, &texture);
        glDeleteProgram(shaderProgram);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &ebo);
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
                glBindTexture(GL_TEXTURE_2D, texture);
                glUseProgram(shaderProgram);
                glBindVertexArray(vao);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
 * - shader compilation and linkage checks
 */
namespace
{

    void opengl_check_shader_compilation(ui32 shader)
    {
        i32 success;
        char log[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, log);
            fprintf(stderr, "failed to compile shader: %s\n", log);
            SDL_Quit();
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
            SDL_Quit();
        }
    }

}

