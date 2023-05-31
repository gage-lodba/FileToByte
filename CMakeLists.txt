cmake_minimum_required(VERSION 3.15)
project(FileToBye C CXX)

set(src_dir ${PROJECT_SOURCE_DIR}/source)
set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

include(scripts/glfw.cmake)
include(scripts/imgui.cmake)

file(GLOB_RECURSE src_main
    ${src_dir}/*.hpp
    ${src_dir}/*.h
    ${src_dir}/*.cpp
)

add_executable(FileToBye ${src_main}
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(FileToBye PRIVATE
    ${src_dir}
    ${glfw_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(FileToBye PUBLIC
    glfw
    Vulkan::Vulkan
)
