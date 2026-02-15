#pragma once
#include "VulkanRenderer.h"
#include "Window.h"

#include <memory>

class Application {
public:
  Application();
  Application(int width, int height);

  ~Application();

  void run();

private:
  std::unique_ptr<Window> window;
  std::unique_ptr<VulkanRenderer> renderer;

  static constexpr int DEFAULT_WIDTH = 800;
  static constexpr int DEFAULT_HEIGHT = 600;
};
