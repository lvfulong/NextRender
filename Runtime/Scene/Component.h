#pragma once
#include <limits>

template <typename T>
struct ComponentTraits
{
    static const uint8_t id = numeric_limits<uint8_t>::max();
};