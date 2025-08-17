#include "application.hpp"

#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "FTB.hpp"
#include "fonts/Consolas.hpp"
#include "fonts/Roboto.hpp"

ImFont *Roboto = nullptr;
ImFont *Consolas = nullptr;

bool open = true;

std::string VarName;
std::string FilePath;
std::string Result;

#if defined(_MSC_VER) && (_MSC_VER >= 1900) &&                                 \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// #define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
#endif

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void check_vk_result(VkResult err) {
  if (err == 0)
    return;

  fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);

  if (err < 0)
    abort();
}

#ifdef APP_USE_VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
             uint64_t object, size_t location, int32_t messageCode,
             const char *pLayerPrefix, const char *pMessage, void *pUserData) {
  (void)flags;
  (void)object;
  (void)location;
  (void)messageCode;
  (void)pUserData;
  (void)pLayerPrefix; // Unused arguments

  fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n",
          objectType, pMessage);

  return VK_FALSE;
}
#endif // APP_USE_VULKAN_DEBUG_REPORT

static bool
IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties,
                     const char *extension) {
  for (const VkExtensionProperties &p : properties)
    if (strcmp(p.extensionName, extension) == 0)
      return true;
  return false;
}

VkPhysicalDevice Application::SetupVulkan_SelectPhysicalDevice() {
  uint32_t gpu_count;
  VkResult err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, nullptr);
  check_vk_result(err);
  IM_ASSERT(gpu_count > 0);

  ImVector<VkPhysicalDevice> gpus;
  gpus.resize(gpu_count);
  err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, gpus.Data);
  check_vk_result(err);

  for (VkPhysicalDevice &device : gpus) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      return device;
  }

  if (gpu_count > 0)
    return gpus[0];
  return VK_NULL_HANDLE;
}

void Application::SetupVulkan(ImVector<const char *> instance_extensions) {
  VkResult err;

  {
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    uint32_t properties_count;
    ImVector<VkExtensionProperties> properties;
    vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
    properties.resize(properties_count);
    err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count,
                                                 properties.Data);
    check_vk_result(err);

    if (IsExtensionAvailable(
            properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
      instance_extensions.push_back(
          VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
    if (IsExtensionAvailable(properties,
                             VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
      instance_extensions.push_back(
          VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
#endif

#ifdef APP_USE_VULKAN_DEBUG_REPORT
    const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
    create_info.enabledLayerCount = 1;
    create_info.ppEnabledLayerNames = layers;
    instance_extensions.push_back("VK_EXT_debug_report");
#endif

    create_info.enabledExtensionCount = (uint32_t)instance_extensions.Size;
    create_info.ppEnabledExtensionNames = instance_extensions.Data;
    err = vkCreateInstance(&create_info, m_Allocator, &m_Instance);
    check_vk_result(err);

#ifdef APP_USE_VULKAN_DEBUG_REPORT
    auto vkCreateDebugReportCallbackEXT =
        (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
            m_Instance, "vkCreateDebugReportCallbackEXT");
    IM_ASSERT(vkCreateDebugReportCallbackEXT != nullptr);
    VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
    debug_report_ci.sType =
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                            VK_DEBUG_REPORT_WARNING_BIT_EXT |
                            VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    debug_report_ci.pfnCallback = debug_report;
    debug_report_ci.pUserData = nullptr;
    err = vkCreateDebugReportCallbackEXT(m_Instance, &debug_report_ci,
                                         m_Allocator, &m_DebugReport);
    check_vk_result(err);
#endif
  }

  m_PhysicalDevice = SetupVulkan_SelectPhysicalDevice();

  {
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, nullptr);
    VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *)malloc(
        sizeof(VkQueueFamilyProperties) * count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queues);
    for (uint32_t i = 0; i < count; i++)
      if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        m_QueueFamily = i;
        break;
      }
    free(queues);
    IM_ASSERT(m_QueueFamily != (uint32_t)-1);
  }

  {
    ImVector<const char *> device_extensions;
    device_extensions.push_back("VK_KHR_swapchain");

    uint32_t properties_count;
    ImVector<VkExtensionProperties> properties;
    vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr,
                                         &properties_count, nullptr);
    properties.resize(properties_count);
    vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr,
                                         &properties_count, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
    if (IsExtensionAvailable(properties,
                             VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
      device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    const float queue_priority[] = {1.0f};
    VkDeviceQueueCreateInfo queue_info[1] = {};
    queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info[0].queueFamilyIndex = m_QueueFamily;
    queue_info[0].queueCount = 1;
    queue_info[0].pQueuePriorities = queue_priority;

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount =
        sizeof(queue_info) / sizeof(queue_info[0]);
    create_info.pQueueCreateInfos = queue_info;
    create_info.enabledExtensionCount = (uint32_t)device_extensions.Size;
    create_info.ppEnabledExtensionNames = device_extensions.Data;
    err =
        vkCreateDevice(m_PhysicalDevice, &create_info, m_Allocator, &m_Device);
    check_vk_result(err);
    vkGetDeviceQueue(m_Device, m_QueueFamily, 0, &m_Queue);
  }

  {
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    err = vkCreateDescriptorPool(m_Device, &pool_info, m_Allocator,
                                 &m_DescriptorPool);
    check_vk_result(err);
  }
}

void Application::SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd,
                                    VkSurfaceKHR surface, int width,
                                    int height) {
  wd->Surface = surface;

  VkBool32 res;
  vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_QueueFamily,
                                       wd->Surface, &res);
  if (res != VK_TRUE) {
    fprintf(stderr, "Error no WSI support on physical device 0\n");
    exit(-1);
  }

  const VkFormat requestSurfaceImageFormat[] = {
      VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
      VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
  const VkColorSpaceKHR requestSurfaceColorSpace =
      VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
      m_PhysicalDevice, wd->Surface, requestSurfaceImageFormat,
      (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat),
      requestSurfaceColorSpace);

#ifdef APP_USE_UNLIMITED_FRAME_RATE
  VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR,
                                      VK_PRESENT_MODE_IMMEDIATE_KHR,
                                      VK_PRESENT_MODE_FIFO_KHR};
#else
  VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
  wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
      m_PhysicalDevice, wd->Surface, &present_modes[0],
      IM_ARRAYSIZE(present_modes));

  IM_ASSERT(m_MinImageCount >= 2);
  ImGui_ImplVulkanH_CreateOrResizeWindow(m_Instance, m_PhysicalDevice, m_Device,
                                         wd, m_QueueFamily, m_Allocator, width,
                                         height, m_MinImageCount);
}

void Application::CleanupVulkan() {
  vkDestroyDescriptorPool(m_Device, m_DescriptorPool, m_Allocator);

#ifdef APP_USE_VULKAN_DEBUG_REPORT
  auto vkDestroyDebugReportCallbackEXT =
      (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
          m_Instance, "vkDestroyDebugReportCallbackEXT");
  vkDestroyDebugReportCallbackEXT(m_Instance, m_DebugReport, m_Allocator);
#endif // APP_USE_VULKAN_DEBUG_REPORT

  vkDestroyDevice(m_Device, m_Allocator);
  vkDestroyInstance(m_Instance, m_Allocator);
}

void Application::CleanupVulkanWindow() {
  ImGui_ImplVulkanH_DestroyWindow(m_Instance, m_Device, &m_MainWindowData,
                                  m_Allocator);
}

void Application::FrameRender(ImGui_ImplVulkanH_Window *wd,
                              ImDrawData *draw_data) {
  VkResult err;

  VkSemaphore image_acquired_semaphore =
      wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;

  VkSemaphore render_complete_semaphore =
      wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;

  err = vkAcquireNextImageKHR(m_Device, wd->Swapchain, UINT64_MAX,
                              image_acquired_semaphore, VK_NULL_HANDLE,
                              &wd->FrameIndex);

  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    m_SwapChainRebuild = true;
    return;
  }

  check_vk_result(err);
  ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];

  {
    err = vkWaitForFences(m_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);
    check_vk_result(err);

    err = vkResetFences(m_Device, 1, &fd->Fence);
    check_vk_result(err);
  }

  {
    err = vkResetCommandPool(m_Device, fd->CommandPool, 0);
    check_vk_result(err);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
    check_vk_result(err);
  }

  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = wd->RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = wd->Width;
    info.renderArea.extent.height = wd->Height;
    info.clearValueCount = 1;
    info.pClearValues = &wd->ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);
  vkCmdEndRenderPass(fd->CommandBuffer);

  {
    VkPipelineStageFlags wait_stage =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    check_vk_result(err);

    err = vkQueueSubmit(m_Queue, 1, &info, fd->Fence);
    check_vk_result(err);
  }
}

void Application::FramePresent(ImGui_ImplVulkanH_Window *wd) {
  if (m_SwapChainRebuild)
    return;

  VkSemaphore render_complete_semaphore =
      wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;

  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &wd->Swapchain;
  info.pImageIndices = &wd->FrameIndex;

  VkResult err = vkQueuePresentKHR(m_Queue, &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    m_SwapChainRebuild = true;
    return;
  }
  check_vk_result(err);
  wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount;
}

Application::Application() {
  m_Allocator = NULL;
  m_Instance = VK_NULL_HANDLE;
  m_PhysicalDevice = VK_NULL_HANDLE;
  m_Device = VK_NULL_HANDLE;
  m_QueueFamily = (uint32_t)-1;
  m_Queue = VK_NULL_HANDLE;
  m_DebugReport = VK_NULL_HANDLE;
  m_PipelineCache = VK_NULL_HANDLE;
  m_DescriptorPool = VK_NULL_HANDLE;

  m_WindowHandle = NULL;
  m_MinImageCount = 2;
  m_SwapChainRebuild = false;

  Initialize();
}

Application::~Application() { Uninitialize(); }

void Application::Initialize() {
  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit())
    return;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  m_WindowHandle = glfwCreateWindow(800, 600, "FileToByte", nullptr, nullptr);

  if (!glfwVulkanSupported()) {
    printf("GLFW: Vulkan Not Supported\n");
    return;
  }

  ImVector<const char *> extensions;
  uint32_t extensions_count = 0;
  const char **glfw_extensions =
      glfwGetRequiredInstanceExtensions(&extensions_count);
  for (uint32_t i = 0; i < extensions_count; i++)
    extensions.push_back(glfw_extensions[i]);
  SetupVulkan(extensions);

  VkSurfaceKHR surface;
  VkResult err = glfwCreateWindowSurface(m_Instance, m_WindowHandle,
                                         m_Allocator, &surface);
  check_vk_result(err);

  int w, h;
  glfwGetFramebufferSize(m_WindowHandle, &w, &h);
  ImGui_ImplVulkanH_Window *wd = &m_MainWindowData;
  SetupVulkanWindow(wd, surface, w, h);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  io.IniFilename = NULL;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ApplyTheme();

  ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);

  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = m_Instance;
  init_info.PhysicalDevice = m_PhysicalDevice;
  init_info.Device = m_Device;
  init_info.QueueFamily = m_QueueFamily;
  init_info.Queue = m_Queue;
  init_info.PipelineCache = m_PipelineCache;
  init_info.DescriptorPool = m_DescriptorPool;
  init_info.RenderPass = wd->RenderPass;
  init_info.Subpass = 0;
  init_info.MinImageCount = m_MinImageCount;
  init_info.ImageCount = wd->ImageCount;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = m_Allocator;
  init_info.CheckVkResultFn = check_vk_result;

  ImGui_ImplVulkan_Init(&init_info);

  ImFontConfig fontConfig;
  fontConfig.FontDataOwnedByAtlas = false;

  Roboto = io.Fonts->AddFontFromMemoryCompressedTTF(
      Roboto_compressed_data, Roboto_compressed_size, 14.f, &fontConfig);

  Consolas = io.Fonts->AddFontFromMemoryCompressedTTF(
      Consolas_compressed_data, Consolas_compressed_size, 14.f, &fontConfig);

  io.FontDefault = Roboto;
}

void Application::Uninitialize() {
  VkSurfaceKHR surface;
  VkResult err = glfwCreateWindowSurface(m_Instance, m_WindowHandle,
                                         m_Allocator, &surface);
  check_vk_result(err);

  err = vkDeviceWaitIdle(m_Device);
  check_vk_result(err);

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  CleanupVulkanWindow();
  CleanupVulkan();

  glfwDestroyWindow(m_WindowHandle);
  glfwTerminate();
}

void Application::Run() {
  ImGui_ImplVulkanH_Window *wd = &m_MainWindowData;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  while (!glfwWindowShouldClose(m_WindowHandle)) {
    glfwPollEvents();

    if (m_SwapChainRebuild) {
      int width, height;
      glfwGetFramebufferSize(m_WindowHandle, &width, &height);
      if (width > 0 && height > 0) {
        ImGui_ImplVulkan_SetMinImageCount(m_MinImageCount);
        ImGui_ImplVulkanH_CreateOrResizeWindow(
            m_Instance, m_PhysicalDevice, m_Device, &m_MainWindowData,
            m_QueueFamily, m_Allocator, width, height, m_MinImageCount);
        m_MainWindowData.FrameIndex = 0;
        m_SwapChainRebuild = false;
      }
    }

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Render();

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized =
        (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
    if (!is_minimized) {
      wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
      wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
      wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
      wd->ClearValue.color.float32[3] = clear_color.w;
      FrameRender(wd, draw_data);
      FramePresent(wd);
    }
  }
}

void Application::ApplyTheme() {
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

  style->WindowPadding = ImVec2(10, 10);
  style->FramePadding = ImVec2(5, 5);
  style->ItemSpacing = ImVec2(5, 10);
}

void Application::Render() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  ImGuiWindowFlags window_flags;
  window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

  ImGui::Begin("File To Byte", &open, window_flags);

  ImGui::Columns(2, "locations", false);

  ImGui::Text("Array Name");

  ImGui::PushItemWidth(-1);
  ImGui::InputText("##Array Name", &VarName);
  ImGui::PopItemWidth();

  if (ImGui::Button("Convert", ImVec2(-1, 25.f)))
    Result = Convert(FilePath, VarName);

  ImGui::NextColumn();

  ImGui::Text("File Path");

  ImGui::PushItemWidth(-1);
  ImGui::InputText("##File Path", &FilePath);
  ImGui::PopItemWidth();

  if (ImGui::Button("Copy to clipboard", ImVec2(-1, 25.f))) {
    ImGui::LogToClipboard();
    ImGui::LogText("%s", Result.c_str());
    ImGui::LogFinish();
  }

  ImGui::Columns();

  ImGui::PushFont(Consolas);
  ImGui::InputTextMultiline("##Result", &Result, ImVec2(-1, -1));
  ImGui::PopFont();
  ImGui::End();
}