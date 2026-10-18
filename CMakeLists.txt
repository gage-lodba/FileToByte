cmake_minimum_required(VERSION 3.20)
project(FileToByte VERSION 1.1.0 LANGUAGES CXX)

# Keep the C++ dialect portable: no compiler-specific extensions (-std=c++NN
# rather than -std=gnu++NN). The standard itself is requested per-target below
# so dependencies keep their own requirements instead of inheriting ours.
set(CMAKE_CXX_EXTENSIONS OFF)

# Emit build/compile_commands.json so clangd (see .clangd) can resolve includes
# pulled in transitively from VulkanWindow, e.g. <imgui.h>. Note: only the
# single-config generators (Ninja / Unix Makefiles) write this file; the
# Visual Studio generator does not, hence .clangd's documented Ninja layout.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Opt-in static MSVC runtime: the produced exe then has no VC++ redistributable
# dependency (it still needs the Vulkan loader, which is a system component
# installed with graphics drivers). Must be set before the VulkanWindow
# subdirectory so GLFW/ImGui/VulkanWindow are built against the same CRT.
option(FILETOBYTE_STATIC_CRT
  "Link the MSVC runtime statically (no VC++ redistributable needed)" OFF)
if(FILETOBYTE_STATIC_CRT AND MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

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

# Request C++23 for FileToByte itself. Dependencies keep their own requirement
# (VulkanWindow advertises cxx_std_20), so this no longer forces the whole
# dependency tree onto C++23 the way a global CMAKE_CXX_STANDARD did.
target_compile_features(FileToByte PRIVATE cxx_std_23)

if(MSVC)
  target_compile_options(FileToByte PRIVATE /W4)
else()
  target_compile_options(FileToByte PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(WIN32)
  # GUI subsystem in every configuration except Debug (Debug keeps a console
  # for diagnostics). A normal int main() is retained: with the default
  # /SUBSYSTEM:WINDOWS the linker would look for WinMain, so point MSVC's entry
  # point at the CRT's mainCRTStartup instead of renaming main() with a macro.
  # Mirrors the pattern documented by the VulkanWindow submodule.
  set_property(TARGET FileToByte PROPERTY WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>)
  if(NOT MINGW)
    target_link_options(FileToByte PRIVATE
      $<$<NOT:$<CONFIG:Debug>>:LINKER:/ENTRY:mainCRTStartup>)
  endif()

  # Windows version resource, generated from the project version so the exe
  # carries FileVersion/ProductVersion metadata. No icon is wired in: the
  # provenance/license of assets/Icon.ico is unverified.
  enable_language(RC)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/FileToByte.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/FileToByte.rc"
    @ONLY)
  target_sources(FileToByte PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/FileToByte.rc")
endif()

# Unit tests (tests/ owns its own targets; included only when BUILD_TESTING is
# on, which include(CTest) defaults to ON).
include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
