#include "Application.h"

const std::string& Application::GetName() const
{
    return m_Name;
}

void Application::SetName(const std::string &name)
{
    m_Name = name;
}