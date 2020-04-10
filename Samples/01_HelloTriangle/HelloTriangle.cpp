#include "../../Runtime/Application.h"

class HelloTriangle : public Application
{
public:
    ~HelloTriangle()  override
    {

    }
};
std::unique_ptr<Application> CreateApplication()
{
    return std::make_unique<HelloTriangle>();
}