cmake_minimum_required(VERSION 3.15)
project(FileToBye C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

find_package(Vulkan REQUIRED)

include(scripts/glfw.cmake)
include(scripts/imgui.cmake)

include_directories(${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends ..)
include_directories(${glfw_SOURCE_DIR}/deps)

file(GLOB sources source/**.cpp)

add_executable(FileToBye ${sources}
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)

set_target_properties(FileToBye PROPERTIES OUTPUT_NAME "File to byte")

target_link_libraries(FileToBye PUBLIC
    glfw
    Vulkan::Vulkan
)

target_compile_definitions(FileToBye PUBLIC -DImTextureID=ImU64)