#include "Context.hpp"
#include "Utils.hpp"
#include "VulkanUtils.hpp"
#include <set>
#include <algorithm>

namespace
{
const int c_windowWidth = 1600;
const int c_windowHeight = 1200;

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT /*flags*/,
                                             VkDebugReportObjectTypeEXT /*objType*/,
                                             uint64_t /*obj*/,
                                             size_t /*location*/,
                                             int32_t /*code*/,
                                             const char* /*layerPrefix*/,
                                             const char* msg,
                                             void* /*userData*/)
{
    printf("Validation layer: %s\n", msg);
    return VK_FALSE;
}
} // namespace

Context::Context()
{
    initGLFW();
    createInstance();
    createDebugCallback();
    createWindow();
    getPhysicalDevice();
    createDevice();
    createSwapchain();
    createCommandPools();
    createSemaphores();
}

Context::~Context()
{
    vkDestroySemaphore(m_device, m_renderFinished, nullptr);
    vkDestroySemaphore(m_device, m_imageAvailable, nullptr);
    vkDestroyCommandPool(m_device, m_computeCommandPool, nullptr);
    vkDestroyCommandPool(m_device, m_graphicsCommandPool, nullptr);

    for (const VkImageView& imageView : m_swapchainImageViews)
    {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);

    vkDestroyDevice(m_device, nullptr);

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();

    auto destroyDebugReportCallback
        = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");
    CHECK(destroyDebugReportCallback);
    destroyDebugReportCallback(m_instance, m_callback, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

void Context::initGLFW()
{
    const int glfwInitialized = glfwInit();
    CHECK(glfwInitialized == GLFW_TRUE);

    const int vulkanSupported = glfwVulkanSupported();
    CHECK(vulkanSupported == GLFW_TRUE);
}

void Context::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "MyApp";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    const std::vector<const char*> extensions = getRequiredInstanceExtensions();

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledExtensionCount = ui32Size(extensions);
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    instanceCreateInfo.enabledLayerCount = ui32Size(c_validationLayers);
    instanceCreateInfo.ppEnabledLayerNames = c_validationLayers.data();

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);
    VK_CHECK(result);
}

void Context::createDebugCallback()
{
    VkDebugReportCallbackCreateInfoEXT cbCreateInfo{};
    cbCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    cbCreateInfo.flags = //
        VK_DEBUG_REPORT_ERROR_BIT_EXT | //
        VK_DEBUG_REPORT_WARNING_BIT_EXT | //
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    cbCreateInfo.pfnCallback = debugCallback;

    auto createDebugReportCallback
        = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
    CHECK(createDebugReportCallback);
    createDebugReportCallback(m_instance, &cbCreateInfo, nullptr, &m_callback);
}

void Context::createWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(c_windowWidth, c_windowHeight, "Vulkan", nullptr, nullptr);
    CHECK(m_window);
    glfwSetWindowPos(m_window, 1200, 200);

    VkResult result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);
    VK_CHECK(result);
}

void Context::getPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    CHECK(deviceCount);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (VkPhysicalDevice device : devices)
    {
        if (isDeviceSuitable(device, m_surface))
        {
            m_physicalDevice = device;
            break;
        }
    }
    CHECK(m_physicalDevice != VK_NULL_HANDLE);

    //printDeviceExtensions(m_physicalDevice);
    vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
    printPhysicalDeviceName(m_physicalDeviceProperties);
}

void Context::createDevice()
{
    const QueueFamilyIndices indices = getQueueFamilies(m_physicalDevice, m_surface);

    const std::set<int> uniqueQueueFamilies = //
        {
            indices.graphicsFamily,
            indices.computeFamily,
            indices.presentFamily //
        };

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    const float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = ui32Size(queueCreateInfos);
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = ui32Size(c_deviceExtensions);
    createInfo.ppEnabledExtensionNames = c_deviceExtensions.data();
    createInfo.enabledLayerCount = ui32Size(c_validationLayers);
    createInfo.ppEnabledLayerNames = c_validationLayers.data();

    VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    VK_CHECK(result);

    vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.computeFamily, 0, &m_computeQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily, 0, &m_presentQueue);
}

void Context::createSwapchain()
{
    const SwapchainCapabilities capabilities = getSwapchainCapabilities(m_physicalDevice, m_surface);

    const VkSurfaceFormatKHR surfaceFormat{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

    bool formatAvailable = true;
    for (const VkSurfaceFormatKHR& format : capabilities.formats)
    {
        formatAvailable = formatAvailable || (surfaceFormat.format && format.colorSpace == surfaceFormat.colorSpace);
    }
    CHECK(formatAvailable);

    const VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    const auto foundPresentMode = std::find(std::begin(capabilities.presentModes), std::end(capabilities.presentModes), presentMode);
    CHECK(foundPresentMode != std::end(capabilities.presentModes));

    const VkExtent2D extent{c_windowWidth, c_windowHeight};
    CHECK(extent.width <= capabilities.surfaceCapabilities.maxImageExtent.width);
    CHECK(extent.width >= capabilities.surfaceCapabilities.minImageExtent.width);
    CHECK(extent.height <= capabilities.surfaceCapabilities.maxImageExtent.height);
    CHECK(extent.height >= capabilities.surfaceCapabilities.minImageExtent.height);

    const uint32_t imageCount = 3;
    CHECK(imageCount > capabilities.surfaceCapabilities.minImageCount);
    CHECK(imageCount < capabilities.surfaceCapabilities.maxImageCount);

    const QueueFamilyIndices indices = getQueueFamilies(m_physicalDevice, m_surface);
    uint32_t queueFamilyIndices[] = {(uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily};
    CHECK(indices.graphicsFamily == indices.presentFamily);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.preTransform = capabilities.surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain);
    VK_CHECK(result);

    uint32_t queriedImageCount;
    vkGetSwapchainImagesKHR(m_device, m_swapChain, &queriedImageCount, nullptr);
    CHECK(queriedImageCount == imageCount);
    m_swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapChain, &queriedImageCount, m_swapchainImages.data());

    m_swapchainImageViews.resize(m_swapchainImages.size());
    for (size_t i = 0; i < m_swapchainImages.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(m_device, &createInfo, nullptr, &m_swapchainImageViews[i]);
        VK_CHECK(result);
    }
}

void Context::createCommandPools()
{
    const QueueFamilyIndices indices = getQueueFamilies(m_physicalDevice, m_surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = indices.graphicsFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult result = vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_graphicsCommandPool);
    VK_CHECK(result);

    poolInfo.queueFamilyIndex = indices.computeFamily;
    result = vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_computeCommandPool);
    VK_CHECK(result);
}

void Context::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailable);
    VK_CHECK(result);
    result = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinished);
    VK_CHECK(result);
}
