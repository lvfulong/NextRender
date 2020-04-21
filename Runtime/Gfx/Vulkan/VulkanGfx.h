#pragma once

#include <volk.h>
#include <string>
#include <unordered_map>
#include "Common/Utils.h"

class VulkanGfx : public NonCopyable
{
public:

    VulkanGfx(const std::string &applicationName, const std::unordered_map<const char *, bool> &requiredExtensions = {}, const std::vector<const char *> &requiredValidationLayers = {}, bool headless = false);

    ~VulkanGfx();

    void BeginFrame();

    void EndFrame();

private:

    uint32_t m_CurrentFrameIndex{ 0 };
};