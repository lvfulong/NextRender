#pragma once

#include "Common/Utils.h"
#include <algorithm>
#include <vector>
#include <string>
#include <volk.h>

class VulkanInstance;

class VulkanPhysicalDevice : public NonCopyable
{
public:

    VulkanPhysicalDevice(const VulkanInstance &instance, VkPhysicalDevice physicalDevice);

    const VkPhysicalDeviceProperties &GetProperties() const;

    const std::vector<VkQueueFamilyProperties> &GetQueueFamilyProperties() const;

    VkPhysicalDevice GetHandle() const;

    //const  VulkanInstance &GetVulkanInstance() const;

    void *GetRequestedExtensionFeatures() const;
private:

    const class VulkanInstance &m_Instance;

    VkPhysicalDevice m_Handle{ VK_NULL_HANDLE };

    VkPhysicalDeviceProperties m_Properties;

    std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;

    void * m_LastRequestedExtensionFeature{ nullptr };
};
