#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>

extern ImFont *Roboto;
extern ImFont *Consolas;

class ImGuiManager {
public:
  ImGuiManager(GLFWwindow *window, VkInstance instance,
               VkPhysicalDevice physicalDevice, VkDevice device,
               VkAllocationCallbacks *allocator, uint32_t queueFamily,
               VkQueue queue, VkRenderPass renderPass, uint32_t imageCount);
  ~ImGuiManager();

  ImGuiManager(const ImGuiManager &) = delete;
  ImGuiManager &operator=(const ImGuiManager &) = delete;

  void newFrame();
  void render(VkCommandBuffer commandBuffer);

private:
  void setupStyle();

  GLFWwindow *window;
  VkAllocationCallbacks *allocator;
  VkDevice device;
  VkDescriptorPool descriptorPool;
};
