#include "Globals.h"
#include "FTB.h"
#include "Menu.h"

#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

int main() {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(800, 600, "File To Byte", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		return EXIT_FAILURE;
	}

	Menu::Initialize(window);

	while (!glfwWindowShouldClose(window)) {
		Menu::DrawMenu(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Menu::Shutdown();
	glfwTerminate();
	return 0;
}