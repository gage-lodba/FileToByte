#include "Application.h"

Application::Application() : Application(DEFAULT_WIDTH, DEFAULT_HEIGHT) {}

Application::Application(int width, int height) {
  window = std::make_unique<Window>(width, height, "Vulkan Window");
  renderer = std::make_unique<VulkanRenderer>(window->getGLFWWindow());
}

Application::~Application() {
  if (renderer) {
    renderer->waitIdle();
    renderer.reset();
  }
}

void Application::run() {
  while (!window->shouldClose()) {
    window->pollEvents();
    renderer->drawFrame();
  }
}