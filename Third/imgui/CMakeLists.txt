project(imgui)

#set(HRDS imgui.h imgui_internal.h)

set(SRCS
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${SRCS})

target_link_libraries(imgui PUBLIC glfw)

target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)