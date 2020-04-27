#pragma once

#include "../Common/Utils.h"
#include "GfxShader.h"
//#include <algorithm>
#include <vector>
//#include <string>



class GfxResourceManager : public NonCopyable
{
public:

    GfxResourceManager();

    GfxShaderPtr RequestShader(ShaderType shaderType, const std::string &entryPoint, const std::vector<uint8_t> &source, const std::vector<std::string> &definitions);

private:


};