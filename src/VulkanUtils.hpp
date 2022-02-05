#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>
#include <cassert>

const std::vector<const char*> c_validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> c_instanceExtensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
const std::vector<const char*> c_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

// clang-format off
#define VK_CHECK(result)                                                         \
{                                                                                \
    if (result != VK_SUCCESS)                                                    \
    {                                                                            \
        printf("Abort. VkResult != VK_SUCCESS at %s:%d\n", __FILE__ ,__LINE__ ); \
		abort();																	 \
	}																			 \
}
// clang-format on

struct QueueFamilyIndices
{
    int graphicsFamily = -1;
    int computeFamily = -1;
    int presentFamily = -1;
};

struct SwapchainCapabilities
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

void printInstanceLayers();
void printDeviceExtensions(VkPhysicalDevice physicalDevice);
void printPhysicalDeviceName(VkPhysicalDeviceProperties properties);
std::vector<const char*> getRequiredInstanceExtensions();
bool hasAllQueueFamilies(const QueueFamilyIndices& indices);
QueueFamilyIndices getQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
bool hasDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
SwapchainCapabilities getSwapchainCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
bool areSwapchainCapabilitiesAdequate(const SwapchainCapabilities& capabilities);
bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);