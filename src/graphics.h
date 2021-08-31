#pragma once
#include "common.h"
#include <string>

namespace BeepBoop
{
    struct Mesh
    {
        ui32 gl_id;

        ui32 vertex_buffer;
        ui64 num_vertices;

        ui32 index_buffer;
        ui64 num_indices;
    };
    using MeshRef = Ref<Mesh>;

    struct Texture
    {
        std::string filename;
        ui32 gl_id;
        int width;
        int height;
    };
    using TextureRef = Ref<Texture>;

    namespace Graphics
    {
        TextureRef create_texture(const char* filename);
        void destroy_texture(TextureRef texture);

        MeshRef create_mesh();
        void destroy_mesh(MeshRef mesh);
        void draw_mesh(MeshRef mesh, TextureRef texture);
        void mesh_set_vertices(MeshRef mesh, const void* vertices, i64 count);
        void mesh_set_indices(MeshRef mesh, const void* indices, i64 count);

        void startup();
        void shutdown();
        void update();
        void render();

        void set_viewport(i32 x, i32 y, i32 width, i32 height);

        void* get_gl_context();

    }

}



