#include "VulkanQueue.h"
#include "VulkanDevice.h"

VulkanQueue::VulkanQueue(VulkanDevice &device, uint32_t familyIndex, VkQueueFamilyProperties properties, VkBool32 canPresent, uint32_t index) :
    m_Device{device},
    m_FamilyIndex{familyIndex},
    m_Index{index},
    m_CanPresent{canPresent},
    m_Properties{properties}
{
	vkGetDeviceQueue(m_Device.GetHandle(), familyIndex, index, &m_Handle);
}

VulkanQueue::VulkanQueue(VulkanQueue &&other) :
    m_Device{ other.m_Device },
    m_Handle{ other.m_Handle },
    m_FamilyIndex{ other.m_FamilyIndex },
    m_Index{ other.m_Index },
    m_CanPresent{ other.m_CanPresent },
    m_Properties{ other.m_Properties }
{
    other.m_Handle = VK_NULL_HANDLE;
    other.m_FamilyIndex = {};
    other.m_Properties = {};
    other.m_CanPresent = VK_FALSE;
    other.m_Index = 0;
}