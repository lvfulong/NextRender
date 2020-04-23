#pragma once

#include "Common/Utils.h"

class VulkanPhysicalDevice : public NonCopyable
{
public:

    VulkanPhysicalDevice(const Instance &instance, VkPhysicalDevice physicalDevice);

    const VkPhysicalDeviceProperties &GetProperties() const;

    const std::vector<VkQueueFamilyProperties> &GetQueueFamilyProperties() const;

private:

    const Instance &m_Instance;

    VkPhysicalDevice m_Handle{ VK_NULL_HANDLE };

    VkPhysicalDeviceProperties m_Properties;

    std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
};
