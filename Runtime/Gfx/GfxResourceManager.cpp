#include "GfxResourceManager.h
#include <cassert>

GfxShaderPtr GfxResourceManager::RequestShader(ShaderType shaderType, const std::string &entryPoint, const std::vector<uint8_t> &source, const std::vector<std::string> &definitions)
{
    assert(!entry_point.empty());
    assert(!source.empty());

    return std::make_ptr();
}