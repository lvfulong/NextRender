#include "../../Runtime/Application.h"
#include "../../Runtime/Gfx/Vulkan/VulkanGfx.h"

class HelloTriangle : public Application
{
private:

    std::unique_ptr<VulkanGfx> m_Gfx{ nullptr };

public:
    ~HelloTriangle()  override
    {

    }
};
std::unique_ptr<Application> CreateApplication()
{
    return std::make_unique<HelloTriangle>();
}