#include "VulkanInstance.h"
#include "Common/Logging.h"
#include "VulkanUtils.h"
#include <algorithm>

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)

VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *                                      user_data)
{
    // Log debug messge
    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        LOGW("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
    }
    else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LOGE("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
    }
    return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT /*type*/,
    uint64_t /*object*/, size_t /*location*/, int32_t /*message_code*/,
    const char *layer_prefix, const char *message, void * /*user_data*/)
{
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        LOGE("{}: {}", layer_prefix, message);
    }
    else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        LOGW("{}: {}", layer_prefix, message);
    }
    else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        LOGW("{}: {}", layer_prefix, message);
    }
    else
    {
        LOGI("{}: {}", layer_prefix, message);
    }
    return VK_FALSE;
}
#endif
static bool validateLayers(const std::vector<const char *> &required, const std::vector<VkLayerProperties> &available)
{
    for (auto layer : required)
    {
        bool found = false;
        for (auto &availableLayer : available)
        {
            if (strcmp(availableLayer.layerName, layer) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            LOGE("Validation Layer {} not found", layer);
            return false;
        }
    }

    return true;
}


static std::vector<const char *> getOptimalValidationLayers(const std::vector<VkLayerProperties> &supportedInstanceLayers)
{
    std::vector<std::vector<const char *>> validationLayerPriorityList =
    {
        // The preferred validation layer is "VK_LAYER_KHRONOS_validation"
        {"VK_LAYER_KHRONOS_validation"},

        // Otherwise we fallback to using the LunarG meta layer
        {"VK_LAYER_LUNARG_standard_validation"},

        // Otherwise we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't exist
        {
            "VK_LAYER_GOOGLE_threading",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_object_tracker",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_GOOGLE_unique_objects",
        },

        // Otherwise as a last resort we fallback to attempting to enable the LunarG core layer
        {"VK_LAYER_LUNARG_core_validation"} };

    for (auto &validationLayers : validationLayerPriorityList)
    {
        if (validateLayers(validationLayers, supportedInstanceLayers))
        {
            return validationLayers;
        }

        LOGW("Couldn't enable validation layers (see log for error) - falling back");
    }

    // Else return nothing
    return {};
}



VulkanInstance::VulkanInstance(const std::string &applicationName, 
                               const std::unordered_map<const char *, bool> &requiredExtensions, 
                               const std::vector<const char *> &requiredValidationLayers,
                               bool headless)
{
    VkResult result = volkInitialize();
    assert(result == VK_SUCCESS && "Failt to initialize volk.");

    // Process instance extention
    uint32_t instanceExtensionCount;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr));

    m_SupportedInstanceExtensions.resize(instanceExtensionCount);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, m_SupportedInstanceExtensions.data()));


#if defined(NEXT_RENDER_DEBUG) || defined(NEXT_RENDER_VALIDATION_LAYERS)
    // Check if VK_EXT_debug_utils is supported, which supersedes VK_EXT_Debug_Report
    bool debugUtils = false;
    if (IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        debugUtils = true;
        LOGI("{} is available, enabling it", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        m_EnabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    else
    {
        LOGI("{} is available, enabling it", VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        m_EnabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
#endif

    // Try to enable headless surface extension if it exists
    if (headless)
    {
        bool headlessExtension = false;

        if (IsExtensionSupported(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME))
        {
            headlessExtension = true;
            LOGI("{} is available, enabling it", VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
            m_EnabledExtensions.push_back(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
        }
        else
        {
            LOGW("{} is not available, disabling swapchain creation", VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
        }
    }
    else
    {
        LOGI("{} is available, enabling it", VK_KHR_SURFACE_EXTENSION_NAME);
        m_EnabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    }

    for (std::pair<const char *, bool> extension : requiredExtensions)
    {
        auto extensionName = extension.first;
        auto extensionIsOptional = extension.second;

        if (!IsExtensionSupported(extensionName))
        {
            if (extensionIsOptional)
            {
                LOGW("Optional instance extension {} not available, some features may be disabled", extensionName);
            }
            else
            {
                LOGE("Required instance extension {} not available, cannot run", extensionName);
            }
        }
        else
        {
            LOGI("{} is available, enabling it", extensionName);
            m_EnabledExtensions.push_back(extensionName);
        }
    }

    uint32_t instanceLayerCount;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));

    m_SupportedValidationLayers.resize(instanceLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, m_SupportedValidationLayers.data()));

    std::vector<const char *> enabledValidationLayers;
    enabledValidationLayers.reserve(requiredValidationLayers.size());

#ifdef NEXT_RENDER_VALIDATION_LAYERS
    // Determine the optimal validation layers to enable that are necessary for useful debugging
    std::vector<const char *> optimalValidationLayers = getOptimalValidationLayers(supportedValidationLayers);
    enabledValidationLayers.insert(requestedValidationLayers.end(), optimalValidationLayers.begin(), optimalValidationLayers.end());
#endif


    for (const char *validationLayer : requiredValidationLayers)
    {
        if (IsValidationLayerSupported(validationLayer))
        {
            enabledValidationLayers.emplace_back(validationLayer);
            LOGI("{} Enabled Validation Layers", validationLayer);
        }
        else
        {
            LOGE("{}  Required validation layers are missing.", validationLayer);
        }
    }

    VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };

    appInfo.pApplicationName = applicationName.c_str();
    appInfo.applicationVersion = 0;
    appInfo.pEngineName = "Vulkan Samples";
    appInfo.engineVersion = 0;
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

    instanceInfo.pApplicationInfo = &appInfo;

    instanceInfo.enabledExtensionCount = /*to_u32*/(m_EnabledExtensions.size());
    instanceInfo.ppEnabledExtensionNames = m_EnabledExtensions.data();

    instanceInfo.enabledLayerCount = /*to_u32*/(enabledValidationLayers.size());
    instanceInfo.ppEnabledLayerNames = enabledValidationLayers.data();

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
    if (debug_utils)
    {
        debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debug_utils_create_info.pfnUserCallback = debugUtilsMessengerCallback;

        instance_info.pNext = &debugUtilsCreateInfo;
    }
    else
    {
        debug_report_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        debug_report_create_info.pfnCallback = debugCallback;

        instance_info.pNext = &debugReportCreateInfo;
    }
#endif


    // Create the Vulkan instance
    result = vkCreateInstance(&instanceInfo, nullptr, &m_Handle);

    assert(result == VK_SUCCESS && "Could not create Vulkan instance");

    volkLoadInstance(m_Handle);

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    if (debugUtils)
    {
        result = vkCreateDebugUtilsMessengerEXT(handle, &debug_utils_create_info, nullptr, &debug_utils_messenger);

        assert(result == VK_SUCCESS && "Could not create debug utils messenger");

    }
    else
    {
        result = vkCreateDebugReportCallbackEXT(handle, &debug_report_create_info, nullptr, &debug_report_callback);

        assert(result == VK_SUCCESS && "Could not create debug report callback");

    }
#endif

    QueryGpus();

}


void VulkanInstance::QueryGpus()
{
    // Querying valid physical devices on the machine
    uint32_t physicalDeviceCount{ 0 };
    VK_CHECK(vkEnumeratePhysicalDevices(m_Handle, &physicalDeviceCount, nullptr));

    assert(physicalDeviceCount >= 1 && "Couldn't find a physical device that supports Vulkan.");

    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(physicalDeviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(m_Handle, &physicalDeviceCount, physicalDevices.data()));

    // Create gpus wrapper objects from the VkPhysicalDevice's
    for (auto &device : physicalDevices)
    {
        m_GPUs.push_back(std::make_unique<VulkanPhysicalDevice>(*this, device));
    }
}

/*PhysicalDevice &Instance::getSuitableGpu()
{
    assert(!gpus.empty() && "No physical devices were found on the system.");

    // Find a discrete GPU
    for (auto &gpu : gpus)
    {
        if (gpu->get_properties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            return *gpu;
        }
    }

    // Otherwise just pick the first one
    LOGW("Couldn't find a discrete physical device, picking default GPU");
    return *gpus.at(0);
}*/

VulkanInstance::~VulkanInstance()
{
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
    if (debug_utils_messenger != VK_NULL_HANDLE)
    {
        vkDestroyDebugUtilsMessengerEXT(handle, debug_utils_messenger, nullptr);
    }
    if (debug_report_callback != VK_NULL_HANDLE)
    {
        vkDestroyDebugReportCallbackEXT(handle, debug_report_callback, nullptr);
    }
#endif

    if (m_Handle != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_Handle, nullptr);
    }
}

bool VulkanInstance::IsExtensionSupported(const std::string &requestedExtension)
{
    return std::find_if(m_SupportedInstanceExtensions.begin(), m_SupportedInstanceExtensions.end(),
        [requestedExtension](VkExtensionProperties &deviceExtension) {
        return std::strcmp(deviceExtension.extensionName, requestedExtension.c_str()) == 0;
    }) != m_SupportedInstanceExtensions.end();
}

bool VulkanInstance::IsValidationLayerSupported(const std::string &requestedValidationLayer)
{
    return std::find_if(m_SupportedValidationLayers.begin(), m_SupportedValidationLayers.end(),
        [requestedValidationLayer](VkLayerProperties &layerProperties) {
        return std::strcmp(layerProperties.layerName, requestedValidationLayer.c_str()) == 0;
    }) != m_SupportedValidationLayers.end();
}