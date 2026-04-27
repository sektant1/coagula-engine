#include "graphics/vulkan/Context.h"

#include <cstring>
#include <set>
#include <stdexcept>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace mnd::vk
{
namespace
{
constexpr const char *kValidationLayerName = "VK_LAYER_KHRONOS_validation";

const std::vector<const char *> kRequiredDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

void Check(VkResult result, const char *what)
{
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("vk: ") + what + " failed (VkResult=" +
                                 std::to_string(static_cast<int>(result)) + ")");
    }
}

bool ValidationLayerAvailable()
{
    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, nullptr);
    std::vector<VkLayerProperties> layers(count);
    vkEnumerateInstanceLayerProperties(&count, layers.data());
    for (const auto &layer : layers)
    {
        if (std::strcmp(layer.layerName, kValidationLayerName) == 0)
        {
            return true;
        }
    }
    return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                             VkDebugUtilsMessageTypeFlagsEXT /*type*/,
                                             const VkDebugUtilsMessengerCallbackDataEXT *data,
                                             void * /*userData*/)
{
    // Forward to stderr so the preview tool can run without the engine logger.
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        fprintf(stderr, "[vk] %s\n", data->pMessage);
    }
    return VK_FALSE;
}

QueueFamilies FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilies out;
    uint32_t      count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

    for (uint32_t i = 0; i < count; ++i)
    {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            out.graphics = i;
        }
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
            out.present = i;
        }
        if (out.IsComplete())
        {
            break;
        }
    }
    return out;
}

bool DeviceHasRequiredExtensions(VkPhysicalDevice device)
{
    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());

    for (const char *required : kRequiredDeviceExtensions)
    {
        bool found = false;
        for (const auto &ext : extensions)
        {
            if (std::strcmp(ext.extensionName, required) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return false;
        }
    }
    return true;
}

bool DeviceHasUsableSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    return formatCount > 0 && presentModeCount > 0;
}
}  // namespace

Context::Context(GLFWwindow *window, bool enableValidation)
    : m_enableValidation(enableValidation && ValidationLayerAvailable())
{
    CreateInstance();
    if (m_enableValidation)
    {
        CreateDebugMessenger();
    }
    CreateSurface(window);
    PickPhysicalDevice();
    CreateLogicalDevice();
}

Context::~Context()
{
    if (m_device)
    {
        vkDestroyDevice(m_device, nullptr);
    }
    if (m_debugMessenger)
    {
        auto destroyFn = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (destroyFn)
        {
            destroyFn(m_instance, m_debugMessenger, nullptr);
        }
    }
    if (m_surface)
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }
    if (m_instance)
    {
        vkDestroyInstance(m_instance, nullptr);
    }
}

void Context::CreateInstance()
{
    VkApplicationInfo app{};
    app.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName   = "monad-engine vulkan preview";
    app.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app.pEngineName        = "monad-engine";
    app.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
    app.apiVersion         = VK_API_VERSION_1_2;

    uint32_t     glfwExtCount = 0;
    const char **glfwExt      = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    std::vector<const char *> extensions(glfwExt, glfwExt + glfwExtCount);
    if (m_enableValidation)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo info{};
    info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo        = &app;
    info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    info.ppEnabledExtensionNames = extensions.data();

    const char *layers[] = { kValidationLayerName };
    if (m_enableValidation)
    {
        info.enabledLayerCount   = 1;
        info.ppEnabledLayerNames = layers;
    }

    Check(vkCreateInstance(&info, nullptr, &m_instance), "vkCreateInstance");
}

void Context::CreateDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = DebugCallback;

    auto createFn = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
    if (!createFn)
    {
        return;
    }
    Check(createFn(m_instance, &info, nullptr, &m_debugMessenger),
          "vkCreateDebugUtilsMessengerEXT");
}

void Context::CreateSurface(GLFWwindow *window)
{
    Check(glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface),
          "glfwCreateWindowSurface");
}

void Context::PickPhysicalDevice()
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(m_instance, &count, nullptr);
    if (count == 0)
    {
        throw std::runtime_error("vk: no physical devices found");
    }
    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(m_instance, &count, devices.data());

    // Score: discrete GPU > integrated > anything else, gated on swapchain support.
    int              bestScore  = -1;
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    QueueFamilies    bestFamilies;

    for (auto device : devices)
    {
        if (!DeviceHasRequiredExtensions(device))     continue;
        if (!DeviceHasUsableSwapchain(device, m_surface)) continue;

        QueueFamilies fam = FindQueueFamilies(device, m_surface);
        if (!fam.IsComplete()) continue;

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);
        int score = 0;
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)   score = 1000;
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) score = 500;
        if (score > bestScore)
        {
            bestScore    = score;
            bestDevice   = device;
            bestFamilies = fam;
        }
    }

    if (bestDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("vk: no suitable physical device");
    }
    m_physicalDevice = bestDevice;
    m_queueFamilies  = bestFamilies;
}

void Context::CreateLogicalDevice()
{
    std::set<uint32_t> uniqueFamilies = { m_queueFamilies.graphics, m_queueFamilies.present };
    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    queueInfos.reserve(uniqueFamilies.size());
    float priority = 1.0f;
    for (uint32_t family : uniqueFamilies)
    {
        VkDeviceQueueCreateInfo qci{};
        qci.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci.queueFamilyIndex = family;
        qci.queueCount       = 1;
        qci.pQueuePriorities = &priority;
        queueInfos.push_back(qci);
    }

    VkPhysicalDeviceFeatures features{};

    VkDeviceCreateInfo info{};
    info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount    = static_cast<uint32_t>(queueInfos.size());
    info.pQueueCreateInfos       = queueInfos.data();
    info.pEnabledFeatures        = &features;
    info.enabledExtensionCount   = static_cast<uint32_t>(kRequiredDeviceExtensions.size());
    info.ppEnabledExtensionNames = kRequiredDeviceExtensions.data();

    Check(vkCreateDevice(m_physicalDevice, &info, nullptr, &m_device), "vkCreateDevice");

    vkGetDeviceQueue(m_device, m_queueFamilies.graphics, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_queueFamilies.present,  0, &m_presentQueue);
}

}  // namespace mnd::vk
