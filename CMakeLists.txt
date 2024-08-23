cmake_minimum_required(VERSION 3.11)
project(FileToByte C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Vulkan REQUIRED)

add_subdirectory(glfw)

set(IMGUI_DIR imgui/)
include_directories(imgui/ imgui/backends imgui/misc/cpp)

file(GLOB sources source/*.cpp source/*.hpp)
add_executable(
  FileToByte
  ${sources}
  imgui/imgui.cpp
  imgui/imgui.h
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_vulkan.cpp
  imgui/misc/cpp/imgui_stdlib.cpp)

target_link_libraries(FileToByte PUBLIC Vulkan::Vulkan glfw)

if(WIN32)
  set_property(TARGET FileToByte PROPERTY WIN32_EXECUTABLE true)
endif()
