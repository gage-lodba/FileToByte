#include <GLEW/glew.h>

#include <GLFW/glfw3.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_stdlib.h"

namespace Menu {
	extern void DrawMenu(GLFWwindow* window);
	extern void Initialize(GLFWwindow* window);
	extern void Shutdown();
}