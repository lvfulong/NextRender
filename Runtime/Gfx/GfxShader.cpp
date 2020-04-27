#include "GfxShader.h"

GfxShader::GfxShader(ShaderType shaderType, const std::string &entryPoint, const std::vector<uint8_t> &source, const std::vector<std::string> &definitions) :
    m_ShaderType{ shaderType }
    , m_EntryPoint{ entryPoint }
{

}