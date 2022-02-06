#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class Context final
{
public:
    Context();
    ~Context();

private:
    void initGLFW();
    void createInstance();
    void createDebugCallback();
    void createWindow();
    void getPhysicalDevice();
    void createDevice();
    void createSwapchain();
    void createCommandPools();
    void createSemaphores();

    VkInstance m_instance;
    VkDebugReportCallbackEXT m_callback;
    GLFWwindow* m_window;
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice;
    VkPhysicalDeviceProperties m_physicalDeviceProperties;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    VkQueue m_computeQueue;
    VkQueue m_presentQueue;
    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
    VkCommandPool m_graphicsCommandPool;
    VkCommandPool m_computeCommandPool;
    VkSemaphore m_imageAvailable;
    VkSemaphore m_renderFinished;
};
