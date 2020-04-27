#pragma once

#include "Common/Utils.h"
//#include <algorithm>
//#include <vector>
//#include <string>
#include <volk.h>
#include "../GfxShader.h"

class VulkanDevice;

class VulkanShader : public GfxShader
{
public:

    VulkanShader(VulkanDevice &device, ShaderType shaderType, const std::string &entryPoint, const std::vector<uint8_t> &source, const std::vector<std::string> &definitions);

private:

    VulkanDevice &m_Device;

    VkShaderModule m_Handle{ VK_NULL_HANDLE };
};
