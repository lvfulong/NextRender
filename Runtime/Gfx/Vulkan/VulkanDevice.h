#pragma once

#include "Common/Utils.h"
#include "VulkanInstance.h"
#include <vector>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include "VulkanQueue.h"
#include <vk_mem_alloc.h>
#include <volk.h>

class VulkanPhysicalDevice;

class VulkanDevice : public NonCopyable
{
public:

    VulkanDevice(const VulkanPhysicalDevice &gpu, VkSurfaceKHR surface, std::unordered_map<const char *, bool> requestedExtensions = {});

    ~VulkanDevice();

    bool IsExtensionSupported(const std::string &requestedExtension);

    VkDevice GetHandle() const;

private:

    const VulkanPhysicalDevice &mGPU;

    VkSurfaceKHR m_Surface{ VK_NULL_HANDLE };

    VkDevice m_Handle{ VK_NULL_HANDLE };

    //const VulkanInstance &m_Instance;

    std::vector<VkExtensionProperties> m_DeviceExtensions;

    std::vector<const char *> m_EnabledExtensions{};

    std::vector<std::vector<VulkanQueue>> m_Queues;

    VmaAllocator m_MemoryAllocator{ VK_NULL_HANDLE };

};
