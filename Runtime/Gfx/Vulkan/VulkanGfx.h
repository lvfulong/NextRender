#pragma once

#include <volk.h>
#include <string>
#include <unordered_map>
#include "Common/Utils.h"

class VulkanGfx : public NonCopyable
{
public:

    VulkanGfx(const std::string &applicationName, const std::unordered_map<const char *, bool> &requiredExtensions = {}, const std::vector<const char *> &requiredValidationLayers = {}, bool headless = false);

    ~VulkanGfx();

private:

    VkInstance handle{ VK_NULL_HANDLE };

    std::vector<const char *> enabled_extensions;

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)

    VkDebugUtilsMessengerEXT debug_utils_messenger{ VK_NULL_HANDLE };

    VkDebugReportCallbackEXT debug_report_callback{ VK_NULL_HANDLE };

#endif


};