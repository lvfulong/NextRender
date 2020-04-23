#include "VulkanPhysicalDevice.h"

VulkanPhysicalDevice::VulkanPhysicalDevice(const Instance &instance, VkPhysicalDevice physicalDevice) : m_Instance{ instance }, m_Handle{ physicalDevice }
{
   //vkGetPhysicalDeviceFeatures(physical_device, &features);
    vkGetPhysicalDeviceProperties(m_Handle, &m_Properties);
     //vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

    LOGI("Found GPU: {}", m_Properties.deviceName);

    uint32_t queueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_Handle, &queueFamilyPropertiesCount, nullptr);
    m_QueueFamilyProperties.resize(queueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_Handle, &queueFamilyPropertiesCount, m_QueueFamilyProperties.data());
}

const VulkanPhysicalDevice::VkPhysicalDeviceProperties &GetProperties() const
{
    return m_Properties;
}

const std::vector<VkQueueFamilyProperties> &PhysicalDevice::GetQueueFamilyProperties() const
{
    return m_QueueFamilyProperties;
}