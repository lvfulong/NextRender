#pragma once

#include "Common/Utils.h"
#include <algorithm>

class VulkanInstance : public NonCopyable
{
public:

    VulkanInstance(const std::string &applictionName,
                   const std::unordered_map<const char *, bool> &requiredExtensions,
                   const std::vector<const char *> &requiredValidationLayers,
                   bool headless);

private:

    void QueryGpus();

    VkInstance m_Handle{ VK_NULL_HANDLE };

    std::vector<const char *> m_EnabledExtensions;

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)

    VkDebugUtilsMessengerEXT debug_utils_messenger{ VK_NULL_HANDLE };

    VkDebugReportCallbackEXT debug_report_callback{ VK_NULL_HANDLE };

#endif

};