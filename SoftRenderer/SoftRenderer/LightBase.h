#pragma once
#include "BaseStruct.h"

enum class ELightType : unsigned char
{
	Directional,
	Point,
	Spot,
};

class ULightBase
{
public:
	ELightType LightType = ELightType::Directional;
	
	FTransform Transform;

	FVector4 LightColor = { 1.f, 1.f, 1.f, 1.f };

	virtual FVector3 GetLightDirection(const FVector4& WorldPos) const;

	virtual float GetAttenuationFactor(const FVector4& WorldPos) const;

	virtual FVector4 GetLightWorldPos() const;

	

};
