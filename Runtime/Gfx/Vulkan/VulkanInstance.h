#pragma once

#include "Common/Utils.h"
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include <volk.h>
#include "VulkanPhysicalDevice.h"

class VulkanInstance : public NonCopyable
{
public:

    VulkanInstance(const std::string &applicationName,
                   const std::unordered_map<const char *, bool> &requiredExtensions,
                   const std::vector<const char *> &requiredValidationLayers,
                   bool headless);

    ~VulkanInstance();

private:

    void QueryGpus();

    bool IsExtensionSupported(const std::string &requestedExtension);

    bool IsValidationLayerSupported(const std::string &requestedValidationLayer);


    VkInstance m_Handle{ VK_NULL_HANDLE };

    std::vector<const char *> m_EnabledExtensions;

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)

    VkDebugUtilsMessengerEXT debug_utils_messenger{ VK_NULL_HANDLE };

    VkDebugReportCallbackEXT debug_report_callback{ VK_NULL_HANDLE };

#endif
    std::vector<std::unique_ptr<VulkanPhysicalDevice>> m_GPUs;

    std::vector<VkExtensionProperties> m_SupportedInstanceExtensions;

    std::vector<VkLayerProperties> m_SupportedValidationLayers;

};