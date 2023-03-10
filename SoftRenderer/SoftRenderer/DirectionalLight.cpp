#include "DirectionalLight.h"


FVector3 UDirectionalLight::GetLightDirection(const FVector4& WorldPos) const
{
	return Transform.GetForwardDirection() * -1.f;
}