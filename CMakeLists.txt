cmake_minimum_required(VERSION 3.20)
project(beep)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_subdirectory(libs/imgui)

add_executable(beep
        src/main.cpp
        src/common.h
        src/game.h
        src/game.cpp
        src/config.h
        src/config.cpp
        src/app.h
        src/app.cpp
        )

# copy SDL2 to the build directory
set(SDL2_DLL "" CACHE FILEPATH "SDL2 DLL Path")
if (SDL2_ENABLED)
    if (EXISTS ${SDL2_DLL})
        add_custom_command(
                TARGET game POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_DLL} $<TARGET_FILE_DIR:game>)
    endif()
endif()
