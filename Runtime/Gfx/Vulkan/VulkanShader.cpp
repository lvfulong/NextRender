#include "VulkanShader.h"
#include "VulkanDevice.h"
#include <cassert>
//#include "glsl_compiler.h"
//#include "spirv_reflection.h"

VulkanShader::VulkanShader(VulkanDevice &device, ShaderType shaderType, const std::string &entryPoint, const std::vector<uint8_t> &source, const std::vector<std::string> &definitions) : 
    m_Device{device}, 
    GfxShader{ shaderType, entryPoint, source, definitions }
{
    //VkPipelineShaderStageCreateInfo stage_create_info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };

    //stage_create_info.stage = shader_module->get_stage();
    //stage_create_info.pName = shader_module->get_entry_point().c_str();

    // Create the Vulkan handle
    VkShaderModuleCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };

    //vk_create_info.codeSize = shader_module->get_binary().size() * sizeof(uint32_t);
    //vk_create_info.pCode = shader_module->get_binary().data();

    VkResult result = vkCreateShaderModule(m_Device.GetHandle(), &vkCreateInfo, nullptr, &m_Handle);

    assert(result == VK_SUCCESS);

    //stage_create_info.pSpecializationInfo = &specialization_info;

    //stage_create_infos.push_back(stage_create_info);
    //shader_modules.push_back(stage_create_info.module);


    //GLSLCompiler glsl_compiler;

    // Compile the GLSL source
    /*if (!glsl_compiler.compile_to_spirv(stage, glsl_source.get_data(), entry_point, shader_variant, spirv, info_log))
    {
        if (glsl_source.get_filename().empty())
        {
            throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED, "Shader compilation failed:\n" + info_log };
        }
        else
        {
            throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED, "Compilation failed for shader \"" + glsl_source.get_filename() + "\":\n" + info_log };
        }
    }

    SPIRVReflection spirv_reflection;
    */
    // Reflect all shader resouces
    /*if (!spirv_reflection.reflect_shader_resources(stage, spirv, resources, shader_variant))
    {
        throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
    }
    */
    // Generate a unique id, determined by source and variant
    //std::hash<std::string> hasher{};
    //id = hasher(std::string{ spirv.cbegin(), spirv.cend() });
}