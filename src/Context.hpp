#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT m_callback = nullptr;
    GLFWwindow* m_window = nullptr;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties m_physicalDeviceProperties;
};
