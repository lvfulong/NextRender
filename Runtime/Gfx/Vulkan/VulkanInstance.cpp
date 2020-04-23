#include "VulkanInstance.h"


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



VulkanInstance::VulkanInstance(const std::string &applictionName, 
                               const std::unordered_map<const char *, bool> &requiredExtensions, 
                               const std::vector<const char *> &requiredValidationLayers,
                               bool headless)
{
    VkResult result = volkInitialize();
    assert(result == 0 && "Failt to initialize volk.");

    uint32_t instanceExtensionCount;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr));

    std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data()));


#if defined(NEXT_RENDER_DEBUG) || defined(NEXT_RENDER_VALIDATION_LAYERS)
    // Check if VK_EXT_debug_utils is supported, which supersedes VK_EXT_Debug_Report
    bool debugUtils = false;
    for (auto &availableExtension : availableInstanceExtensions)
    {
        if (strcmp(availableExtension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
        {
            debugUtils = true;
            LOGI("{} is available, enabling it", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            m_EnabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
    }
    if (!debugUtils)
    {
        m_EnabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
#endif

    // Try to enable headless surface extension if it exists
    if (headless)
    {
        bool headlessExtension = false;
        for (auto &availableExtension : availableInstanceExtensions)
        {
            if (strcmp(availableExtension.extensionName, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME) == 0)
            {
                headlessExtension = true;
                LOGI("{} is available, enabling it", VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
                m_EnabledExtensions.push_back(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
            }
        }
        if (!headlessExtension)
        {
            LOGW("{} is not available, disabling swapchain creation", VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
        }
    }
    else
    {
        m_EnabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    }

    auto extensionError = false;
    for (auto extension : requiredExtensions)
    {
        auto extensionName = extension.first;
        auto extensionIsOptional = extension.second;
        if (std::find_if(availableInstanceExtensions.begin(),
                         availableInstanceExtensions.end(),
                         [&extensionName](VkExtensionProperties availableExtension) { return strcmp(availableExtension.extensionName, extensionName) == 0; }) == availableInstanceExtensions.end())
        {
            if (extensionIsOptional)
            {
                LOGW("Optional instance extension {} not available, some features may be disabled", extensionName);
            }
            else
            {
                extensionError = true;
                LOGE("Required instance extension {} not available, cannot run", extensionName);
            }
            //extensionError = !extension_is_optional;
        }
        else
        {
            enabledExtensions.push_back(extensionName);
        }
    }

    {
        //throw std::runtime_error
        LOGW(!extensionError && "Required instance extensions are missing.");
    }

    uint32_t instanceLayerCount;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));

    std::vector<VkLayerProperties> supportedValidationLayers(instanceLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, supportedValidationLayers.data()));

    std::vector<const char *> requestedValidationLayers(requiredValidationLayers);

#ifdef NEXT_RENDER_VALIDATION_LAYERS
    // Determine the optimal validation layers to enable that are necessary for useful debugging
    std::vector<const char *> optimalValidationLayers = getOptimalValidationLayers(supportedValidationLayers);
    requestedValidationLayers.insert(requestedValidationLayers.end(), optimalValidationLayers.begin(), optimalValidationLayers.end());
#endif

    //TODO
    if (validateLayers(requestedValidationLayers, supportedValidationLayers))
    {
        LOGI("Enabled Validation Layers:")
        for (const auto &layer : requestedValidationLayers)
        {
            LOGI("	\t{}", layer);
        }
    }
    else
    {
        //throw std::runtime_error("Required validation layers are missing.");
        LOGW("Required validation layers are missing.");
    }


    VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };

    app_info.pApplicationName = applicationName.c_str();
    app_info.applicationVersion = 0;
    app_info.pEngineName = "Vulkan Samples";
    app_info.engineVersion = 0;
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

    instanceInfo.pApplicationInfo = &appInfo;

    instanceInfo.enabledExtensionCount = /*to_u32*/(enabledExtensions.size());
    instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();

    instanceInfo.enabledLayerCount = /*to_u32*/(requestedValidationLayers.size());
    instanceInfo.ppEnabledLayerNames = requestedValidationLayers.data();

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


void Instance::QueryGpus()
{
    // Querying valid physical devices on the machine
    uint32_t physical_device_count{ 0 };
    VK_CHECK(vkEnumeratePhysicalDevices(handle, &physical_device_count, nullptr));

    if (physical_device_count < 1)
    {
        throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
    }

    std::vector<VkPhysicalDevice> physical_devices;
    physical_devices.resize(physical_device_count);
    VK_CHECK(vkEnumeratePhysicalDevices(handle, &physical_device_count, physical_devices.data()));

    // Create gpus wrapper objects from the VkPhysicalDevice's
    for (auto &physical_device : physical_devices)
    {
        gpus.push_back(std::make_unique<PhysicalDevice>(*this, physical_device));
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

    if (handle != VK_NULL_HANDLE)
    {
        vkDestroyInstance(handle, nullptr);
    }
}

bool VulkanInstance::is_extension_supported(const std::string &requested_extension)
{
    return std::find_if(device_extensions.begin(), device_extensions.end(),
        [requested_extension](auto &device_extension) {
        return std::strcmp(device_extension.extensionName, requested_extension.c_str()) == 0;
    }) != device_extensions.end();
}