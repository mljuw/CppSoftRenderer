#pragma once
#include "LightBase.h"

class UDirectionalLight : public ULightBase
{
public:

	FVector3 GetLightDirection(const FVector4& WorldPos) const override;
};
