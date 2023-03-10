#include "LightBase.h"

FVector3 ULightBase::GetLightDirection(const FVector4& WorldPos) const
{
    FVector3 Ret = (GetLightWorldPos() - WorldPos);
    Ret.Normalize();
    return Ret;
}

float ULightBase::GetAttenuationFactor(const FVector4& WorldPos) const
{
    return 1.f;
}

FVector4 ULightBase::GetLightWorldPos() const
{
    return Transform.GetLocation();
}
