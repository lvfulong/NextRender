#pragma once

#include <memory>
#include "Application.h"
#include "Common/Utils.h"

class Platform : public NonCopyable
{
public:

    Platform() = default;

    virtual ~Platform() = default;

    virtual bool Initialize(std::unique_ptr<Application> &&application) = 0;

    virtual void MainLoop() = 0;

    virtual void Terminate() = 0;

    virtual void Tick() = 0;

protected:

    std::unique_ptr<Application> m_CurrentApplication{ nullptr };

};