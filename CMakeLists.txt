cmake_minimum_required(VERSION 3.30)
project(simple_audio_player)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

file(COPY Music DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(OpenGL REQUIRED)

add_subdirectory(Third/glfw)
add_subdirectory(Third/imgui)
add_subdirectory(Third/miniaudio)

add_executable(simple_audio_player main.cpp
        Window.cpp
        Window.h
        Third/miniaudio/miniaudio.cpp)

target_link_libraries(simple_audio_player PRIVATE imgui miniaudio OpenGL::GL)
