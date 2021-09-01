#include "game.h"
#include "app.h"
#include "graphics.h"

#include <glm/ext/matrix_transform.hpp>

using namespace BeepBoop;

namespace
{
    glm::mat4 transform;
    MeshRef test_mesh;
    TextureRef test_texture;
    ShaderRef test_shader;

    void create_resources();
    void destroy_resources();
}

Game::State Game::state;

void Game::startup()
{
    printf("game startup\n");

    auto cfg = App::get_config();
    Graphics::set_viewport(0, 0, cfg.width, cfg.height);

    Game::state.angle = 0.f;
    Game::state.camera = Camera();

    create_resources();
}

void Game::shutdown()
{
    printf("game shutdown\n");

    destroy_resources();
}

void Game::update()
{
    transform = glm::translate(
            glm::rotate(
                    glm::mat4(1), state.angle, glm::vec3(0, 0, 1)),
                    glm::vec3(0, 0, -5));

    state.camera.update();
}

void Game::render()
{
    Graphics::draw_mesh(
            state.camera.projection,
            state.camera.view,
            transform,
            test_mesh,
            test_texture,
            test_shader);
}

namespace
{
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
    void create_resources()
    {
        ShaderSource shader_source = {
                App::file_read_as_string("resources/shaders/default.vert"),
                App::file_read_as_string("resources/shaders/default.frag")
        };
        test_shader = Graphics::create_shader(shader_source);

        test_texture = Graphics::create_texture("resources/images/test.png");

        test_mesh = Graphics::create_mesh();
        Graphics::mesh_set_vertices(test_mesh, vertices, 4);
        Graphics::mesh_set_indices(test_mesh, indices, 6);

        transform = glm::mat4(1);
    }

    void destroy_resources()
    {
        Graphics::destroy_mesh(test_mesh);
        Graphics::destroy_texture(test_texture);
        Graphics::destroy_shader(test_shader);
    }
}
