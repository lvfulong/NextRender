#pragma once

#include "Common/Utils.h"
#include "VulkanInstance.h"
#include <vector>
#include <algorithm>
#include <vector>
#include <string>
#include <volk.h>
#include <unordered_map>

class VulkanPhysicalDevice;

class VulkanDevice : public NonCopyable
{
public:

    VulkanDevice(const VulkanPhysicalDevice &gpu, VkSurfaceKHR surface, std::unordered_map<const char *, bool> requestedExtensions = {});

    bool IsExtensionSupported(const std::string &requestedExtension);

private:

    const VulkanPhysicalDevice &mGPU;

    VkSurfaceKHR m_Surface{ VK_NULL_HANDLE };

    VkDevice m_Handle{ VK_NULL_HANDLE };

    //const VulkanInstance &m_Instance;

    std::vector<VkExtensionProperties> m_DeviceExtensions;

    std::vector<const char *> m_EnabledExtensions{};

};
