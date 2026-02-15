#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
  Window(int width, int height, const std::string &title);
  ~Window();

  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  [[nodiscard]] bool shouldClose() const noexcept;
  void pollEvents() const noexcept;
  [[nodiscard]] GLFWwindow *getGLFWWindow() const noexcept { return window; }

private:
  void initWindow();
  void cleanup() noexcept;

  GLFWwindow *window;

  int width;
  int height;

  std::string title;
};
