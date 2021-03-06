cmake_minimum_required(VERSION 3.20)
project(beep)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# configure and include SDL to build from git submodule sources in libs/sdl
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/sdl)

# Configure and include imgui to build from git submodule sources in libs/imgui
# Note
add_library(imgui
        libs/imgui/imgui.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_sdl.cpp
        libs/imgui/backends/imgui_impl_opengl3.cpp
        )
# Make sure the backend source files can find required headers
target_include_directories(imgui PRIVATE
        libs/imgui
        libs/sdl/include
        )

# Configure and include gl3w to wrangle opengl extensions for us
add_subdirectory(libs/gl3w)

# Configure and include glm for opengl math data types and operations
add_subdirectory(libs/glm)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/common.h
        src/config.h
        src/app.h
        src/app.cpp
        src/game.h
        src/game.cpp
        src/graphics.h
        src/graphics.cpp
        )

include_directories(${PROJECT_NAME} PRIVATE
        libs/sdl/include
        libs/stb
        libs/imgui
        libs/imgui/backends
        libs/glm
        )

target_link_libraries(${PROJECT_NAME}
        opengl32
        gl3w
        SDL2main SDL2-static
        imgui
        )

## NOTE: no longer need this stuff since we're building SDL with static linkage
## copy SDL2 dll to the build directory
#set(SDL2_DLL "${PROJECT_BINARY_DIR}/libs/sdl/SDL2d.dll")
#if (EXISTS ${SDL2_DLL})
#    add_custom_command(
#            TARGET beep POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy
#                ${SDL2_DLL} $<TARGET_FILE_DIR:beep>)
#endif()
