#pragma once
#include "Component.h"
#include <array>

class GameObject
{
public:

    template <typename T>
    T *GetComponent();

private:

    std::array<Component *, 10> m_Components{nullptr};//TODO
};