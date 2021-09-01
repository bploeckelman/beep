#pragma once
#include <glm/glm.hpp>
#include <string>
#include "common.h"

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

    struct ShaderSource
    {
        std::string vertex;
        std::string fragment;
    };
    enum class UniformType
    {
          none
        , float1
        , float2
        , float3
        , float4
        , mat3x2
        , mat4x4
        , texture2d
        , sampler2d
    };
    struct UniformInfo
    {
        std::string name;
        UniformType type;
        ui32 location;
    };
    struct Shader
    {
        ui32 gl_id;
        ShaderSource source;
        std::vector<UniformInfo> uniforms;

        void use();
        void stop();

        void set_int(const char* uniform_name, int value);
        void set_float(const char* uniform_name, float value);
        void set_vec3(const char* uniform_name, const float* values);
        void set_vec4(const char* uniform_name, const float* values);
        void set_mat3(const char* uniform_name, const float* values);
        void set_mat4(const char* uniform_name, const float* values);
    };
    using ShaderRef = Ref<Shader>;

    struct Camera
    {
        ui32 id;

        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;

        float fov;
        float aspect;
        float near_dist;
        float far_dist;

        glm::mat4 projection;
        glm::mat4 view;

        void set(const glm::vec3& position, const glm::vec3& target);
        void set(float aspect, float angle, float near_dist, float far_dist);

        static ui32 camera_id;
        Camera();

        void update();
    };



    namespace Graphics
    {
        ShaderRef create_shader(const ShaderSource& source);
        void destroy_shader(ShaderRef shader);

        TextureRef create_texture(const char* filename);
        void destroy_texture(TextureRef texture);

        MeshRef create_mesh();
        void destroy_mesh(MeshRef mesh);
        void draw_mesh(glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model, MeshRef mesh, TextureRef texture,
                       ShaderRef shader);
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
