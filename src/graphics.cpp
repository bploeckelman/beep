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
    ui32 shader_program;
    void create_resources();
    void destroy_resources();

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

void Graphics::update()
{

}

void Graphics::render()
{

}

void* Graphics::get_gl_context()
{
    return gl_context;
}

void Graphics::set_viewport(i32 x, i32 y, i32 width, i32 height)
{
    glViewport(x, y, width, height);
}

// ----------------------------------------------------------------------------

TextureRef Graphics::create_texture(const char *filename)
{
    auto texture = new Texture();
    printf("loading '%s'... ", filename);
    {
        texture->filename = filename;

        int num_channels;
        unsigned char* data = stbi_load(texture->filename.c_str(), &texture->width, &texture->height, &num_channels, 0);
        if (data == nullptr)
        {
            fprintf(stderr, "failed to load texture data for '%s'\n", texture->filename.c_str());
            delete texture;
            return TextureRef();
        }

        glGenTextures(1, &texture->gl_id);
        if (texture->gl_id == 0)
        {
            fprintf(stderr, "failed to generate gl texture id for '%s'\n", texture->filename.c_str());
            stbi_image_free(data);
            delete texture;
            return TextureRef();
        }
        glBindTexture(GL_TEXTURE_2D, texture->gl_id);
        {
            // TODO: allow specification of wrap, filter, and bpp params
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
    }
    printf("complete\n");
    return TextureRef(texture);
}

void Graphics::destroy_texture(TextureRef texture)
{
    printf("deleting texture '%s'... ", texture->filename.c_str());
    {
        glDeleteTextures(1, &texture->gl_id);
    }
    printf("complete\n");
}

// ----------------------------------------------------------------------------

MeshRef Graphics::create_mesh()
{
    Mesh* mesh = new Mesh();

    glGenVertexArrays(1, &(mesh->gl_id));
    if (mesh->gl_id <= 0)
    {
        delete mesh;
        return MeshRef();
    }

    return MeshRef(mesh);
}

void Graphics::destroy_mesh(const MeshRef mesh)
{
    if (mesh->gl_id != 0)
    {
        if (mesh->vertex_buffer != 0)
        {
            glDeleteBuffers(1, &mesh->vertex_buffer);
            mesh->vertex_buffer = 0;
            mesh->num_vertices = 0;
        }
        if (mesh->index_buffer != 0)
        {
            glDeleteBuffers(1, &mesh->index_buffer);
            mesh->index_buffer = 0;
            mesh->num_indices = 0;
        }
        glDeleteVertexArrays(1, &mesh->gl_id);
    }
    mesh->gl_id = 0;
}

void Graphics::draw_mesh(MeshRef mesh, TextureRef texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->gl_id);
    glUseProgram(shader_program);
    glBindVertexArray(mesh->gl_id);
    glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);
}

void Graphics::mesh_set_vertices(MeshRef mesh, const void *vertices, i64 count)
{
    mesh->num_vertices = count;

    glBindVertexArray(mesh->gl_id);
    {
        if (mesh->vertex_buffer == 0) {
            glGenBuffers(1, &mesh->vertex_buffer);
        }

        // NOTE: assumes vertices are floats
        auto vertex_data = (float*) vertices;
        int attribs_size = 8 * sizeof(float);

        // upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * attribs_size * sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

        // setup vertex attributes in the form: pos(3) col(3) tex(2)
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attribs_size, (void*) 0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, attribs_size, (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texcoord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, attribs_size, (void*) (6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    glBindVertexArray(0);
}

void Graphics::mesh_set_indices(MeshRef mesh, const void *indices, i64 count)
{
    mesh->num_indices = count;

    glBindVertexArray(mesh->gl_id);
    {
        if (mesh->index_buffer == 0)
        {
            glGenBuffers(1, &mesh->index_buffer);
        }

        // NOTE: assumes indices are unsigned 32 bit ints
        auto index_data = (ui32*) indices;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * sizeof(index_data), index_data, GL_STATIC_DRAW);
    }
    glBindVertexArray(0);
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
                "  FragColor = texture(texture1, TexCoord) * vec4(OurColor, 1.0);\n"
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

    }

    void create_texture()
    {

    }

    void create_resources()
    {
        create_shader();
        create_objects();
        create_texture();
    }

    void destroy_resources()
    {
        glDeleteProgram(shader_program);
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
