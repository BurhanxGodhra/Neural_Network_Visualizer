cmake_minimum_required(VERSION 3.16)
project(NeuralNetworkVisualizer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Include paths for Homebrew & ImGui
include_directories(
    /opt/homebrew/include
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/backends
)

link_directories(/opt/homebrew/lib)

# ImGui Source Files
set(IMGUI_SOURCES
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl2.cpp
)

add_executable(NeuralVisualizer 
    src/main.cpp
    ${IMGUI_SOURCES}
)

target_link_libraries(NeuralVisualizer 
    PRIVATE 
    glfw 
    OpenGL::GL
)
