#pragma once

#include "Common/Utils.h"
#include <algorithm>

class VulkanInstance : public NonCopyable
{
public:

    VulkanInstance(const std::string &applictionName,
                   const std::unordered_map<const char *, bool> &requiredExtensions,
                   const std::vector<const char *> &requiredValidationLayers,
                   bool headless);

private:

    std::vector<const char *> m_EnabledExtensions;

};