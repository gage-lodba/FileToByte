#include "ImGuiManager.h"
#include "fonts/Consolas.h"
#include "fonts/Roboto.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <stdexcept>

ImFont *Roboto = nullptr;
ImFont *Consolas = nullptr;

ImGuiManager::ImGuiManager(GLFWwindow *window, VkInstance instance,
                           VkPhysicalDevice physicalDevice, VkDevice device,
                           uint32_t queueFamily, VkQueue queue,
                           VkRenderPass renderPass, uint32_t minImageCount,
                           uint32_t imageCount)
    : device(device), descriptorPool(VK_NULL_HANDLE) {

  // Create descriptor pool for ImGui
  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
  };

  VkDescriptorPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 100;
  pool_info.poolSizeCount = sizeof(pool_sizes) / sizeof(pool_sizes[0]);
  pool_info.pPoolSizes = pool_sizes;

  if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create ImGui descriptor pool");
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;

  setupStyle();

  ImGui_ImplGlfw_InitForVulkan(window, true);

  // Initialize ImGui for Vulkan
  ImGui_ImplVulkan_InitInfo init_info{};
  init_info.Instance = instance;
  init_info.PhysicalDevice = physicalDevice;
  init_info.Device = device;
  init_info.QueueFamily = queueFamily;
  init_info.Queue = queue;
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = descriptorPool;
  init_info.PipelineInfoMain.RenderPass = renderPass;
  init_info.PipelineInfoMain.Subpass = 0;
  init_info.MinImageCount = minImageCount;
  init_info.ImageCount = imageCount;
  init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = nullptr;
  init_info.CheckVkResultFn = nullptr;

  ImGui_ImplVulkan_Init(&init_info);

  ImFontConfig fontConfig;
  fontConfig.FontDataOwnedByAtlas = false;

  Roboto = io.Fonts->AddFontFromMemoryCompressedTTF(
      Roboto_compressed_data, Roboto_compressed_size, 14.f, &fontConfig);

  Consolas = io.Fonts->AddFontFromMemoryCompressedTTF(
      Consolas_compressed_data, Consolas_compressed_size, 14.f, &fontConfig);

  io.FontDefault = Roboto;
}

ImGuiManager::~ImGuiManager() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  if (descriptorPool != VK_NULL_HANDLE) {
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
  }
}

void ImGuiManager::newFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiManager::render(VkCommandBuffer commandBuffer) {
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void ImGuiManager::setupStyle() {
  ImColor Dim = ImColor(32, 32, 32);
  ImColor Faint = ImColor(40, 40, 40);
  ImColor Pale = ImColor(48, 48, 48);
  ImColor Soft = ImColor(56, 56, 56);

  ImGui::StyleColorsDark();

  ImGuiStyle *style = &ImGui::GetStyle();
  style->Colors[ImGuiCol_WindowBg] = ImColor(24, 24, 24);
  style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
  style->Colors[ImGuiCol_TextSelectedBg] = ImColor(0, 127, 255, 127);
  style->Colors[ImGuiCol_TextDisabled] = ImColor(127, 127, 127);
  style->Colors[ImGuiCol_FrameBg] = Dim;
  style->Colors[ImGuiCol_FrameBgHovered] = Faint;
  style->Colors[ImGuiCol_FrameBgActive] = Pale;
  style->Colors[ImGuiCol_ScrollbarBg] = Dim;
  style->Colors[ImGuiCol_ScrollbarGrab] = Faint;
  style->Colors[ImGuiCol_ScrollbarGrabHovered] = Pale;
  style->Colors[ImGuiCol_ScrollbarGrabActive] = Soft;
  style->Colors[ImGuiCol_Button] = Dim;
  style->Colors[ImGuiCol_ButtonHovered] = Faint;
  style->Colors[ImGuiCol_ButtonActive] = Pale;

  style->WindowBorderSize = 0.0f;
  style->WindowPadding = ImVec2(10, 10);
  style->FramePadding = ImVec2(5, 5);
  style->ItemSpacing = ImVec2(5, 10);
}
