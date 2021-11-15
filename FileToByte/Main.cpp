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

    GLFWimage images[1];
    images[0].pixels = stbi_load("./FTB.png", &images[0].width, &images[0].height, 0, 4);

    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit()) {
        std::cout << "GLEW failed to initialize." << std::endl;
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