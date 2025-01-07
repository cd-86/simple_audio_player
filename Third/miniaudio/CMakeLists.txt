project(miniaudio)

add_library(miniaudio STATIC miniaudio.cpp)

target_include_directories(miniaudio PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)