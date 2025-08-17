#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Application {
public:
  Application();
  ~Application();

  void Initialize();
  void Uninitialize();

  void Run();
  void ApplyTheme();
  void Render();

  void SetupVulkan(ImVector<const char *> instance_extensions);
  void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface,
                         int width, int height);

  void CleanupVulkan();
  void CleanupVulkanWindow();

  void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data);
  void FramePresent(ImGui_ImplVulkanH_Window *wd);

  VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();

private:
  GLFWwindow *m_WindowHandle;

  VkAllocationCallbacks *m_Allocator;
  VkInstance m_Instance;
  VkPhysicalDevice m_PhysicalDevice;
  VkDevice m_Device;
  uint32_t m_QueueFamily;
  VkQueue m_Queue;
  VkDebugReportCallbackEXT m_DebugReport;
  VkPipelineCache m_PipelineCache;
  VkDescriptorPool m_DescriptorPool;

  ImGui_ImplVulkanH_Window m_MainWindowData;
  int m_MinImageCount;
  bool m_SwapChainRebuild;
};