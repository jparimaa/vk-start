#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "VulkanUtils.hpp"

class Context final
{
public:
    Context();
    ~Context();

    VkPhysicalDevice getPhysicalDevice() const;
    VkDevice getDevice() const;
    const std::vector<VkImage>& getSwapchainImages() const;
    VkQueue getGraphicsQueue() const;
    VkCommandPool getGraphicsCommandPool() const;

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
    VkCommandPool m_graphicsCommandPool;
    VkCommandPool m_computeCommandPool;
    VkSemaphore m_imageAvailable;
    VkSemaphore m_renderFinished;
};
