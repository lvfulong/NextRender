#pragma once

#include <string>

class Application
{
public:

    Application() = default;

    virtual ~Application() = default;

    const std::string& GetName() const;

    void SetName(const std::string& name);

    //virtual void createRender() = 0;
    //virtual void init() = 0;
    //virtual int shutDown() = 0;
    //virtual bool update() = 0;

private:

    std::string m_Name{};
};