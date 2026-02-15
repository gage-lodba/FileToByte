#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>
#include <memory>
#include <optional>
#include <vector>

class ImGuiManager;
class UserInterface;

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  [[nodiscard]] bool isComplete() const noexcept {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer {
public:
  explicit VulkanRenderer(GLFWwindow *window);
  ~VulkanRenderer();

  VulkanRenderer(const VulkanRenderer &) = delete;
  VulkanRenderer &operator=(const VulkanRenderer &) = delete;

  void drawFrame();
  void waitIdle() const;

  [[nodiscard]] VkDevice getDevice() const noexcept { return device; }
  [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const noexcept {
    return physicalDevice;
  }
  [[nodiscard]] VkCommandPool getCommandPool() const noexcept {
    return commandPool;
  }
  [[nodiscard]] VkQueue getGraphicsQueue() const noexcept {
    return graphicsQueue;
  }

private:
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
  void createImGui();
  void createUserInterface();
  void cleanup();

  void cleanupSwapChain();
  void recreateSwapChain();

  [[nodiscard]] bool checkValidationLayerSupport() const;
  [[nodiscard]] bool isDeviceSuitable(VkPhysicalDevice device) const;
  [[nodiscard]] bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
  [[nodiscard]] QueueFamilyIndices
  findQueueFamilies(VkPhysicalDevice device) const;
  [[nodiscard]] SwapChainSupportDetails
  querySwapChainSupport(VkPhysicalDevice device) const;
  [[nodiscard]] VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats) const;
  [[nodiscard]] VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes) const;
  [[nodiscard]] VkExtent2D
  chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;
  [[nodiscard]] std::vector<const char *> getRequiredExtensions() const;

  // Debug callback
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

  GLFWwindow *window;

  VkAllocationCallbacks *allocator;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass;
  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  uint32_t currentFrame{0};

  std::unique_ptr<ImGuiManager> imguiManager;
  std::unique_ptr<UserInterface> userInterface;

  static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

  static constexpr std::array<const char *, 1> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  static constexpr std::array<const char *, 1> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
  static constexpr bool enableValidationLayers = false;
#else
  static constexpr bool enableValidationLayers = true;
#endif
};
