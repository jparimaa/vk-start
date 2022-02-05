#include "Context.hpp"
#include "Utils.hpp"
#include "VulkanUtils.hpp"

namespace
{
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
}

Context::~Context()
{
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

    const VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);
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
    m_window = glfwCreateWindow(1600, 1200, "Vulkan", nullptr, nullptr);
    CHECK(m_window);
    glfwSetWindowPos(m_window, 1200, 200);

    const VkResult r = glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);
    VK_CHECK(r);
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
