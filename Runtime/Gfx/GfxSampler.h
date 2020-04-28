#pragma once

#include "../Common/Utils.h"
//#include <algorithm>
#include <vector>
#include <string>
#include <memory>


struct SamplerDesc : : public NonCopyable
{
private:

    float m_MipLodBias = 0.0f;

    float m_MaxAnisotropy = 0.0f;

    float m_MinLod = 0.0f;

    float m_MaxLod = 0.0f;

    uint16_t mMagFilter : 1 = 1;

    uint16_t mMinFilter : 1 = 1;

    uint16_t mMipmapMode : 1 = 1;

    uint16_t mAddressModeU : 3 = 1;

    uint16_t mAddressModeV : 3 = 1;

    uint16_t mAddressModeW : 3 = 1;

    uint16_t mCompareEnabled : 1 = 1;

    uint16_t mCompareOp : 3 = 1;

    size_t Hash() const;

    bool operator==(const SamplerDesc &other) const;

};

static_assert(sizeof(SamplerDesc) == 18, "Unexpected SamplerDesc size");

namespace std
{
    template <>
    struct hash<SamplerDesc>
    {
        size_t operator()(const SamplerDesc &key) const { return key.Hash(); }
    };
}

class GfxSampler : public NonCopyable
{
public:

    GfxSampler();

protected:


};