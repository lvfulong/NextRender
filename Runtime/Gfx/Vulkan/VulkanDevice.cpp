#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "Common/Logging.h"
#include "VulkanUtils.h"
#include <volk.h>
//VKBP_DISABLE_WARNINGS()
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
//VKBP_ENABLE_WARNINGS()

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice &gpu, VkSurfaceKHR surface, std::unordered_map<const char *, bool> requestedExtensions) : mGPU{ gpu }, m_Surface{ surface }
{

    LOGI("Selected GPU: {}", gpu.GetProperties().deviceName);

    // Prepare the device queues
    uint32_t queueFamilyPropertiesCount = /*to_u32*/(gpu.GetQueueFamilyProperties().size());
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilyPropertiesCount, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
    std::vector<std::vector<float>> queuePriorities(queueFamilyPropertiesCount);

    for (uint32_t index = 0U; index < queueFamilyPropertiesCount; ++index)
    {
        const VkQueueFamilyProperties &queueFamilyProperty = gpu.GetQueueFamilyProperties()[index];

        queuePriorities[index].resize(queueFamilyProperty.queueCount, 1.0f);

        VkDeviceQueueCreateInfo &queueCreateInfo = queueCreateInfos[index];

        queueCreateInfo.queueFamilyIndex = index;
        queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
        queueCreateInfo.pQueuePriorities = queuePriorities[index].data();
    }

    // Check extensions to enable Vma Dedicated Allocation
    uint32_t deviceExtensionCount;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.GetHandle(), nullptr, &deviceExtensionCount, nullptr));
    m_DeviceExtensions.resize(deviceExtensionCount);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.GetHandle(), nullptr, &deviceExtensionCount, m_DeviceExtensions.data()));

    // Display supported extensions
    if (m_DeviceExtensions.size() > 0)
    {
        LOGD("Device supports the following extensions:");
        for (VkExtensionProperties &extension : m_DeviceExtensions)
        {
            LOGD("  \t{}", extension.extensionName);
        }
    }

    bool canGetMemoryRequirements = IsExtensionSupported("VK_KHR_get_memory_requirements2");
    bool hasDedicatedAllocation = IsExtensionSupported("VK_KHR_dedicated_allocation");

    if (canGetMemoryRequirements && hasDedicatedAllocation)
    {
        m_EnabledExtensions.push_back("VK_KHR_get_memory_requirements2");
        m_EnabledExtensions.push_back("VK_KHR_dedicated_allocation");
        LOGI("Dedicated Allocation enabled");
    }

    // Check that extensions are supported before trying to create the device
    std::vector<const char *> unsupportedExtensions{};
    for (auto &extension : requestedExtensions)
    {
        if (IsExtensionSupported(extension.first))
        {
            m_EnabledExtensions.emplace_back(extension.first);
        }
        else
        {
            unsupportedExtensions.emplace_back(extension.first);
        }
    }

    if (m_EnabledExtensions.size() > 0)
    {
        LOGI("Device supports the following requested extensions:");
        for (auto &extension : m_EnabledExtensions)
        {
            LOGI("  \t{}", extension);
        }
    }

    if (unsupportedExtensions.size() > 0)
    {
        for (auto &extension : unsupportedExtensions)
        {
            auto extensionIsOptional = requestedExtensions[extension];
            if (extensionIsOptional)
            {
                LOGW("Optional device extension {} not available, some features may be disabled", extension);
            }
            else
            {
                LOGE("Required device extension {} not available, cannot run", extension);
            }
            
        }
    }

    VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

    // Latest requested feature will have the pNext's all set up for device creation.
    //createInfo.pNext = gpu.GetRequestedExtensionFeatures(); TODO

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = /*to_u32*/(queueCreateInfos.size());
    const auto requested_gpu_features = gpu.GetRequestedFeatures();
    createInfo.pEnabledFeatures = &requested_gpu_features;
    createInfo.enabledExtensionCount = /*to_u32*/(m_EnabledExtensions.size());
    createInfo.ppEnabledExtensionNames = m_EnabledExtensions.data();

    VkResult result = vkCreateDevice(gpu.GetHandle(), &createInfo, nullptr, &m_Handle);

    assert(result == VK_SUCCESS && "Cannot create device");

    queues.resize(queueFamilyPropertiesCount);

    for (uint32_t familyIndex = 0U; familyIndex < queueFamilyPropertiesCount; ++familyIndex)
    {
        const VkQueueFamilyProperties &queueFamilyProperty = gpu.GetQueueFamilyProperties()[familyIndex];

        VkBool32 presentSupported = gpu.IsPresentSupported(surface, familyIndex);

        //??????TODO
        // Only check if surface is valid to allow for headless applications
        /*if (surface != VK_NULL_HANDLE)
        {
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu.GetHandle(), index, surface, &present_supported));
        }*/

        for (uint32_t queueIndex = 0U; queueIndex < queueFamilyProperty.queueCount; ++queueIndex)
        {
            queues[familyIndex].emplace_back(*this, familyIndex, queueFamilyProperty, presentSupported, queueIndex);
        }
    }

    VmaVulkanFunctions vma_vulkan_func{};
    vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
    vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
    vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
    vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
    vma_vulkan_func.vkCreateImage = vkCreateImage;
    vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
    vma_vulkan_func.vkDestroyImage = vkDestroyImage;
    vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    vma_vulkan_func.vkFreeMemory = vkFreeMemory;
    vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    vma_vulkan_func.vkMapMemory = vkMapMemory;
    vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.physicalDevice = gpu.GetHandle();
    allocator_info.device = m_Handle;

    if (canGetMemoryRequirements && hasDedicatedAllocation)
    {
        allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
        vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
        vma_vulkan_func.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
    }

    allocator_info.pVulkanFunctions = &vma_vulkan_func;

    result = vmaCreateAllocator(&allocator_info, &m_MemoryAllocator);

    assert(result == VK_SUCCESS && "Cannot create allocator" );

    //command_pool = std::make_unique<CommandPool>(*this, get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0).get_family_index());
    //fence_pool = std::make_unique<FencePool>(*this);

}

bool VulkanDevice::IsExtensionSupported(const std::string &requestedExtension)
{
    return std::find_if(m_DeviceExtensions.begin(), m_DeviceExtensions.end(),
        [requestedExtension](auto &deviceExtension) {
        return std::strcmp(deviceExtension.extensionName, requestedExtension.c_str()) == 0;
    }) != m_DeviceExtensions.end();
}