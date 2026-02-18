#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>
#include <memory>
#include <vector>

class ImGuiManager;
class UserInterface;

class VulkanRenderer {
public:
  explicit VulkanRenderer(GLFWwindow *window);
  ~VulkanRenderer();

  VulkanRenderer(const VulkanRenderer &) = delete;
  VulkanRenderer &operator=(const VulkanRenderer &) = delete;

  void drawFrame();
  void waitIdle() const;

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
  [[nodiscard]] std::vector<const char *> getRequiredExtensions() const;

  GLFWwindow *window;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  uint32_t graphicsQueueFamily{0};
  uint32_t presentQueueFamily{0};

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  uint32_t swapChainMinImageCount{0};
  VkFormat swapChainImageFormat{};
  VkExtent2D swapChainExtent{};
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
