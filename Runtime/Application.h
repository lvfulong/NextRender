#pragma once

#include <string>
#include "Common/Utils.h"

class Application : public NonCopyable
{
public:

    Application() = default;

    virtual ~Application() = default;

    const std::string& GetName() const;

    void SetName(const std::string &name);

private:

    std::string m_Name{};
};