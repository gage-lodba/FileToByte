#include "Window.h"

#include <stdexcept>

Window::Window(int width, int height, const std::string &title)
    : window(nullptr), width(width), height(height), title(title) {
  initWindow();
}

Window::~Window() { cleanup(); }

void Window::initWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
}

void Window::cleanup() noexcept {
  if (window) {
    glfwDestroyWindow(window);
    window = nullptr;
  }
  glfwTerminate();
}

bool Window::shouldClose() const noexcept {
  return glfwWindowShouldClose(window);
}

void Window::pollEvents() const noexcept { glfwPollEvents(); }
