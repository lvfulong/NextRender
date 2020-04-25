#include "VulkanPhysicalDevice.h"
#include "VulkanInstance.h"
#include "Common/Logging.h"
#include "VulkanUtils.h"

VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanInstance &instance, VkPhysicalDevice physicalDevice) : m_Instance{ instance }, m_Handle{ physicalDevice }
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

const VkPhysicalDeviceProperties &VulkanPhysicalDevice::GetProperties() const
{
    return m_Properties;
}

const std::vector<VkQueueFamilyProperties> &VulkanPhysicalDevice::GetQueueFamilyProperties() const
{
    return m_QueueFamilyProperties;
}

VkPhysicalDevice VulkanPhysicalDevice::GetHandle() const
{
    return m_Handle;
}

/*const  VulkanInstance &VulkanPhysicalDevice::GetVulkanInstance() const
{
    return m_Instance;
}*/

void *VulkanPhysicalDevice::GetRequestedExtensionFeatures() const
{
    return m_LastRequestedExtensionFeature;
}

const VkPhysicalDeviceFeatures VulkanPhysicalDevice::GetRequestedFeatures() const
{
    return m_RequestedFeatures;
}

VkBool32 VulkanPhysicalDevice::IsPresentSupported(VkSurfaceKHR surface, uint32_t queueFamilyIndex) const
{
    VkBool32 presentSupported{ VK_FALSE };

    if (surface != VK_NULL_HANDLE)
    {
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_Handle, queueFamilyIndex, surface, &presentSupported));
    }

    return presentSupported;
}