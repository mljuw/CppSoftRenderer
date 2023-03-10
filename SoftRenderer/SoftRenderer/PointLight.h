#pragma once
#include "LightBase.h"

class UPointLight : public ULightBase
{
protected:
    float SourceRadius = 200.f;
    float AttenuationRadius = 500.f;

public:
    UPointLight()
    {
        LightType = ELightType::Point;
    }
    void SetSourceRadius(float Value)
    {
        SourceRadius = Value;
        if(SourceRadius > AttenuationRadius)
        {
            AttenuationRadius = SourceRadius;
        }
    }

    void SetAttenuationRadius(float Value)
    {
        AttenuationRadius = Value;
        if(SourceRadius > AttenuationRadius)
        {
            AttenuationRadius = SourceRadius;
        }
    }
    
    float GetAttenuationFactor(const FVector4& WorldPos) const override;
    
};
