#pragma once

#include "Common/Utils.h"
//#include <algorithm>
//#include <vector>
//#include <string>
#include <volk.h>

class VulkanDevice;

class VulkanQueue
{
public:

    VulkanQueue(VulkanDevice &device, uint32_t familyIndex, VkQueueFamilyProperties properties, VkBool32 canPresent, uint32_t index);

    VulkanQueue(const VulkanQueue &) = default;

    VulkanQueue(VulkanQueue &&other);

    VulkanQueue &operator=(const VulkanQueue &) = delete;

    VulkanQueue &operator=(VulkanQueue &&) = delete;

private:

    VulkanDevice &m_Device;

    VkQueue m_Handle{ VK_NULL_HANDLE };

    uint32_t m_FamilyIndex{ 0 };

    uint32_t m_Index{ 0 };

    VkBool32 m_CanPresent{ VK_FALSE };

    VkQueueFamilyProperties m_Properties{};
};
