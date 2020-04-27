#pragma once

#include "Common/Utils.h"
//#include <algorithm>
//#include <vector>
//#include <string>
#include <volk.h>

class VulkanDevice;

class VulkanShader : public NonCopyable
{
public:

    VulkanShader(VulkanDevice &device);

private:

    VulkanDevice &m_Device;

};
