cmake_minimum_required(VERSION 3.20)
project(beep)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# configure and include SDL to build from git submodule sources in /lib on our dev platform
set(SDL2_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/libs/sdl/include)
add_subdirectory(libs/sdl)

#add_subdirectory(libs/imgui)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/common.h
        src/config.h
        src/app.h
        src/app.cpp
        src/game.h
        src/game.cpp
        )

target_link_libraries(${PROJECT_NAME} SDL2)

## copy SDL2 to the build directory
set(SDL2_DLL "${PROJECT_BINARY_DIR}/libs/sdl/SDL2d.dll")
if (EXISTS ${SDL2_DLL})
    set(COPY_SOURCE "${PROJECT_BINARY_DIR}/libs/sdl/SDL2d.dll")
    add_custom_command(
            TARGET beep POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${COPY_SOURCE} $<TARGET_FILE_DIR:beep>)
endif()
