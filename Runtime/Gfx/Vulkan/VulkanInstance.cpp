#include "VulkanInstance.h"

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

    //
    //auto extensionError = false;
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
                LOGE("Required instance extension {} not available, cannot run", extensionName);
            }
            //extensionError = !extension_is_optional;
        }
        else
        {
            enabled_extensions.push_back(extension_name);
        }
    }

    /*if (extensionError)
    {
        throw std::runtime_error
        LOGW("Required instance extensions are missing.");
    }*/

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
}