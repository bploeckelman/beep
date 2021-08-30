#include "game.h"
#include "app.h"
#include "graphics.h"

using namespace BeepBoop;

namespace
{
    MeshRef test_mesh;

    void create_resources();
    void destroy_resources();
}

void Game::startup()
{
    printf("game startup\n");

    auto cfg = App::get_config();
    Graphics::set_viewport(0, 0, cfg.width, cfg.height);

    create_resources();
}

void Game::shutdown()
{
    printf("game shutdown\n");

    destroy_resources();
}

void Game::update()
{
}

void Game::render()
{
    Graphics::draw_mesh(test_mesh);
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
        test_mesh = Graphics::create_mesh();
        Graphics::mesh_set_vertices(test_mesh, vertices, 4);
        Graphics::mesh_set_indices(test_mesh, indices, 6);
    }

    void destroy_resources()
    {
        Graphics::destroy_mesh(test_mesh);
    }
}
