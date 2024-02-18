cmake_minimum_required(VERSION 3.11)
project(FileToBye C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Vulkan REQUIRED)

add_subdirectory(glfw)

set(IMGUI_DIR imgui/)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ${IMGUI_DIR}/misc/cpp ..)

file(GLOB sources source/*.cpp *.hpp)
add_executable(FileToBye ${sources}
  ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
)

target_link_libraries(FileToBye PUBLIC
  Vulkan::Vulkan
  glfw
)