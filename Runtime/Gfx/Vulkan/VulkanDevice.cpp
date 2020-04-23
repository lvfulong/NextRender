#include "VulkanPhysicalDevice.h"

VulkanDevice::VulkanDevice(const PhysicalDevice &gpu, VkSurfaceKHR surface, std::unordered_map<const char *, bool> requested_extensions = {})
{

    LOGI("Selected GPU: {}", gpu.GetProperties().deviceName);

    // Prepare the device queues
    uint32_t                             queueFamilyPropertiesCount = /*to_u32*/(gpu.GetQueueFamilyProperties().size());
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilyPropertiesCount, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
    std::vector<std::vector<float>>      queuePriorities(queueFamilyPropertiesCount);

    for (uint32_t queue_family_index = 0U; queue_family_index < queueFamilyPropertiesCount; ++queue_family_index)
    {
        const VkQueueFamilyProperties &queue_family_property = gpu.GetQueueFamilyProperties()[queue_family_index];

        queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 1.0f);

        VkDeviceQueueCreateInfo &queue_create_info = queue_create_infos[queue_family_index];

        queue_create_info.queueFamilyIndex = queue_family_index;
        queue_create_info.queueCount = queue_family_property.queueCount;
        queue_create_info.pQueuePriorities = queue_priorities[queue_family_index].data();
    }

    // Check extensions to enable Vma Dedicated Allocation
    uint32_t device_extension_count;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.get_handle(), nullptr, &device_extension_count, nullptr));
    device_extensions = std::vector<VkExtensionProperties>(device_extension_count);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu.get_handle(), nullptr, &device_extension_count, device_extensions.data()));

    // Display supported extensions
    if (device_extensions.size() > 0)
    {
        LOGD("Device supports the following extensions:");
        for (auto &extension : device_extensions)
        {
            LOGD("  \t{}", extension.extensionName);
        }
    }

    bool can_get_memory_requirements = is_extension_supported("VK_KHR_get_memory_requirements2");
    bool has_dedicated_allocation = is_extension_supported("VK_KHR_dedicated_allocation");

    if (can_get_memory_requirements && has_dedicated_allocation)
    {
        enabled_extensions.push_back("VK_KHR_get_memory_requirements2");
        enabled_extensions.push_back("VK_KHR_dedicated_allocation");
        LOGI("Dedicated Allocation enabled");
    }

    // Check that extensions are supported before trying to create the device
    std::vector<const char *> unsupported_extensions{};
    for (auto &extension : requested_extensions)
    {
        if (is_extension_supported(extension.first))
        {
            enabled_extensions.emplace_back(extension.first);
        }
        else
        {
            unsupported_extensions.emplace_back(extension.first);
        }
    }

    if (enabled_extensions.size() > 0)
    {
        LOGI("Device supports the following requested extensions:");
        for (auto &extension : enabled_extensions)
        {
            LOGI("  \t{}", extension);
        }
    }

    if (unsupported_extensions.size() > 0)
    {
        auto error = false;
        for (auto &extension : unsupported_extensions)
        {
            auto extension_is_optional = requested_extensions[extension];
            if (extension_is_optional)
            {
                LOGW("Optional device extension {} not available, some features may be disabled", extension);
            }
            else
            {
                error = true;
                LOGE("Required device extension {} not available, cannot run", extension);
            }
            
        }
        {
           assert(!error && "Extensions not present");
        }
    }
}

bool VulkanDevice::is_extension_supported(const std::string &requested_extension)
{
    return std::find_if(device_extensions.begin(), device_extensions.end(),
        [requested_extension](auto &device_extension) {
        return std::strcmp(device_extension.extensionName, requested_extension.c_str()) == 0;
    }) != device_extensions.end();
}