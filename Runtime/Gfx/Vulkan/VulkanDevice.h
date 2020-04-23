#pragma once

#include "Common/Utils.h"
#include "VulkanInstance.h"
#include <vector>

class VulkanDevice : public NonCopyable
{
public:

    VulkanDevice(const PhysicalDevice &gpu, VkSurfaceKHR surface, std::unordered_map<const char *, bool> requested_extensions = {});

private:

    const VulkanInstance &m_Instance;

    VkPhysicalDevice m_Handle{ VK_NULL_HANDLE };


};
