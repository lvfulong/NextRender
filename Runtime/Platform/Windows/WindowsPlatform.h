#pragma once


#include "Platform.h"
#include <Windows.h>

struct GLFWwindow;

class WindowsPlatform : public Platform
{
public:

    WindowsPlatform(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow);

    ~WindowsPlatform();

    virtual bool Initialize(std::unique_ptr<Application>&& application) override;

    virtual void MainLoop() override;

    virtual void Terminate() override;

    virtual void Tick() override;

private:

    bool ShouldClose();

    void ProcesEvents();

private:

    GLFWwindow* m_Handle = nullptr;
};