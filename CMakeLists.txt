cmake_minimum_required(VERSION 3.12)
project(FileToByte)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Emit build/compile_commands.json so clangd (see .clangd) can resolve includes
# pulled in transitively from VulkanWindow, e.g. <imgui.h>.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# VulkanWindow scaffold: provides the Application/Window/VulkanRenderer/ImGui
# stack and transitively brings in GLFW, Vulkan, VMA, and Dear ImGui.
add_subdirectory(deps/VulkanWindow)

# CONFIGURE_DEPENDS re-runs the glob on incremental builds, so adding a new
# source file triggers a reconfigure instead of being silently left out.
file(GLOB sources CONFIGURE_DEPENDS
  source/*.cpp
  source/*.h
  source/fonts/*.h
)

add_executable(FileToByte)

target_sources(FileToByte PRIVATE ${sources})
# VulkanWindow's imgui target now compiles imgui_stdlib and exposes its include
# dir, so <imgui_stdlib.h> and ImGui::InputText(label, &std::string) work
# transitively.
target_link_libraries(FileToByte PRIVATE VulkanWindow::VulkanWindow)

if(WIN32)
  set_source_files_properties(source/main.cpp PROPERTIES COMPILE_DEFINITIONS main=WinMain)
  set_property(TARGET FileToByte PROPERTY WIN32_EXECUTABLE true)
endif()
