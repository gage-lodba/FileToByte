cmake_minimum_required(VERSION 3.11)
project(FileToByte)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Vulkan REQUIRED)

add_subdirectory(deps/glfw)
include_directories(deps/glfw/include)

include_directories(deps/imgui/ deps/imgui/backends deps/imgui/misc/cpp)

file(GLOB sources 
  source/*.cpp 
  source/*.h
  source/fonts/*.h
  deps/imgui/imgui.cpp
  deps/imgui/imgui.h
  deps/imgui/imgui_draw.cpp
  deps/imgui/imgui_tables.cpp
  deps/imgui/imgui_widgets.cpp
  deps/imgui/backends/imgui_impl_glfw.cpp
  deps/imgui/backends/imgui_impl_glfw.h
  deps/imgui/backends/imgui_impl_vulkan.cpp
  deps/imgui/backends/imgui_impl_vulkan.h
  deps/imgui/misc/cpp/imgui_stdlib.cpp
  deps/imgui/misc/cpp/imgui_stdlib.h
)

add_executable(FileToByte)

target_sources(FileToByte PRIVATE ${sources})
target_link_libraries(FileToByte PRIVATE glfw Vulkan::Vulkan)

if(WIN32)
  set_source_files_properties(source/Main.cpp PROPERTIES COMPILE_DEFINITIONS main=WinMain)
  set_property(TARGET FileToByte PROPERTY WIN32_EXECUTABLE true)
endif()
