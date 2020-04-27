#pragma once

#include "../Common/Utils.h"
//#include <algorithm>
#include <vector>
#include <string>
#include <memory>

class GfxShader;

using  GfxShaderPtr = std::shared_ptr<GfxShader>;

enum ShaderType
{
    VertexShader,

    FragmentShader,
};

class GfxShader : public NonCopyable
{
public:

    GfxShader(ShaderType shaderType, const std::string &entryPoint, const std::vector<uint8_t> &source, const std::vector<std::string> &definitions);

protected:

    ShaderType m_ShaderType;

    std::string m_EntryPoint;

};