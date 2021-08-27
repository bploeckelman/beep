cmake_minimum_required(VERSION 3.20)
project(beep)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBS_DIR ${PROJECT_BINARY_DIR}/libs)

# configure and include SDL to build from git submodule sources in libs/sdl
set(SDL2_INCLUDE_DIRS ${LIBS_DIR}/sdl/include)
add_subdirectory(libs/sdl)

# configure and include SDL_image to build from git submodule sources in libs/sdl_image
set(SDL2_IMAGE_INCLUDE_DIRS ${LIBS_DIR}/sdl_image)
add_subdirectory(libs/sdl_image)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/common.h
        src/config.h
        src/app.h
        src/app.cpp
        src/game.h
        src/game.cpp
        )

target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image)

## copy SDL2 dll to the build directory
set(SDL2_DLL "${PROJECT_BINARY_DIR}/libs/sdl/SDL2d.dll")
if (EXISTS ${SDL2_DLL})
    add_custom_command(
            TARGET beep POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${SDL2_DLL} $<TARGET_FILE_DIR:beep>)
endif()

## copy SDL_image dll and associated dlls to the build directory
set(SDL_IMAGE_DLL "${PROJECT_BINARY_DIR}/libs/sdl_image/SDL2_image.dll")
set(SDL_IMAGE_PNG_DLL "${PROJECT_BINARY_DIR}/libs/sdl_image/external/libpng-1.6.37/libpng16d.dll")
set(SDL_IMAGE_ZLIB_DLL "${PROJECT_BINARY_DIR}/libs/sdl_image/external/zlib-1.2.11/zlibd1.dll")
if (EXISTS ${SDL_IMAGE_DLL}
AND EXISTS ${SDL_IMAGE_PNG_DLL}
AND EXISTS ${SDL_IMAGE_ZLIB_DLL})
    add_custom_command(
            TARGET beep POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${SDL_IMAGE_DLL}
                ${SDL_IMAGE_PNG_DLL}
                ${SDL_IMAGE_ZLIB_DLL}
                $<TARGET_FILE_DIR:beep>)
endif()
