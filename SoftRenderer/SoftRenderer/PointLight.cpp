#include "PointLight.h"

float UPointLight::GetAttenuationFactor(const FVector4& WorldPos) const
{
    const float Distance = (WorldPos - GetLightWorldPos()).Len();
    if(Distance > AttenuationRadius) return 0.f;
    
    float Ret = (AttenuationRadius - Distance) / (AttenuationRadius - SourceRadius);
    if(Ret > 1) Ret = 1;
    return Ret;
}
