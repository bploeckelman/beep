#include <GL/gl3w.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "graphics.h"
#include "app.h"

using namespace BeepBoop;

namespace
{

    void *gl_context;

    // todo - these gl resources should be in Game
    ui32 vao, vbo, ebo;
    ui32 shader_program;
    ui32 texture;
    void create_resources();
    void destroy_resources();

    float vertices[] = {
            // positions          // colors           // texture coords
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // bottom right
             0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // top right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // top left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // bottom left
    };
    ui32 indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    void check_shader_compilation(ui32 shader);
    void check_shader_linkage(ui32 program);

}

void Graphics::startup()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    gl_context = App::gl_context_create();
    App::gl_context_attach_to_window(gl_context);

    if (gl3wInit())
    {
        fprintf(stderr, "failed to initialize gl3w\n");
        App::exit();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    stbi_set_flip_vertically_on_load(true);

    create_resources();
}

void Graphics::shutdown()
{
    destroy_resources();

    App::gl_context_destroy(gl_context);
}

void Graphics::render()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void* Graphics::get_gl_context()
{
    return gl_context;
}

// ----------------------------------------------------------------------------

namespace
{
    void create_shader()
    {
        const char *vertex_shader_source = ""
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
        ui32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);
        check_shader_compilation(vertex_shader);

        const char *fragment_shader_source = ""
                "#version 330 core\n"
                "out vec4 FragColor;\n"
                "in vec3 OurColor;\n"
                "in vec2 TexCoord;\n"
                "uniform sampler2D texture1;\n"
                "void main()\n"
                "{\n"
                "  FragColor = texture(texture1, TexCoord);\n"// * vec4(OurColor, 1.0);\n"
                "}\0";
        ui32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);
        check_shader_compilation(fragment_shader);

        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        check_shader_linkage(shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    void create_objects()
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

    void create_texture()
    {
        const char *filename = "test.png";
        printf("loading '%s'... ", filename);
        {
            int width, height, numChannels;
            unsigned char *data = stbi_load(filename, &width, &height, &numChannels, 0);
            if (data == nullptr)
            {
                fprintf(stderr, "failed to load texture data for '%s'\n", filename);
                App::exit();
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

    void create_resources()
    {
        create_shader();
        create_objects();
        create_texture();
    }

    void destroy_resources()
    {
        glDeleteTextures(1, &texture);
        glDeleteProgram(shader_program);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void check_shader_compilation(ui32 shader)
    {
        i32 success;
        char log[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, log);
            fprintf(stderr, "failed to compile shader: %s\n", log);
            App::exit();
        }
    }

    void check_shader_linkage(ui32 program)
    {
        i32 success;
        char log[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 512, nullptr, log);
            fprintf(stderr, "failed to link shader program: %s\n", log);
            App::exit();
        }
    }

}
