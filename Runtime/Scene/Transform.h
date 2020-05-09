#pragma once
#include "Component.h"


class Transform : public Component
{

};

template <>
struct ComponentTraits<Transform>
{
    static const uint8_t id = 0;
};
