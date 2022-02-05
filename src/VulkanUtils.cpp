#include "VulkanUtils.hpp"
#include <GLFW/glfw3.h>
#include <set>
#include <string>

void printInstanceLayers()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto& layerProperties : availableLayers)
    {
        printf("%s\n", layerProperties.layerName);
    }
}

void printDeviceExtensions(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    for (const auto& extension : availableExtensions)
    {
        printf("%s\n", extension.extensionName);
    }
}

void printPhysicalDeviceName(VkPhysicalDeviceProperties properties)
{
    printf("Device name: %s\n", properties.deviceName);
}

std::vector<const char*> getRequiredInstanceExtensions()
{
    std::vector<const char*> extensions;
    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned int i = 0; i < glfwExtensionCount; ++i)
    {
        extensions.push_back(glfwExtensions[i]);
    }

    extensions.insert(extensions.end(), c_instanceExtensions.begin(), c_instanceExtensions.end());
    return extensions;
}

bool hasAllQueueFamilies(const QueueFamilyIndices& indices)
{
    return indices.graphicsFamily != -1 && indices.computeFamily != -1 && indices.presentFamily != -1;
}

QueueFamilyIndices getQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    QueueFamilyIndices indices;
    for (unsigned int i = 0; i < queueFamilies.size(); ++i)
    {
        if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            indices.computeFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (queueFamilies[i].queueCount > 0 && presentSupport)
        {
            indices.presentFamily = i;
        }

        if (hasAllQueueFamilies(indices))
        {
            break;
        }
    }

    return indices;
}

bool hasDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(c_deviceExtensions.begin(), c_deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapchainCapabilities getSwapchainCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    SwapchainCapabilities capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities.surfaceCapabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        capabilities.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, capabilities.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        capabilities.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, capabilities.presentModes.data());
    }

    return capabilities;
}

bool areSwapchainCapabilitiesAdequate(const SwapchainCapabilities& capabilities)
{
    return !capabilities.formats.empty() && !capabilities.presentModes.empty();
}

bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    const bool allQueueFamilies = hasAllQueueFamilies(getQueueFamilies(physicalDevice, surface));
    const bool deviceExtensionSupport = hasDeviceExtensionSupport(physicalDevice);
    const bool swapchainCapabilitiesAdequate = areSwapchainCapabilitiesAdequate(getSwapchainCapabilities(physicalDevice, surface));
    return allQueueFamilies && deviceExtensionSupport && swapchainCapabilitiesAdequate;
}
