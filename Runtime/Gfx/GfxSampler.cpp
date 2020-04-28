#include "GfxSampler.h"


size_t SamplerDesc::hash() const
{
    return angle::ComputeGenericHash(*this);
}

bool SamplerDesc::operator==(const SamplerDesc &other) const
{
    return (memcmp(this, &other, sizeof(SamplerDesc)) == 0);
}

GfxSampler::GfxSampler()
{

}

