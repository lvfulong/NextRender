#include "VulkanShader.h"


VulkanShader::VulkanShader(VulkanDevice &device, ShaderType shaderType, const std::string &entryPoint, const std::vector<uint8_t> &source, const std::vector<std::string> &definitions) : 
    m_Device{device}, 
    GfxShader{ shaderType, entryPoint, source, definitions }
{

}