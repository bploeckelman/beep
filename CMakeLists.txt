cmake_minimum_required(VERSION 3.20)
project(beep)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# configure and include SDL to build from git submodule sources in libs/sdl
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/sdl)

# configure and include SDL_image to build from git submodule sources in libs/sdl_image
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/sdl_image)

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

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/common.h
        src/config.h
        src/app.h
        src/app.cpp
        src/game.h
        src/game.cpp
        )

include_directories(${PROJECT_NAME} PRIVATE
        libs/sdl/include
        libs/sdl_image
        libs/stb
        libs/imgui
        libs/imgui/backends
        )

target_link_libraries(${PROJECT_NAME}
        opengl32
        SDL2main SDL2-static
        SDL2_image
        imgui
        )

## NOTE: no longer need this stuff since we're building all the libs with static linkage

## copy SDL2 dll to the build directory
#set(SDL2_DLL "${PROJECT_BINARY_DIR}/libs/sdl/SDL2d.dll")
#if (EXISTS ${SDL2_DLL})
#    add_custom_command(
#            TARGET beep POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy
#                ${SDL2_DLL} $<TARGET_FILE_DIR:beep>)
#endif()

## copy SDL_image dll and associated dlls to the build directory
#set(SDL_IMAGE_DLL "${PROJECT_BINARY_DIR}/libs/sdl_image/SDL2_image.dll")
#set(SDL_IMAGE_PNG_DLL "${PROJECT_BINARY_DIR}/libs/sdl_image/external/libpng-1.6.37/libpng16d.dll")
#set(SDL_IMAGE_ZLIB_DLL "${PROJECT_BINARY_DIR}/libs/sdl_image/external/zlib-1.2.11/zlibd1.dll")
#if (EXISTS ${SDL_IMAGE_DLL}
#AND EXISTS ${SDL_IMAGE_PNG_DLL}
#AND EXISTS ${SDL_IMAGE_ZLIB_DLL})
#    add_custom_command(
#            TARGET beep POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy
#                ${SDL_IMAGE_DLL}
#                ${SDL_IMAGE_PNG_DLL}
#                ${SDL_IMAGE_ZLIB_DLL}
#                $<TARGET_FILE_DIR:beep>)
#endif()
